/*
 * main.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: parallels
 */

#include <iostream>
#include <cstdlib>
#include <libgen.h>
#include <signal.h>
#include <getopt.h>
#include <sndfile.h>
#include <BeagleRT.h>

extern int gScreenFramesPerSecond;

float *gMusicBuffer = 0;
int gMusicBufferLength = 0;
float *gSoundBoomBuffer = 0;
int gSoundBoomBufferLength = 0;

using namespace std;

// Load a sound sample from file
int loadSoundFile(const string& path, float **buffer, int *bufferLength)
{
	SNDFILE *sndfile ;
	SF_INFO sfinfo ;

	if (!(sndfile = sf_open (path.c_str(), SFM_READ, &sfinfo))) {
		cout << "Couldn't open file " << path << endl;
		return 1;
	}

	int numChan = sfinfo.channels;
	if(numChan != 1)
	{
		cout << "Error: " << path << " is not a mono file" << endl;
		return 1;
	}

	*bufferLength = sfinfo.frames * numChan;
	*buffer = new float[*bufferLength];
	if(*buffer == 0){
		cout << "Could not allocate buffer" << endl;
		return 1;
	}

	int subformat = sfinfo.format & SF_FORMAT_SUBMASK;
	int readcount = sf_read_float(sndfile, *buffer, *bufferLength);

	// Pad with zeros in case we couldn't read whole file
	for(int k = readcount; k < *bufferLength; k++)
		(*buffer)[k] = 0;

	sf_close(sndfile);
	return 0;
}

// Handle Ctrl-C by requesting that the audio rendering stop
void interrupt_handler(int var)
{
	gShouldStop = true;
}

// Print usage information
void usage(const char * processName)
{
	cerr << "Usage: " << processName << " [options]" << endl;

	BeagleRT_usage();

	cerr << "   --fps [-f] value:           Set target frames per second\n";
	cerr << "   --help [-h]:                Print this menu\n";
}

int main(int argc, char *argv[])
{
	BeagleRTInitSettings settings;	// Standard audio settings
	string musicFileName = "music.wav";
	string soundBoomFileName = "boom.wav";

	struct option customOptions[] =
	{
		{"help", 0, NULL, 'h'},
		{"fps", 1, NULL, 'f'},
		{NULL, 0, NULL, 0}
	};

	// Set default settings
	BeagleRT_defaultSettings(&settings);

	// Parse command-line arguments
	while (1) {
		int c;
		if ((c = BeagleRT_getopt_long(argc, argv, "hf:", customOptions, &settings)) < 0)
				break;
		switch (c) {
		case 'f':
				gScreenFramesPerSecond = atoi(optarg);
				if(gScreenFramesPerSecond < 1)
					gScreenFramesPerSecond = 1;
				if(gScreenFramesPerSecond > 100)
					gScreenFramesPerSecond = 100;
				break;
		case 'h':
				usage(basename(argv[0]));
				exit(0);
		case '?':
		default:
				usage(basename(argv[0]));
				exit(1);
		}
	}

	// Load the sound files
	if(loadSoundFile(musicFileName, &gMusicBuffer, &gMusicBufferLength) != 0) {
		cout << "Warning: unable to load sound file " << musicFileName << endl;
	}
	if(loadSoundFile(soundBoomFileName, &gSoundBoomBuffer, &gSoundBoomBufferLength) != 0) {
		cout << "Warning: unable to load sound file " << soundBoomFileName << endl;
	}

	// Initialise the PRU audio device
	if(BeagleRT_initAudio(&settings, 0) != 0) {
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

	// Release sound files
	if(gMusicBuffer != 0)
		free(gMusicBuffer);
	if(gSoundBoomBuffer != 0)
		free(gSoundBoomBuffer);

	// All done!
	return 0;
}
