/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: parallels
 */

#include <BeagleRT.h>
#include <cmath>
#include <Utilities.h>
#include <I2c_Codec.h>
#include <PRU.h>
#include <stdio.h>
#include "z_libpd.h"
#include <UdpServer.h>

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
#define DEFDACBLKSIZE 8u  //make sure this matches the one used to compile libpd

int gChannelsInUse = 10;
int gBufLength;

float* gInBuf;
float* gOutBuf;

void pdnoteon(int ch, int pitch, int vel) {
  printf("noteon: %d %d %d\n", ch, pitch, vel);
}

void BeagleRT_printHook(const char *recv){
	rt_printf("%s", recv);
}

UdpServer udpServer;

void udpRead(){
    char dest[100] = {0};
    while(!gShouldStop){
    	libpd_sys_microsleep(0);
        usleep(1000);
    }
}

AuxiliaryTask  udpReadTask;
bool setup(BeagleRTContext *context, void *userData)
{
	gChannelsInUse = min((int)(context->analogChannels+context->audioChannels), (int)gChannelsInUse);
	udpServer.bindToPort(1234);

	// check that we are not running with a blocksize smaller than DEFDACBLKSIZE
	// it would still work, but the load would be executed unevenly between calls to render
	if(context->audioFrames < DEFDACBLKSIZE){
		fprintf(stderr, "Error: minimum block size must be %d\n", DEFDACBLKSIZE);
		return false;
	}

	// check that the sampling rate of the analogs is the same as audio if running with
	// more than 2 channels (that is with analog). If we fix the TODO in render, then
	// this test is not needed.
//	if(context->analogFrames != context->audioFrames){
//		fprintf(stderr, "Error: analog and audio sampling rates must be the same\n");
//		return false;
//	}
	//following lines borrowed from libpd/samples/c/pdtest/pdtest.c
	// init pd
	libpd_set_printhook(BeagleRT_printHook); // set this before calling libpd_init
	libpd_set_noteonhook(pdnoteon);
	libpd_init();
	//TODO: analyse the ASCII of the patch file and find the in/outs to use
	libpd_init_audio(gChannelsInUse, gChannelsInUse, context->audioSampleRate);

	libpd_start_message(1); // one entry in list
	libpd_add_float(1.0f);
	libpd_finish_message("pd", "dsp");

	gBufLength = max(DEFDACBLKSIZE, context->audioFrames);
	unsigned int bufferSize = sizeof(float)*gChannelsInUse*gBufLength;
	gInBuf = (float*)malloc(bufferSize);
	gOutBuf = (float*)malloc(bufferSize);
	// no need to memset to zero

	char file[] = "_main.pd";
	char folder[] = "./";
	// open patch       [; pd open file folder(
	libpd_openfile(file, folder);
    
	udpReadTask = BeagleRT_createAuxiliaryTask(udpRead, 60, "udpReadTask");
	BeagleRT_scheduleAuxiliaryTask(udpReadTask);
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.
BeagleRTContext *c;
void render(BeagleRTContext *context, void *userData)
{
	static int inW = 0;
	static int outR = 0;
/*
 *	NOTE: if you are only using audio (or only analogs) and you are using interleaved buffers
 *	 and the blocksize of Bela is the same as DEFDACBLKSIZE, then you probably
 *	 do not need the for loops before and after libpd_process_float, so you can save quite some
 *	 memory operations.
 */
	static int analogChannelsInUse = min(context->analogChannels, gChannelsInUse - context->audioChannels);
	// rt_printf("channelsInUse: %d, analogChannels in Use: %d\n", gChannelsInUse, analogChannelsInUse);
	for(unsigned int n = 0; n < context->audioFrames; ++n){ //pd buffers are interleaved
		for(unsigned int ch = 0; ch < context->audioChannels; ++ch){ //first two channels are audio
			gInBuf[inW++] = audioReadFrame(context, n, ch);
		}
		// then analogs
		// this loop resamples by ZOH, as needed, using m
		if(context->analogChannels == 8 ){ //hold the value for two frames
			for(int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				gInBuf[inW++] = analogReadFrame(context, n/2, analogCh); // n/2 wil be the same for n and n+1 when n is even
			}
		} else if(context->analogChannels == 4){ //write every frame
			for(int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				gInBuf[inW++] = analogReadFrame(context, n, analogCh);
			}
		} else if(context->analogChannels == 2){ //drop every other frame
			for(int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				gInBuf[inW++] = analogReadFrame(context, n*2, analogCh);
			}
		}
		if(inW == gBufLength * gChannelsInUse){
			inW = 0;
		}
	}
	// rt_printf("inW %d\n", inW);
	if(inW == 0){ //if the buffer is full, process it
		static int numberOfPdBlocksToProcess = gBufLength/DEFDACBLKSIZE;
		libpd_process_float(numberOfPdBlocksToProcess, gInBuf, gOutBuf);
		outR = 0; // reset the read pointer. NOTE: hopefully this is needed only the first time
	}

	for(unsigned int n = 0; n < context->audioFrames; n++){ //pd buffers are interleaved
		for(unsigned int ch = 0; ch < context->audioChannels; ++ch){
			audioWriteFrame(context, n, ch, gOutBuf[outR++]);
		}
		//and analogs
		if(context->analogChannels == 8){
			for(unsigned int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				float analogOut = gOutBuf[outR++];				
				if((n&1) == 0){//write every two frames
					analogWriteFrame(context, n/2, analogCh, analogOut);
				} else {
					// discard this sample
				}
			}
		} else if(context->analogChannels == 4){ //write every frame
			for(int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				float analogOut = gOutBuf[outR++];
				analogWriteFrame(context, n, analogCh, analogOut);
			}
		} else if(context->analogChannels == 2){ //write twice every frame
			for(unsigned int analogCh = 0; analogCh < analogChannelsInUse; ++analogCh){
				float analogOut = gOutBuf[outR++];
				analogWriteFrame(context, 2*n, analogCh, analogOut);
				analogWriteFrame(context, 2*n + 1, analogCh, analogOut);
			}
		}
		if(outR == gBufLength * gChannelsInUse){
			outR = 0;
		}
	}
	// rt_printf("outR %d, analogChannelsInUse %d, channelsInUse %d\n", 
	// 	outR , analogChannelsInUse, gChannelsInUse);
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{
	free(gInBuf);
	free(gOutBuf);
}
