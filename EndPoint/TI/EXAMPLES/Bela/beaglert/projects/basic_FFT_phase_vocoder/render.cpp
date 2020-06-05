/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\.io

 */

/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: parallels
 */

/**
\example 4_audio_FFT_phase_vocoder

Phase Vocoder
----------------------

This sketch shows an implementation of a phase vocoder and builds on the previous FFT example.
Again it uses the NE10 library, included at the top of the file (line 11).

Read the documentation on the NE10 library [here](http://projectne10.github.io/Ne10/doc/annotated.html).

*/


#include <BeagleRT.h>
#include <rtdk.h>
#include <NE10.h>					// NEON FFT library
#include <cmath>
#include "SampleData.h"
#include <Midi.h>

#define BUFFER_SIZE 16384

// TODO: your buffer and counter go here!
float gInputBuffer[BUFFER_SIZE];
int gInputBufferPointer = 0;
float gOutputBuffer[BUFFER_SIZE];
int gOutputBufferWritePointer = 0;
int gOutputBufferReadPointer = 0;
int gSampleCount = 0;

float *gWindowBuffer;

// -----------------------------------------------
// These variables used internally in the example:
int gFFTSize = 2048;
int gHopSize = 512;
int gPeriod = 512;
float gFFTScaleFactor = 0;

// FFT vars
ne10_fft_cpx_float32_t* timeDomainIn;
ne10_fft_cpx_float32_t* timeDomainOut;
ne10_fft_cpx_float32_t* frequencyDomain;
ne10_fft_cfg_float32_t cfg;

// Sample info
SampleData gSampleData;	// User defined structure to get complex data from main
int gReadPtr = 0;		// Position of last read sample from file

// Auxiliary task for calculating FFT
AuxiliaryTask gFFTTask;
int gFFTInputBufferPointer = 0;
int gFFTOutputBufferPointer = 0;

void process_fft_background();


int gEffect = 0; // change this here or with midi CC
enum{
	kBypass,
	kRobot,
	kWhisper,
};

float gDryWet = 1; // mix between the unprocessed and processed sound
float gPlaybackLive = 0.5f; // mix between the file playback and the live audio input
float gGain = 1; // overall gain
Midi midi;
void midiCallback(MidiChannelMessage message, void* arg){
	if(message.getType() == kmmNoteOn){
		if(message.getDataByte(1) > 0){
			int note = message.getDataByte(0);
			float frequency = powf(2, (note-69)/12.f)*440;
			gPeriod = (int)(44100 / frequency + 0.5);
			printf("\nnote: %d, frequency: %f, hop: %d\n", note, frequency, gPeriod);
		}
	}

	bool shouldPrint = false;
	if(message.getType() == kmmControlChange){
		float data = message.getDataByte(1) / 127.0f;
		switch (message.getDataByte(0)){
		case 2 :
			gEffect = (int)(data * 2 + 0.5); // CC2 selects an effect between 0,1,2
			break;
		case 3 :
			gPlaybackLive = data;
			break;
		case 4 :
			gDryWet = data;
			break;
		case 5:
			gGain = data*10;
			break;
		default:
			shouldPrint = true;
		}
	}
	if(shouldPrint){
		message.prettyPrint();
	}
}

// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
bool setup(BeagleRTContext* context, void* userData)
{
	midi.readFrom(0);
	midi.setParserCallback(midiCallback);
	// Retrieve a parameter passed in from the initAudio() call
	gSampleData = *(SampleData *)userData;

	gFFTScaleFactor = 1.0f / (float)gFFTSize;
	gOutputBufferWritePointer += gHopSize;

	timeDomainIn = (ne10_fft_cpx_float32_t*) NE10_MALLOC (gFFTSize * sizeof (ne10_fft_cpx_float32_t));
	timeDomainOut = (ne10_fft_cpx_float32_t*) NE10_MALLOC (gFFTSize * sizeof (ne10_fft_cpx_float32_t));
	frequencyDomain = (ne10_fft_cpx_float32_t*) NE10_MALLOC (gFFTSize * sizeof (ne10_fft_cpx_float32_t));
	cfg = ne10_fft_alloc_c2c_float32 (gFFTSize);

	memset(timeDomainOut, 0, gFFTSize * sizeof (ne10_fft_cpx_float32_t));
	memset(gOutputBuffer, 0, BUFFER_SIZE * sizeof(float));

	// Allocate the window buffer based on the FFT size
	gWindowBuffer = (float *)malloc(gFFTSize * sizeof(float));
	if(gWindowBuffer == 0)
		return false;

	// Calculate a Hann window
	for(int n = 0; n < gFFTSize; n++) {
		gWindowBuffer[n] = 0.5f * (1.0f - cosf(2.0 * M_PI * n / (float)(gFFTSize - 1)));
	}

	// Initialise auxiliary tasks
	if((gFFTTask = BeagleRT_createAuxiliaryTask(&process_fft_background, 90, "fft-calculation")) == 0)
		return false;
	rt_printf("You are listening to an FFT phase-vocoder with overlap-and-add.\n"
	"Use Midi Control Change to control:\n"
	"CC 2: effect type (bypass/robotization/whisperization)\n"
	"CC 3: mix between recorded sample and live audio input\n"
	"CC 4: mix between the unprocessed and processed sound\n"
	"CC 5: gain\n"
	);
	return true;
}

// This function handles the FFT processing in this example once the buffer has
// been assembled.
void process_fft(float *inBuffer, int inWritePointer, float *outBuffer, int outWritePointer)
{
	// Copy buffer into FFT input
	int pointer = (inWritePointer - gFFTSize + BUFFER_SIZE) % BUFFER_SIZE;
	for(int n = 0; n < gFFTSize; n++) {
		timeDomainIn[n].r = (ne10_float32_t) inBuffer[pointer] * gWindowBuffer[n];
		timeDomainIn[n].i = 0;

		pointer++;
		if(pointer >= BUFFER_SIZE)
			pointer = 0;
	}

	// Run the FFT
	ne10_fft_c2c_1d_float32_neon (frequencyDomain, timeDomainIn, cfg->twiddles, cfg->factors, gFFTSize, 0);

	switch (gEffect){
	  case kRobot :
	  // Robotise the output
	    for(int n = 0; n < gFFTSize; n++) {
	      float amplitude = sqrtf(frequencyDomain[n].r * frequencyDomain[n].r + frequencyDomain[n].i * frequencyDomain[n].i);
	      frequencyDomain[n].r = amplitude;
	      frequencyDomain[n].i = 0;
	    }
	    break;
	  case kWhisper :
	    for(int n = 0; n < gFFTSize; n++) {
	      float amplitude = sqrtf(frequencyDomain[n].r * frequencyDomain[n].r + frequencyDomain[n].i * frequencyDomain[n].i);
	      float phase = rand()/(float)RAND_MAX * 2 * M_PI;
	      frequencyDomain[n].r = cosf(phase) * amplitude;
	      frequencyDomain[n].i = sinf(phase) * amplitude;
	    }
	    break;
	  case kBypass:
	    //bypass
	    break;
	  }

	// Run the inverse FFT
	ne10_fft_c2c_1d_float32_neon (timeDomainOut, frequencyDomain, cfg->twiddles, cfg->factors, gFFTSize, 1);
	// Overlap-and-add timeDomainOut into the output buffer
	pointer = outWritePointer;
	for(int n = 0; n < gFFTSize; n++) {
		outBuffer[pointer] += (timeDomainOut[n].r) * gFFTScaleFactor;
		if(isnan(outBuffer[pointer]))
			rt_printf("outBuffer OLA\n");
		pointer++;
		if(pointer >= BUFFER_SIZE)
			pointer = 0;
	}
}

// Function to process the FFT in a thread at lower priority
void process_fft_background() {
	process_fft(gInputBuffer, gFFTInputBufferPointer, gOutputBuffer, gFFTOutputBufferPointer);
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.
void render(BeagleRTContext* context, void* userData)
{
	float* audioIn = context->audioIn;
	float* audioOut = context->audioOut;
	int numAudioFrames = context->audioFrames;
	int numAudioChannels = context->audioChannels;
	// ------ this code internal to the demo; leave as is ----------------

	// Prep the "input" to be the sound file played in a loop
	for(int n = 0; n < numAudioFrames; n++) {
		if(gReadPtr < gSampleData.sampleLen)
			audioIn[2*n] = audioIn[2*n+1] = gSampleData.samples[gReadPtr]*(1-gPlaybackLive) +
			gPlaybackLive*0.5f*(audioReadFrame(context,n,0)+audioReadFrame(context,n,1));
		else
			audioIn[2*n] = audioIn[2*n+1] = 0;
		if(++gReadPtr >= gSampleData.sampleLen)
			gReadPtr = 0;
	}
	// -------------------------------------------------------------------

	for(int n = 0; n < numAudioFrames; n++) {
		gInputBuffer[gInputBufferPointer] = ((audioIn[n*numAudioChannels] + audioIn[n*numAudioChannels+1]) * 0.5);

		// Copy output buffer to output
		for(int channel = 0; channel < numAudioChannels; channel++){
			audioOut[n * numAudioChannels + channel] = gOutputBuffer[gOutputBufferReadPointer] * gGain * gDryWet + (1 - gDryWet) * audioIn[n * numAudioChannels + channel];
		}

		// Clear the output sample in the buffer so it is ready for the next overlap-add
		gOutputBuffer[gOutputBufferReadPointer] = 0;
		gOutputBufferReadPointer++;
		if(gOutputBufferReadPointer >= BUFFER_SIZE)
			gOutputBufferReadPointer = 0;
		gOutputBufferWritePointer++;
		if(gOutputBufferWritePointer >= BUFFER_SIZE)
			gOutputBufferWritePointer = 0;

		gInputBufferPointer++;
		if(gInputBufferPointer >= BUFFER_SIZE)
			gInputBufferPointer = 0;

		gSampleCount++;
		if(gSampleCount >= gHopSize) {
			//process_fft(gInputBuffer, gInputBufferPointer, gOutputBuffer, gOutputBufferPointer);
			gFFTInputBufferPointer = gInputBufferPointer;
			gFFTOutputBufferPointer = gOutputBufferWritePointer;
			BeagleRT_scheduleAuxiliaryTask(gFFTTask);

			gSampleCount = 0;
		}
	}
	gHopSize = gPeriod;
}

// cleanup_render() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in initialise_render().

void cleanup(BeagleRTContext* context, void* userData)
{
	NE10_FREE(timeDomainIn);
	NE10_FREE(timeDomainOut);
	NE10_FREE(frequencyDomain);
	NE10_FREE(cfg);
	free(gWindowBuffer);
}
