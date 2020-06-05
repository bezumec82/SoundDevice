/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: Andrew McPherson and Victor Zappi
 */

#include <iostream>
#include <cstdlib>
#include <libgen.h>
#include <signal.h>
#include <string>
#include <getopt.h>
#include <sndfile.h>				// to load audio files

#include <BeagleRT.h>
#include "SampleData.h"

using namespace std;

float gCutFreq = 100;

// Load samples from file
int initFile(string file, SampleData *smp)//float *& smp)
{
	SNDFILE *sndfile ;
	SF_INFO sfinfo ;

	if (!(sndfile = sf_open (file.c_str(), SFM_READ, &sfinfo))) {
		cout << "Couldn't open file " << file << endl;
		return 1;
	}

	int numChan = sfinfo.channels;
	if(numChan != 1)
	{
		cout << "Error: " << file << " is not a mono file" << endl;
		return 1;
	}

	smp->sampleLen = sfinfo.frames * numChan;
	smp->samples = new float[smp->sampleLen];
	if(smp == NULL){
		cout << "Could not allocate buffer" << endl;
		return 1;
	}

	int subformat = sfinfo.format & SF_FORMAT_SUBMASK;
	int readcount = sf_read_float(sndfile, smp->samples, smp->sampleLen);

	// Pad with zeros in case we couldn't read whole file
	for(int k = readcount; k <smp->sampleLen; k++)
		smp->samples[k] = 0;

	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE) {
		double	scale ;
		int 	m ;

		sf_command (sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
		if (scale < 1e-10)
			scale = 1.0 ;
		else
			scale = 32700.0 / scale ;
		cout << "File samples scale = " << scale << endl;

		for (m = 0; m < smp->sampleLen; m++)
			smp->samples[m] *= scale;
	}

	sf_close(sndfile);

	return 0;
}

// Handle Ctrl-C by requesting that the audio rendering stop
void interrupt_handler(int var)
{
	//rt_task_delete ((RT_TASK *) &gTriggerSamplesTask);
	gShouldStop = true;
}

// Print usage information
void usage(const char * processName)
{
	cerr << "Usage: " << processName << " [options]" << endl;

	BeagleRT_usage();

	cerr << "   --file [-f] filename:    Name of the file to load (default is \"longsample.wav\")\n";
	cerr << "   --cutfreq [-c] freq:     Set the cut off frequency of the filter in Hz\n";
	cerr << "   --help [-h]:             Print this menu\n";
}

int main(int argc, char *argv[])
{
	BeagleRTInitSettings settings;	// Standard audio settings
	string fileName;			// Name of the sample to load

	SampleData sampleData;		// User define structure to pass data retrieved from file to render function
	sampleData.samples = 0;
	sampleData.sampleLen = -1;


	struct option customOptions[] =
	{
		{"help", 0, NULL, 'h'},
		{"cutfreq", 1, NULL, 'c'},
		{"file", 1, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	// Set default settings
	BeagleRT_defaultSettings(&settings);

	// Parse command-line arguments
	while (1) {
		int c;
		if ((c = BeagleRT_getopt_long(argc, argv, "hf:c:", customOptions, &settings)) < 0)
				break;
		switch (c) {
		case 'h':
				usage(basename(argv[0]));
				exit(0);
		case 'f':
				fileName = string((char *)optarg);
				break;
		case 'c':
				gCutFreq = atof(optarg);
				break;
		case '?':
		default:
				usage(basename(argv[0]));
				exit(1);
		}
	}

	if(fileName.empty()){
		fileName = "filter/longsample.wav";
	}

	if(settings.verbose) {
		cout << "Loading file " << fileName << endl;
	}

	// Load file
	if(initFile(fileName, &sampleData) != 0)
	{
		cout << "Error: unable to load samples " << endl;
		return -1;
	}

	if(settings.verbose)
		cout << "File contains " << sampleData.sampleLen << " samples" << endl;


	// Initialise the PRU audio device
	if(BeagleRT_initAudio(&settings, &sampleData) != 0) {
		cout << "Error: unable to initialise audio" << endl;
		return -1;
	}

	// Start the audio device running
	if(BeagleRT_startAudio()) {
		cout << "Error: unable to start real-time audio" << endl;
		return -1;
	}

	// Set up interrupt handler to catch Control-C and SIGTERM
	signal(SIGINT, interrupt_handler);
	signal(SIGTERM, interrupt_handler);

	// Run until told to stop
	while(!gShouldStop) {
		usleep(100000);
	}

	// Stop the audio device
	BeagleRT_stopAudio();

	// Clean up any resources allocated for audio
	BeagleRT_cleanupAudio();

	// All done!
	return 0;
}

