/*
 * render.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: Andrew McPherson and Victor Zappi
 */


#include <BeagleRT.h>	// to schedule lower prio parallel process
#include <rtdk.h>
#include <cmath>
#include <stdio.h>
#include "SampleData.h"

SampleData gSampleData;	// User defined structure to get complex data from main
int gReadPtr;			// Position of last read sample from file

// filter vars
float gLastX[2];
float gLastY[2];
double lb0, lb1, lb2, la1, la2 = 0.0;

// communication vars between the 2 auxiliary tasks
int gChangeCoeff = 0;
int gFreqDelta = 0;

void initialise_filter(float freq);

void calculate_coeff(float cutFreq);

bool initialise_aux_tasks();

// Task for handling the update of the frequencies using the matrix
AuxiliaryTask gChangeCoeffTask;

void check_coeff();

// Task for handling the update of the frequencies using the matrix
AuxiliaryTask gInputTask;

void read_input();


extern float gCutFreq;


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

	initialise_filter(200);

	// Initialise auxiliary tasks
	if(!initialise_aux_tasks())
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
		float sample = 0;
		float out = 0;

		// If triggered...
		if(gReadPtr != -1)
			sample += gSampleData.samples[gReadPtr++];	// ...read each sample...

		if(gReadPtr >= gSampleData.sampleLen)
			gReadPtr = -1;

		out = lb0*sample+lb1*gLastX[0]+lb2*gLastX[1]-la1*gLastY[0]-la2*gLastY[1];

		gLastX[1] = gLastX[0];
		gLastX[0] = out;
		gLastY[1] = gLastY[0];
		gLastY[0] = out;

		for(unsigned int channel = 0; channel < context->audioChannels; channel++)
			context->audioOut[n * context->audioChannels + channel] = out;	// ...and put it in both left and right channel

	}

	// Request that the lower-priority tasks run at next opportunity
	BeagleRT_scheduleAuxiliaryTask(gChangeCoeffTask);
	BeagleRT_scheduleAuxiliaryTask(gInputTask);
}

// First calculation of coefficients

void initialise_filter(float freq)
{
	calculate_coeff(freq);
}


// Calculate the filter coefficients
// second order low pass butterworth filter

void calculate_coeff(float cutFreq)
{
	// Initialise any previous state (clearing buffers etc.)
	// to prepare for calls to render()
	float sampleRate = 44100;
	double f = 2*M_PI*cutFreq/sampleRate;
	double denom = 4+2*sqrt(2)*f+f*f;
	lb0 = f*f/denom;
	lb1 = 2*lb0;
	lb2 = lb0;
	la1 = (2*f*f-8)/denom;
	la2 = (f*f+4-2*sqrt(2)*f)/denom;
	gLastX[0] = gLastX [1] = 0;
	gLastY[0] = gLastY[1] = 0;

}


// Initialise the auxiliary tasks
// and print info

bool initialise_aux_tasks()
{
	if((gChangeCoeffTask = BeagleRT_createAuxiliaryTask(&check_coeff, 90, "beaglert-check-coeff")) == 0)
		return false;

	if((gInputTask = BeagleRT_createAuxiliaryTask(&read_input, 50, "beaglert-read-input")) == 0)
		return false;

	rt_printf("Press 'a' to trigger sample, 's' to stop\n");
	rt_printf("Press 'z' to low down cut-off freq of 100 Hz, 'x' to raise it up\n");
	rt_printf("Press 'q' to quit\n");

	return true;
}

// Check if cut-off freq has been changed
// and new coefficients are needed

void check_coeff()
{
	if(gChangeCoeff == 1)
	{
		gCutFreq += gFreqDelta;
		gCutFreq = gCutFreq < 0 ? 0 : gCutFreq;
		gCutFreq = gCutFreq > 22050 ? 22050 : gCutFreq;

		rt_printf("Cut-off frequency: %f\n", gCutFreq);

		calculate_coeff(gCutFreq);
		gChangeCoeff = 0;
	}
}

// This is a lower-priority call to periodically read keyboard input
// and trigger samples. By placing it at a lower priority,
// it has minimal effect on the audio performance but it will take longer to
// complete if the system is under heavy audio load.

void read_input()
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
		case 'z':
			gChangeCoeff = 1;
			gFreqDelta = -100;
			break;
		case 'x':
			gChangeCoeff = 1;
			gFreqDelta = 100;
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
}
