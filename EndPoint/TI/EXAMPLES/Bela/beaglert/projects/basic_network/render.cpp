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
\example 5_basic_network

Networking 
----------

This sketch allows you to send audio and sensor data over UDP to a 
DAW on the host. The host needs to run Udpioplugin which you can f
ind [here](https://code.soundsoftware.ac.uk/projects/udpioplugin). 

Note that this sketch and the accompanying plugin are still in testing.

*/

#include <BeagleRT.h>
//#include <rtdk.h>
#include <cmath>
#include <NetworkSend.h>
#include <ReceiveAudioThread.h>
#include <Utilities.h>

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.

NetworkSend networkSend;
ReceiveAudioThread receive;
float gFrequency;
float gInverseSampleRate;
float gPhase;
bool setup(BeagleRTContext *context, void *userData)
{
	// Retrieve a parameter passed in from the initAudio() call
	gFrequency = *(float *)userData;

	networkSend.setup(context->audioSampleRate, context->audioFrames, 0, 9999, "192.168.7.1");
	receive.init(10000, context->audioFrames, 0);
	receive.startThread();
	gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0;
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = 0.7f * sinf(gPhase);
		gPhase += 2.0 * M_PI * gFrequency * gInverseSampleRate;
		if(gPhase > 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;

		networkSend.log(out);
		float in;
		int ret = receive.getSamplesSrc(&in, 1, 1);
		for(unsigned int channel = 0; channel < context->audioChannels; channel++){
			audioWriteFrame(context, n, channel, in);
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{
}
