/*
 *  RTAudio.cpp
 *
 *  Central control code for hard real-time audio on BeagleBone Black
 *  using PRU and Xenomai Linux extensions. This code began as part
 *  of the Hackable Instruments project (EPSRC) at Queen Mary University
 *  of London, 2013-14.
 *
 *  (c) 2014 Victor Zappi and Andrew McPherson
 *  Queen Mary University of London
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <iostream>
#include <assert.h>
#include <vector>

// Xenomai-specific includes
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <native/intr.h>
#include <rtdk.h>

#include "../include/BeagleRT.h"
#include "../include/PRU.h"
#include "../include/I2c_Codec.h"
#include "../include/GPIOcontrol.h"

// ARM interrupt number for PRU event EVTOUT7
#define PRU_RTAUDIO_IRQ		21

using namespace std;

// Data structure to keep track of auxiliary tasks we
// can schedule
typedef struct {
	RT_TASK task;
	void (*function)(void);
	char *name;
	int priority;
	bool started;
} InternalAuxiliaryTask;

const char gRTAudioThreadName[] = "beaglert-audio";
const char gRTAudioInterruptName[] = "beaglert-pru-irq";

// Real-time tasks and objects
RT_TASK gRTAudioThread;
#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
RT_INTR gRTAudioInterrupt;
#endif
PRU *gPRU = 0;
I2c_Codec *gAudioCodec = 0;

vector<InternalAuxiliaryTask*> &getAuxTasks(){
	static vector<InternalAuxiliaryTask*> auxTasks;
	return auxTasks;
}

// Flag which tells the audio task to stop
int gShouldStop = false;

// general settings
char gPRUFilename[MAX_PRU_FILENAME_LENGTH];		// Path to PRU binary file (internal code if empty)_
int gRTAudioVerbose = 0;   						// Verbosity level for debugging
int gAmplifierMutePin = -1;
int gAmplifierShouldBeginMuted = 0;

// Context which holds all the audio/sensor data passed to the render routines
BeagleRTContext gContext;

// User data passed in from main()
void *gUserData;

// initAudio() prepares the infrastructure for running PRU-based real-time
// audio, but does not actually start the calculations.
// periodSize indicates the number of audio frames per period: the analog period size
// will depend on the number of analog channels, in such a way that
// analogPeriodSize = 4*periodSize/numAnalogChannels
// In total, the audio latency in frames will be 2*periodSize,
// plus any latency inherent in the ADCs and DACs themselves.
// useAnalog indicates whether to enable the ADC and DAC or just use the audio codec.
// numAnalogChannels indicates how many ADC and DAC channels to use.
// userData is an opaque pointer which will be passed through to the setup()
// function for application-specific use
//
// Returns 0 on success.

int BeagleRT_initAudio(BeagleRTInitSettings *settings, void *userData)
{
	rt_print_auto_init(1);

	BeagleRT_setVerboseLevel(settings->verbose);
	strncpy(gPRUFilename, settings->pruFilename, MAX_PRU_FILENAME_LENGTH);
	gUserData = userData;

	// Initialise context data structure
	memset(&gContext, 0, sizeof(BeagleRTContext));

	if(gRTAudioVerbose) {
		cout << "Starting with period size " << settings->periodSize << "; ";
		if(settings->useAnalog)
			cout << "analog enabled\n";
		else
			cout << "analog disabled\n";
		cout << "DAC level " << settings->dacLevel << "dB; ADC level " << settings->adcLevel;
		cout << "dB; headphone level " << settings->headphoneLevel << "dB\n";
		if(settings->beginMuted)
			cout << "Beginning with speaker muted\n";
	}

	// Prepare GPIO pins for amplifier mute and status LED
	if(settings->ampMutePin >= 0) {
		gAmplifierMutePin = settings->ampMutePin;
		gAmplifierShouldBeginMuted = settings->beginMuted;

		if(gpio_export(settings->ampMutePin)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export amplifier mute pin " << settings-> ampMutePin << "\n";
		}
		if(gpio_set_dir(settings->ampMutePin, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on amplifier mute pin\n";
			return -1;
		}
		if(gpio_set_value(settings->ampMutePin, LOW)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on amplifier mute pin\n";
			return -1;
		}
	}

	// Limit the analog channels to sane values
	if(settings->numAnalogChannels >= 8)
		settings->numAnalogChannels = 8;
	else if(settings->numAnalogChannels >= 4)
		settings->numAnalogChannels = 4;
	else
		settings->numAnalogChannels = 2;

	// Initialise the rendering environment: sample rates, frame counts, numbers of channels
	gContext.audioSampleRate = 44100.0;
	gContext.audioChannels = 2;

	if(settings->useAnalog) {
		gContext.audioFrames = settings->periodSize;

		gContext.analogFrames = gContext.audioFrames * 4 / settings->numAnalogChannels;
		gContext.analogChannels = settings->numAnalogChannels;
		gContext.analogSampleRate = gContext.audioSampleRate * 4.0 / (float)settings->numAnalogChannels;
	}
	else {
		gContext.audioFrames = settings->periodSize;

		gContext.analogFrames = 0;
		gContext.analogChannels = 0;
		gContext.analogSampleRate = 0;
	}

	// Sanity check the combination of channels and period size
	if( gContext.analogChannels != 0 && ((gContext.analogChannels <= 4 && gContext.analogFrames < 2) ||
			(gContext.analogChannels <= 2 && gContext.analogFrames < 4)) )
	{
		cout << "Error: " << gContext.analogChannels << " channels and period size of " << gContext.analogFrames << " not supported.\n";
		return 1;
	}

	// For now, digital frame rate is equal to audio frame rate
	if(settings->useDigital) {
		gContext.digitalFrames = gContext.audioFrames;
		gContext.digitalSampleRate = gContext.audioSampleRate;
		gContext.digitalChannels = settings->numDigitalChannels;
	}
	else {
		gContext.digitalFrames = 0;
		gContext.digitalSampleRate = 0;
		gContext.digitalChannels = 0;
	}

	// Set flags based on init settings
	if(settings->interleave)
		gContext.flags |= BEAGLERT_FLAG_INTERLEAVED;
	if(settings->analogOutputsPersist)
		gContext.flags |= BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST;

	// Use PRU for audio
	gPRU = new PRU(&gContext);
	gAudioCodec = new I2c_Codec();

	// Initialise the GPIO pins, including possibly the digital pins in the render routines
	if(gPRU->prepareGPIO(1, 1)) {
		cout << "Error: unable to prepare GPIO for PRU audio\n";
		return 1;
	}

	// Get the PRU memory buffers ready to go
	if(gPRU->initialise(0, gContext.analogFrames, gContext.analogChannels, true)) {
		cout << "Error: unable to initialise PRU\n";
		return 1;
	}

	// Prepare the audio codec, which clocks the whole system
	if(gAudioCodec->initI2C_RW(2, settings->codecI2CAddress, -1)) {
		cout << "Unable to open codec I2C\n";
		return 1;
	}
	if(gAudioCodec->initCodec()) {
		cout << "Error: unable to initialise audio codec\n";
		return 1;
	}

	// Set default volume levels
	BeagleRT_setDACLevel(settings->dacLevel);
	BeagleRT_setADCLevel(settings->adcLevel);
	// TODO: add more argument checks
	for(int n = 0; n < 2; n++){
		if(settings->pgaGain[n] > 59.5){
			std::cerr << "PGA gain out of range [0,59.5]\n";
			exit(1);
		}
		BeagleRT_setPgaGain(settings->pgaGain[n], n);
	}
	BeagleRT_setHeadphoneLevel(settings->headphoneLevel);

	// Call the user-defined initialisation function
	if(!setup(&gContext, userData)) {
		cout << "Couldn't initialise audio rendering\n";
		return 1;
	}

	return 0;
}

// audioLoop() is the main function which starts the PRU audio code
// and then transfers control to the PRU object. The PRU object in
// turn will call the audio render() callback function every time
// there is new data to process.

void audioLoop(void *)
{
	if(gRTAudioVerbose==1)
		rt_printf("_________________Audio Thread!\n");

	// PRU audio
	assert(gAudioCodec != 0 && gPRU != 0);

	if(gAudioCodec->startAudio(0)) {
		rt_printf("Error: unable to start I2C audio codec\n");
		gShouldStop = 1;
	}
	else {
		if(gPRU->start(gPRUFilename)) {
			rt_printf("Error: unable to start PRU from file %s\n", gPRUFilename);
			gShouldStop = 1;
		}
		else {
			// All systems go. Run the loop; it will end when gShouldStop is set to 1

			if(!gAmplifierShouldBeginMuted) {
				// First unmute the amplifier
				if(BeagleRT_muteSpeakers(0)) {
					if(gRTAudioVerbose)
						rt_printf("Warning: couldn't set value (high) on amplifier mute pin\n");
				}
			}

#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
			gPRU->loop(&gRTAudioInterrupt, gUserData);
#else
			gPRU->loop(0, gUserData);
#endif
			// Now clean up
			// gPRU->waitForFinish();
			gPRU->disable();
			gAudioCodec->stopAudio();
			gPRU->cleanupGPIO();
		}
	}

	if(gRTAudioVerbose == 1)
		rt_printf("audio thread ended\n");
}

// Create a calculation loop which can run independently of the audio, at a different
// (equal or lower) priority. Audio priority is defined in BEAGLERT_AUDIO_PRIORITY;
// priority should be generally be less than this.
// Returns an (opaque) pointer to the created task on success; 0 on failure
AuxiliaryTask BeagleRT_createAuxiliaryTask(void (*functionToCall)(void), int priority, const char *name)
{
	InternalAuxiliaryTask *newTask = (InternalAuxiliaryTask*)malloc(sizeof(InternalAuxiliaryTask));

	// Attempt to create the task
	if(rt_task_create(&(newTask->task), name, 0, priority, T_JOINABLE | T_FPU)) {
		  cout << "Error: unable to create auxiliary task " << name << endl;
		  free(newTask);
		  return 0;
	}

	// Populate the rest of the data structure and store it in the vector
	newTask->function = functionToCall;
	newTask->name = strdup(name);
	newTask->priority = priority;
	newTask->started = false;

	getAuxTasks().push_back(newTask);

	return (AuxiliaryTask)newTask;
}

// Schedule a previously created (and started) auxiliary task. It will run when the priority rules next
// allow it to be scheduled.
void BeagleRT_scheduleAuxiliaryTask(AuxiliaryTask task)
{
	InternalAuxiliaryTask *taskToSchedule = (InternalAuxiliaryTask *)task;
	if(taskToSchedule->started == false){ // Note: this is not the safest method to check if a task
		BeagleRT_startAuxiliaryTask(task); // is started (or ready to be resumed), but it probably is the fastest.
                                           // A safer approach would use rt_task_inquire()
	}
	rt_task_resume(&taskToSchedule->task);
}

// Calculation loop that can be used for other tasks running at a lower
// priority than the audio thread. Simple wrapper for Xenomai calls.
// Treat the argument as containing the task structure
void auxiliaryTaskLoop(void *taskStruct)
{
	// Get function to call from the argument
	void (*auxiliary_function)(void) = ((InternalAuxiliaryTask *)taskStruct)->function;
	const char *name = ((InternalAuxiliaryTask *)taskStruct)->name;

	// Wait for a notification
	rt_task_suspend(NULL);

	while(!gShouldStop) {
		// Then run the calculations
		auxiliary_function();

		// Wait for a notification
		rt_task_suspend(NULL);
	}

	if(gRTAudioVerbose == 1)
		rt_printf("auxiliary task %s ended\n", name);
}


int BeagleRT_startAuxiliaryTask(AuxiliaryTask task){
	InternalAuxiliaryTask *taskStruct;
	taskStruct = (InternalAuxiliaryTask *)task;
	if(taskStruct->started == true)
		return 0;
	if(rt_task_start(&(taskStruct->task), &auxiliaryTaskLoop, taskStruct)) {
		cerr << "Error: unable to start Xenomai task " << taskStruct->name << endl;
		return -1;
	}
	taskStruct->started = true;
	return 0;
}

// startAudio() should be called only after initAudio() successfully completes.
// It launches the real-time Xenomai task which runs the audio loop. Returns 0
// on success.

int BeagleRT_startAudio()
{
	// Create audio thread with high Xenomai priority
	if(rt_task_create(&gRTAudioThread, gRTAudioThreadName, 0, BEAGLERT_AUDIO_PRIORITY, T_JOINABLE | T_FPU)) {
		  cout << "Error: unable to create Xenomai audio thread" << endl;
		  return -1;
	}

#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
	// Create an interrupt which the audio thread receives from the PRU
	int result = 0;
	if((result = rt_intr_create(&gRTAudioInterrupt, gRTAudioInterruptName, PRU_RTAUDIO_IRQ, I_NOAUTOENA)) != 0) {
		cout << "Error: unable to create Xenomai interrupt for PRU (error " << result << ")" << endl;
		return -1;
	}
#endif

	// Start all RT threads
	if(rt_task_start(&gRTAudioThread, &audioLoop, 0)) {
		  cout << "Error: unable to start Xenomai audio thread" << endl;
		  return -1;
	}

	// The user may have created other tasks. Start those also.
	vector<InternalAuxiliaryTask*>::iterator it;
	for(it = getAuxTasks().begin(); it != getAuxTasks().end(); it++) {
		int ret = BeagleRT_startAuxiliaryTask(*it);
		if(ret != 0)
			return -2;
	}
	return 0;
}

// Stop the PRU-based audio from running and wait
// for the tasks to complete before returning.

void BeagleRT_stopAudio()
{
	// Tell audio thread to stop (if this hasn't been done already)
	gShouldStop = true;

	if(gRTAudioVerbose)
		cout << "Stopping audio...\n";

	// Now wait for threads to respond and actually stop...
	rt_task_join(&gRTAudioThread);

	// Stop all the auxiliary threads too
	vector<InternalAuxiliaryTask*>::iterator it;
	for(it = getAuxTasks().begin(); it != getAuxTasks().end(); it++) {
		InternalAuxiliaryTask *taskStruct = *it;

		// Wake up each thread and join it
		rt_task_resume(&(taskStruct->task));
		rt_task_join(&(taskStruct->task));
	}
}

// Free any resources associated with PRU real-time audio
void BeagleRT_cleanupAudio()
{
	cleanup(&gContext, gUserData);

	// Clean up the auxiliary tasks
	vector<InternalAuxiliaryTask*>::iterator it;
	for(it = getAuxTasks().begin(); it != getAuxTasks().end(); it++) {
		InternalAuxiliaryTask *taskStruct = *it;

		// Delete the task
		rt_task_delete(&taskStruct->task);

		// Free the name string and the struct itself
		free(taskStruct->name);
		free(taskStruct);
	}
	getAuxTasks().clear();

	// Delete the audio task and its interrupt
#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
	rt_intr_delete(&gRTAudioInterrupt);
#endif
	rt_task_delete(&gRTAudioThread);

	if(gPRU != 0)
		delete gPRU;
	if(gAudioCodec != 0)
		delete gAudioCodec;

	if(gAmplifierMutePin >= 0)
		gpio_unexport(gAmplifierMutePin);
	gAmplifierMutePin = -1;
}

// Set the level of the DAC; affects all outputs (headphone, line, speaker)
// 0dB is the maximum, -63.5dB is the minimum; 0.5dB steps
int BeagleRT_setDACLevel(float decibels)
{
	if(gAudioCodec == 0)
		return -1;
	return gAudioCodec->setDACVolume((int)floorf(decibels * 2.0 + 0.5));
}

// Set the level of the ADC
// 0dB is the maximum, -12dB is the minimum; 1.5dB steps
int BeagleRT_setADCLevel(float decibels)
{
	if(gAudioCodec == 0)
		return -1;
	return gAudioCodec->setADCVolume((int)floorf(decibels * 2.0 + 0.5));
}

// Set the level of the Programmable Gain Amplifier
// 59.5dB is maximum, 0dB is minimum; 0.5dB steps
int BeagleRT_setPgaGain(float decibels, int channel){
	if(gAudioCodec == 0)
		return -1;
	return gAudioCodec->setPga(decibels, channel);
}

// Set the level of the onboard headphone amplifier; affects headphone
// output only (not line out or speaker)
// 0dB is the maximum, -63.5dB is the minimum; 0.5dB steps
int BeagleRT_setHeadphoneLevel(float decibels)
{
	if(gAudioCodec == 0)
		return -1;
	return gAudioCodec->setHPVolume((int)floorf(decibels * 2.0 + 0.5));
}

// Mute or unmute the onboard speaker amplifiers
// mute == 0 means unmute; otherwise mute
// Returns 0 on success
int BeagleRT_muteSpeakers(int mute)
{
	int pinValue = mute ? LOW : HIGH;

	// Check that we have an enabled pin for controlling the mute
	if(gAmplifierMutePin < 0)
		return -1;

	return gpio_set_value(gAmplifierMutePin, pinValue);
}

// Set the verbosity level
void BeagleRT_setVerboseLevel(int level)
{
	gRTAudioVerbose = level;
}
