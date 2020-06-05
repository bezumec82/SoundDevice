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
\example 1_basic_helloworld

Producing your first bleep!
---------------------------

This sketch is the hello world of embedded interactive audio. Better known as bleep, it 
produces a sine tone.

The frequency of the sine tone is determined by a global variable, `gFrequency` 
(line 12). The sine tone is produced by incrementing the phase of a sin function 
on every audio frame.

The important thing to notice is the nested `for` loop structure. You will see 
this in all Bela projects and in most digital audio applications. The first `for`
loop cycles through the audio frames, the second through each of the audio
channels (in this case left 0 and right 1). It is good to familiarise yourself
with this structure as it is fundamental to producing sound with the system.
*/


#include <BeagleRT.h>
#include <cmath>
#include <Utilities.h>

float gFrequency = 440.0;
float gPhase;
float gInverseSampleRate;

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
bool setup(BeagleRTContext *context, void *userData)
{
	// Retrieve a parameter passed in from the initAudio() call
	if(userData != 0)
		gFrequency = *(float *)userData;

	gInverseSampleRate = 1.0 / context->audioSampleRate;
	gPhase = 0.0;

	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float out = 0.8f * sinf(gPhase);
		gPhase += 2.0 * M_PI * gFrequency * gInverseSampleRate;
		if(gPhase > 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;

		for(unsigned int channel = 0; channel < context->audioChannels; channel++) {
			// Two equivalent ways to write this code

			// The long way, using the buffers directly:
			// context->audioOut[n * context->audioChannels + channel] = out;

			// Or using the macros:
			audioWriteFrame(context, n, channel, out);
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{

}
