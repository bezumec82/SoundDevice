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
\example 3_analog_input

Connecting potentiometers
-------------------------

This sketch produces a sine tone, the frequency and amplitude of which are 
affected by data received on the analog pins. Before looping through each audio 
frame, we declare a value for the frequency and amplitude of our sine tone 
(line 55); we adjust these values by taking in data from analog sensors 
(for example, a potentiometer).

The important thing to notice is that audio is sampled twice as often as analog 
data. The audio sampling rate is 44.1kHz (44100 frames per second) and the 
analog sampling rate is 22.05kHz (22050 frames per second). On line 62 you might 
notice that we are processing the analog data and updating frequency and 
amplitude only on every second audio sample, since the analog sampling rate is 
half that of the audio.

Note that the pin numbers are stored in the variables `gAnalogInputFrequency` and 
`gAnalogInputAmplitude`. These are declared in the main.cpp file; if you look in 
that file you will see that they have the values of 0 and 1. Bear in mind that 
these are analog input pins which is a specific header!
*/

#include <BeagleRT.h>
#include <Utilities.h>
#include <rtdk.h>
#include <cmath>

float gPhase;
float gInverseSampleRate;
int gAudioFramesPerAnalogFrame;

// These settings are carried over from main.cpp
// Setting global variables is an alternative approach
// to passing a structure to userData in setup()

extern int gSensorInputFrequency;
extern int gSensorInputAmplitude;

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
	if(context->analogFrames == 0 || context->analogFrames > context->audioFrames) {
		rt_printf("Error: this example needs analog enabled, with 4 or 8 channels\n");
		return false;
	}

	gAudioFramesPerAnalogFrame = context->audioFrames / context->analogFrames;
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
	float frequency = 440.0;
	float amplitude = 0.8;

	// There are twice as many audio frames as matrix frames since audio sample rate
	// is twice as high

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		if(!(n % gAudioFramesPerAnalogFrame)) {
			// Even audio samples: update frequency and amplitude from the matrix
			frequency = map(analogReadFrame(context, n/gAudioFramesPerAnalogFrame, gSensorInputFrequency), 0, 1, 100, 1000);
			amplitude = analogReadFrame(context, n/gAudioFramesPerAnalogFrame, gSensorInputAmplitude);
		}

		float out = amplitude * sinf(gPhase);

		for(unsigned int channel = 0; channel < context->audioChannels; channel++)
			context->audioOut[n * context->audioChannels + channel] = out;

		gPhase += 2.0 * M_PI * frequency * gInverseSampleRate;
		if(gPhase > 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{

}
