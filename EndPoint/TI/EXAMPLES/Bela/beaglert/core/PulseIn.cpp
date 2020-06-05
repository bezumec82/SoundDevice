/*
* PulseIn.cpp
*
*  Created on: 4 Feb 2016
*      Author: giulio
*/

#include "../include/PulseIn.h"

void PulseIn::init(BeagleRTContext* context, unsigned int digitalInput, int direction){
	_digitalInput = digitalInput;
	_pulseIsOn = false;
	_pulseOnState = direction == 1 ? 1 : 0;
	_array.resize(context->digitalFrames);
	_lastContext = (uint64_t)-1;
	pinModeFrame(context, 0, digitalInput, INPUT); //context is used to allocate the number of elements in the array
}

void PulseIn::check(BeagleRTContext* context){
	if(_digitalInput == -1){ //must be init'ed before calling check();
		throw(1);
	}
	for(unsigned int n = 0; n < context->digitalFrames; n++){
		_array[n] = 0; //maybe a few of these will be overwritten below
	}
	for(unsigned int n = 0; n < context->digitalFrames; n++){
		if(_pulseIsOn == false){ // look for start edge
			if(digitalReadFrame(context, n, _digitalInput) == _pulseOnState){
				_pulseStart = context->audioSampleCount + n; // store location of start edge
				_pulseIsOn = true;
			}
		} else { // _pulseIsOn == true;
			if(digitalReadFrame(context, n, _digitalInput) == !_pulseOnState){ // look for stop edge
				_array[n] = context->audioSampleCount + n - _pulseStart; // compute and store pulse duration
				_pulseIsOn = false;
			}
		}
	}
	_lastContext = context->audioSampleCount;
};

PulseIn::~PulseIn() {
// TODO Auto-generated destructor stub
}

