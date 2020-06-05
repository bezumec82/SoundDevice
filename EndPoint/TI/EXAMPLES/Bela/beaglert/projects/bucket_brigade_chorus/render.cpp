#include <BeagleRT.h> 
#include <Scope.h>
#include <cmath>
#include <Utilities.h>

float gPhase1, gPhase2;
float gFrequency1, gFrequency2;
float gInverseSampleRate;

// initialise_render() is called once before the audio rendering starts.
// Use it to perform any initialisation and allocation which is dependent
// on the period size or sample rate.
//
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
//
// Return true on success; returning false halts the program.
#include <I2c_Codec.h>
#include <PRU.h>
extern I2c_Codec *gAudioCodec;
extern PRU *gPRU;
float D=5264;
#define delayLength 256
float delay[delayLength];
int writePointer=0;
int readPointer=writePointer+1;
AuxiliaryTask updatePll;

void updatePllFunction(){
//	gPRU->setGPIOTestPin();
	static int count = 0;
	while(!gShouldStop){
		gAudioCodec->setPllD(D);
		count++;
		if((count&4095)==0)
			printf("sampling rate: %f\n",gAudioCodec->getAudioSamplingRate());
		usleep(100);
	}
//	gPRU->clearGPIOTestPin();
}

bool setup(BeagleRTContext *context, void *userData)
{
	gInverseSampleRate = 1.0/context->audioSampleRate;
	
	gPhase1 = 0.0;
	gPhase2 = 0.0;
	
	gFrequency1 = 200.0;
	gFrequency2 = 201.0;
	updatePll=BeagleRT_createAuxiliaryTask(&updatePllFunction, 91, "update PLL");
	for(int n=0; n<delayLength; n++){
		delay[n]=0;
	}
	return true; 
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BeagleRTContext *context, void *userData)
{
//	printf("here\n");
	static bool init = false;
	if(init == false){
		BeagleRT_scheduleAuxiliaryTask(updatePll);
//		gAudioCodec->setPllP(2);
//		gAudioCodec->setPllR();
//		gAudioCodec->setAudioSamplingRate(43600);
//		printf("samplingRate: %f, k: %f\n", gAudioCodec->getAudioSamplingRate(), gAudioCodec->getPllK());
		init = true;
	}
	static int count=0;
	static float lfoPhase=0;
	static float feedback=0;
	int updateRate=1;
	if((count&(updateRate-1))==0){
		float amplitude = 8000;
		float rate = 2;
		lfoPhase+=rate*2*M_PI*updateRate*context->analogFrames/context->audioSampleRate;
		D=amplitude+amplitude*sinf(lfoPhase);
		if((count&255)==0){
//			rt_printf("frequency: %f\n", gAudioCodec->getAudioSamplingRate());
//			rt_printf("D: %.0f\n", D);
//			rt_printf("rate: %f\n", rate);
//			rt_printf("amplitude: %.3f\n", amplitude);
//			rt_printf("feedback: %.3f\n\n", feedback);
		}
	}
	count++;

	for(unsigned int n = 0; n < context->audioFrames; n++) {
		feedback = 0.4;
		float input = audioReadFrame(context, n, 0) + audioReadFrame(context, n, 1);
	    delay[writePointer++] = input + delay[readPointer]*feedback;
	    float output = (input + 0.9*delay[readPointer++] ) * 0.5;
		audioWriteFrame(context, n, 0, output);
		audioWriteFrame(context, n, 1, output);
		if(writePointer>=delayLength)
			writePointer-=delayLength;
		if(readPointer>=delayLength)
			readPointer-=delayLength;

		gPhase1 += 2.0 * M_PI * gFrequency1 * gInverseSampleRate;
	    gPhase2 += 2.0 * M_PI * gFrequency2 * gInverseSampleRate;
		if(gPhase1 > 2.0 * M_PI)
			gPhase1 -= 2.0 * M_PI;
		if(gPhase2 > 2.0 * M_PI)
			gPhase2 -= 2.0 * M_PI;
	}
}

// cleanup_render() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in initialise_render().

void cleanup(BeagleRTContext *context, void *userData)
{
    
}
