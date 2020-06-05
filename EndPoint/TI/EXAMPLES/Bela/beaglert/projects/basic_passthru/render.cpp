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
\example 1_basic_audio_passthrough

Audio passthrough: input to output
-----------------------------------------

This sketch demonstrates the simplest possible case of using audio: it passes 
audio input straight to audio output.

Note the nested `for` loop structure. You will see this in all Bela projects. The 
first `for` loop cycles through the audio frames, the second through each of the 
audio channels (in this case left 0 and right 1).


We write samples to the audio output buffer like this: 
`context->audioOut[n * context->audioChannels + ch]` where `n` is the current audio 
frame and `ch` is the current channel, both provided by the nested for loop structure.

We can access samples in the audio input buffer in a similar way, like this: 
`context->audioIn[n * context->audioChannels + ch]`.

So a simple audio pass through is achieved by setting output buffer equal to 
input buffer: 
`context->audioOut[n * context->audioChannels + ch] = context->audioIn[n * context->audioChannels + ch]`.


*/





#include <BeagleRT.h>
#include <Utilities.h>
#include <rtdk.h>

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
	// Nothing to do here...
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	// Simplest possible case: pass inputs through to outputs
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		for(unsigned int ch = 0; ch < context->audioChannels; ch++){
			// Two equivalent ways to write this code

			// The long way, using the buffers directly:
			// context->audioOut[n * context->audioChannels + ch] =
			// 		context->audioIn[n * context->audioChannels + ch];

			// Or using the macros:
			audioWriteFrame(context, n, ch, audioReadFrame(context, n, ch));
		}
	}

	// Same with analog channelss
	for(unsigned int n = 0; n < context->analogFrames; n++) {
		for(unsigned int ch = 0; ch < context->analogChannels; ch++) {
			// Two equivalent ways to write this code

			// The long way, using the buffers directly:
			// context->analogOut[n * context->analogChannels + ch] = context->analogIn[n * context->analogChannels + ch];

			// Or using the macros:
			analogWriteFrame(context, n, ch, analogReadFrame(context, n, ch));
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{

}
