/*
 * render.cpp
 *
 *  Template render.cpp file for on-board heavy compiling
 *
 *  N.B. this is currently *not* compatible with foleyDesigner source files!
 *
 *  Created on: November 5, 2015
 *
 *  Christian Heinrichs
 *
 */

#include <BeagleRT.h>
#include <Midi.h>
#include <cmath>
#include "../include/Utilities.h"
#include "Heavy_bbb.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>
/*
 *	HEAVY CONTEXT & BUFFERS
 */

Hv_bbb *gHeavyContext;
float *gHvInputBuffers = NULL, *gHvOutputBuffers = NULL;
int gHvInputChannels = 0, gHvOutputChannels = 0;

float gInverseSampleRate;

/*
 *	HEAVY FUNCTIONS
 */

void printHook(double timestampSecs, const char *printLabel, const char *msgString, void *userData) {
  printf("Message from Heavy patch: [@ %.3f] %s: %s\n", timestampSecs, printLabel, msgString);
}

static void sendHook(
    double timestamp, // in milliseconds
    const char *receiverName,
    const HvMessage *const m,
    void *userData) {

  // only react to messages sent to receivers named "hello"
  if (!strncmp(receiverName, "hello", 5)) {
  }

}

/*
 * SETUP, RENDER LOOP & CLEANUP
 */

Midi midi;
bool setup(BeagleRTContext *context, void *userData)	{

	/* HEAVY */

	gHeavyContext = hv_bbb_new(context->audioSampleRate);

	gHvInputChannels = hv_getNumInputChannels(gHeavyContext);
	gHvOutputChannels = hv_getNumOutputChannels(gHeavyContext);

	rt_printf("Starting Heavy context with %d input channels and %d output channels\n",
			  gHvInputChannels, gHvOutputChannels);

	if(gHvInputChannels != 0) {
		gHvInputBuffers = (float *)calloc(gHvInputChannels * context->audioFrames,sizeof(float));
	}
	if(gHvOutputChannels != 0) {
		gHvOutputBuffers = (float *)calloc(gHvOutputChannels * context->audioFrames,sizeof(float));
	}

	gInverseSampleRate = 1.0 / context->audioSampleRate;

	// Set heavy print hook
	hv_setPrintHook(gHeavyContext, &printHook);
	// Set heavy send hook
	hv_setSendHook(gHeavyContext, sendHook);

	midi.readFrom(0);
	midi.writeTo(0);
	midi.enableParser(true);
	return true;
}


void render(BeagleRTContext *context, void *userData)
{

	// De-interleave the data
	if(gHvInputBuffers != NULL) {
		for(int n = 0; n < context->audioFrames; n++) {
			for(int ch = 0; ch < gHvInputChannels; ch++) {
				if(ch >= context->audioChannels+context->analogChannels) {
					// THESE ARE PARAMETER INPUT 'CHANNELS' USED FOR ROUTING
					// 'sensor' outputs from routing channels of dac~ are passed through here
					break;
				} else {
					// If more than 2 ADC inputs are used in the pd patch, route the analog inputs
					// i.e. ADC3->analogIn0 etc. (first two are always audio inputs)
					if(ch >= context->audioChannels)	{
						int m = n/2;
						float mIn = context->analogIn[m*context->analogChannels + (ch-context->audioChannels)];
						gHvInputBuffers[ch * context->audioFrames + n] = mIn;
					} else {
						gHvInputBuffers[ch * context->audioFrames + n] = context->audioIn[n * context->audioChannels + ch];
					}
				}
			}
		}
	}

	// replacement for bang~ object
	//hv_vscheduleMessageForReceiver(gHeavyContext, "bbb_bang", 0.0f, "b");
	{
		int num;
		unsigned int hvHashes[3];
		hvHashes[0] = hv_stringToHash("bela_notein");
		hvHashes[1] = hv_stringToHash("bela_ctlin");
		hvHashes[2] = hv_stringToHash("bela_pgmin"); 
		while((num = midi.getParser()->numAvailableMessages()) > 0){
			static MidiChannelMessage message;
			message = midi.getParser()->getNextChannelMessage();
			switch(message.getType()){
			case kmmNoteOn: {
//					message.prettyPrint();
					float noteNumber = message.getDataByte(0);
					float velocity = message.getDataByte(1);
					float channel = message.getChannel();
//					rt_printf("message: noteNumber: %f, velocity: %f, channel: %f\n", noteNumber, velocity, channel);
					hv_vscheduleMessageForReceiver(gHeavyContext, hvHashes[0], 0, "fff", noteNumber, velocity, channel);
				}
				break;
			case kmmControlChange: {
				hv_vscheduleMessageForReceiver(gHeavyContext, hvHashes[1], 0, "fff",
						(float)message.getDataByte(1), (float)message.getDataByte(0), (float)message.getChannel());
				}
				break;
			case kmmProgramChange:
				hv_vscheduleMessageForReceiver(gHeavyContext, hvHashes[2], 0, "ff",
						(float)message.getDataByte(0), (float)message.getChannel());
				break;
			}
		}
	}
//	hv_sendFloatToReceiver(gHeavyContext, "notein", 1.123f);


	hv_bbb_process_inline(gHeavyContext, gHvInputBuffers, gHvOutputBuffers, context->audioFrames);

	// Interleave the output data
	if(gHvOutputBuffers != NULL) {
		for(int n = 0; n < context->audioFrames; n++) {

			for(int ch = 0; ch < gHvOutputChannels; ch++) {
				if(ch >= context->audioChannels+context->analogChannels) {
					// THESE ARE SENSOR OUTPUT 'CHANNELS' USED FOR ROUTING
					// they are the content of the 'sensor output' dac~ channels
				} else {
					if(ch >= context->audioChannels)	{
						int m = n/2;
						context->analogOut[m * context->analogFrames + (ch-context->audioChannels)] = constrain(gHvOutputBuffers[ch*context->audioFrames + n],0.0,1.0);
					} else {
						context->audioOut[n * context->audioChannels + ch] = gHvOutputBuffers[ch * context->audioFrames + n];
					}
				}
			}
		}
	}

}


void cleanup(BeagleRTContext *context, void *userData)
{

	hv_bbb_free(gHeavyContext);
	if(gHvInputBuffers != NULL)
		free(gHvInputBuffers);
	if(gHvOutputBuffers != NULL)
		free(gHvOutputBuffers);

}
