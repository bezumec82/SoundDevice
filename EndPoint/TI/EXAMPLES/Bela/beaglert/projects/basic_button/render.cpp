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
\example 2_digital_button

Switching an LED on and off
---------------------------

This sketch brings together digital out with digital in. The program will read 
a button and turn the LED on and off according to the state of the button.

- connect an LED in series with a 470ohm resistor between P8_07 and ground.
- connect a 1k resistor to P9_03 (+3.3V),
- connect the other end of the resistor to both a button and P8_08
- connect the other end of the button to ground.

You will notice that the LED will normally stay on and will turn off as long as 
the button is pressed. This is due to the fact that the LED is set to the same 
value read at input P8_08. When the button is not pressed, P8_08 is `HIGH` and so 
P8_07 is set to `HIGH` as well, so that the LED conducts and emits light. When 
the button is pressed, P8_08 goes `LOW` and P8_07 is set to `LOW`, turning off the LED.

As an exercise try and change the code so that the LED only turns on when 
the button is pressed
*/


#include <BeagleRT.h>
#include <Utilities.h>
#include <cmath>
#include <rtdk.h>
#include <stdlib.h>

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
	pinModeFrame(context, 0, P8_08, INPUT);
	pinModeFrame(context, 0, P8_07, OUTPUT);
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numAnalogFrames
// will be 0.

/* basic_button
* - connect an LED in series with a 470ohm resistor between P8_07 and ground.
* - connect a 1k resistor to P9_03 (+3.3V), 
* - connect the other end of the resistor to both a button and P8_08
* - connect the other end of the button to ground.
* The program will read the button and make the LED blink when the button is pressed.
*/

void render(BeagleRTContext *context, void *userData)
{
	for(unsigned int n=0; n<context->digitalFrames; n++){
		int status=digitalReadFrame(context, 0, P8_08); //read the value of the button
		digitalWriteFrameOnce(context, n, P8_07, status); //write the status to the LED
		float out = 0.1 * status * rand() / (float)RAND_MAX * 2 - 1; //generate some noise, gated by the button
		for(unsigned int j = 0; j < context->audioChannels; j++){
			audioWriteFrame(context, n, j, out); //write the audio output
		}
	}
}

// cleanup() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in setup().

void cleanup(BeagleRTContext *context, void *userData)
{
	// Nothing to do here
}

