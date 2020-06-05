/*
 ____  _____ _        _    
| __ )| ____| |      / \   
|  _ \|  _| | |     / _ \  
| |_) | |___| |___ / ___ \ 
|____/|_____|_____/_/   \_\.io

 */

/*
 *
 * Andrew McPherson and Victor Zappi
 * Queen Mary, University of London
 */

/**
\example 3_analog_output

Fading LEDs
-----------

This sketch uses a sine wave to drive the brightness of a series of LEDs 
connected to the eight analog out pins. Again you can see the nested `for` loop 
structure but this time for the analog output channels rather than the audio.

Within the first `for` loop in render we cycle through each frame in the analog 
output matrix. At each frame we then cycle through the analog output channels 
with another `for` loop and set the output voltage according to the phase of a 
sine tone that acts as an LFO. The analog output pins can provide a voltage of 
~4.092V.

The output on each pin is set with `analogWriteFrame` within the `for` loop that 
cycles through the analog output channels. This needs to be provided with 
arguments as follows `analogWriteFrame(context, n, channel, out)`. Channel is 
where the you give the address of the analog output pin (in this case we cycle 
through each pin address in the for loop), out is the variable that holds the 
desired output (in this case set by the sine wave).

Notice that the phase of the brightness cycle for each led is different. This 
is achieved by updating a variable that stores a relative phase value. This 
variable is advanced by pi/4 (1/8 of a full rotation) for each channel giving 
each of the eight LEDs a different phase.

*/


#include <BeagleRT.h>
#include <Utilities.h>
#include <rtdk.h>
#include <cmath>

// Set range for analog outputs designed for driving LEDs
const float kMinimumAmplitude = (1.5 / 5.0);
const float kAmplitudeRange = 1.0 - kMinimumAmplitude;

float gFrequency;
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
	gFrequency = *(float *)userData;

	if(context->analogFrames == 0) {
		rt_printf("Error: this example needs the matrix enabled\n");
		return false;
	}

	gInverseSampleRate = 1.0 / context->analogSampleRate;
	gPhase = 0.0;

	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->analogFrames; n++) {
		// Set LED to different phase for each matrix channel
		float relativePhase = 0.0;
		for(unsigned int channel = 0; channel < context->analogChannels; channel++) {
			float out = kMinimumAmplitude + kAmplitudeRange * 0.5f * (1.0f + sinf(gPhase + relativePhase));

			analogWriteFrame(context, n, channel, out);

			// Advance by pi/4 (1/8 of a full rotation) for each channel
			relativePhase += M_PI * 0.25;
		}

		gPhase += 2.0 * M_PI * gFrequency * gInverseSampleRate;
		if(gPhase > 2.0 * M_PI)
			gPhase -= 2.0 * M_PI;
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{

}
