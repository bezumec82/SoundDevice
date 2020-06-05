/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: Andrew McPherson and Victor Zappi
 */


#include <BeagleRT.h>
#include <cmath>
#include <NE10.h>					// neon library
#include "SampleData.h"
#include "FIRfilter.h"

SampleData gSampleData;	// User defined structure to get complex data from main
int gReadPtr;			// Position of last read sample from file

// filter vars
ne10_fir_instance_f32_t gFIRfilter;
ne10_float32_t *gFIRfilterIn;
ne10_float32_t *gFIRfilterOut;
ne10_uint32_t blockSize;
ne10_float32_t *gFIRfilterState;

void initialise_filter(BeagleRTContext *context);

// Task for handling the update of the frequencies using the matrix
AuxiliaryTask gTriggerSamplesTask;

bool initialise_trigger();
void trigger_samples();

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
	gSampleData = *(SampleData *)userData;

	gReadPtr = -1;

	initialise_filter(context);

	// Initialise auxiliary tasks
	if(!initialise_trigger())
		return false;

	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
	for(unsigned int n = 0; n < context->audioFrames; n++) {
		float in = 0;

		// If triggered...
		if(gReadPtr != -1)
			in += gSampleData.samples[gReadPtr++];	// ...read each sample...

		if(gReadPtr >= gSampleData.sampleLen)
			gReadPtr = -1;

		gFIRfilterIn[n] = in;
	}

	ne10_fir_float_neon(&gFIRfilter, gFIRfilterIn, gFIRfilterOut, blockSize);

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		for(unsigned int channel = 0; channel < context->audioChannels; channel++)
				context->audioOut[n * context->audioChannels + channel] = gFIRfilterOut[n];	// ...and put it in both left and right channel
	}


	// Request that the lower-priority task run at next opportunity
	BeagleRT_scheduleAuxiliaryTask(gTriggerSamplesTask);
}

// Initialise NE10 data structures to define FIR filter

void initialise_filter(BeagleRTContext *context)
{
	blockSize = context->audioFrames;
	gFIRfilterState	= (ne10_float32_t *) NE10_MALLOC ((FILTER_TAP_NUM+blockSize-1) * sizeof (ne10_float32_t));
	gFIRfilterIn = (ne10_float32_t *) NE10_MALLOC (blockSize * sizeof (ne10_float32_t));
	gFIRfilterOut = (ne10_float32_t *) NE10_MALLOC (blockSize * sizeof (ne10_float32_t));
	ne10_fir_init_float(&gFIRfilter, FILTER_TAP_NUM, filterTaps, gFIRfilterState, blockSize);
}


// Initialise the auxiliary task
// and print info

bool initialise_trigger()
{
	if((gTriggerSamplesTask = BeagleRT_createAuxiliaryTask(&trigger_samples, 50, "beaglert-trigger-samples")) == 0)
		return false;

	rt_printf("Press 'a' to trigger sample, 's' to stop\n");
	rt_printf("Press 'q' to quit\n");

	return true;
}

// This is a lower-priority call to periodically read keyboard input
// and trigger samples. By placing it at a lower priority,
// it has minimal effect on the audio performance but it will take longer to
// complete if the system is under heavy audio load.

void trigger_samples()
{
	// This is not a real-time task!
	// Cos getchar is a system call, not handled by Xenomai.
	// This task will be automatically down graded.

	char keyStroke = '.';

	keyStroke =	getchar();
	while(getchar()!='\n'); // to read the first stroke

	switch (keyStroke)
	{
		case 'a':
			gReadPtr = 0;
			break;
		case 's':
			gReadPtr = -1;
			break;
		case 'q':
			gShouldStop = true;
			break;
		default:
			break;
	}
}



// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{
	delete[] gSampleData.samples;

	NE10_FREE(gFIRfilterState);
	NE10_FREE(gFIRfilterIn);
	NE10_FREE(gFIRfilterOut);
}
