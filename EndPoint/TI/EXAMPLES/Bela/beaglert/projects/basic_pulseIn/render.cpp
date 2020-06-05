/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: parallels
 */

#include <BeagleRT.h>
#include <PulseIn.h>
#include <Utilities.h>
#include <stdlib.h>
#include <rtdk.h>
#include <cmath>

// setup() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
PulseIn pulseIn;
int gPulseInPin = 0;
int gDigitalOutPin = 1;
int gPulseLength = 1234;
int gSamplesBetweenPulses = 10000;

bool setup(BeagleRTContext *context, void *userData)
{
	pinModeFrame(context, 0, gDigitalOutPin, OUTPUT);
	pulseIn.init(context, gPulseInPin, 1); //third parameter is direction
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	static bool pulseOut = 0;
	static int count = 0;
	for(unsigned int n = 0; n < context->digitalFrames; n++){
		// detect if a pulse just ended
		int duration = pulseIn.hasPulsed(context, n);
		if(duration > 0){
			rt_printf("duration = %d\n", duration);
		}

		// generate a rectangular waveform as a test signal.
		// Connect gDigitalOutPin to gPulseInPin
		// to verify that the detected pulse length is gPulseLength
		if(count == gPulseLength ){
			pulseOut = false;
		}
		if(count == (gPulseLength + gSamplesBetweenPulses)){
			pulseOut = true;
			count = 0;
		}
		digitalWriteFrame(context, n, gDigitalOutPin, pulseOut);
		count++;
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{

}
