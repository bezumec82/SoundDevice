/**
 *  @file
 *  @brief Wiring-inspired utility functions and macros
 *
 *  Macros and functions for I/O and data processing taking after the Wiring
 *  (Arduino) language. This code began as part of the Hackable Instruments
 *  project (EPSRC) at Queen Mary University of London, 2013-14.
 *
 *  (c) 2014-15 Andrew McPherson, Victor Zappi and Giulio Moro,
 *  Queen Mary University of London
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "BeagleRT.h"

/**
 * \defgroup iofunctions I/O functions and constants
 *
 * These functions and macros are used for audio, analog and digital I/O. All the
 * I/O functions require the BeagleRTContext data structure from render() to be passed
 * in. This means that these functions are, by design, \b only usable from within
 * the rendering thread.
 * 
 * The naming conventions are loosely derived from the Arduino environment, and the
 * syntax is similar. Unlike Arduino, the I/O functions require the frame number at which
 * the read or write should take place, since all I/O happens synchronously with the
 * audio clock.
 *
 * @{
 */

#ifndef INPUT
	#define INPUT 0x0
#endif /* INPUT */

#ifndef OUTPUT
	#define OUTPUT 0x1
#endif /* OUTPUT */

/** @} */

/**
 * \ingroup wiring
 *
 * @{
 */

/// Set the given bit in \c word to 1.
#define setBit(word,bit) 			((word) | (1 << (bit)))

/// Clear the given bit in \c word to 0.
#define clearBit(word,bit) 			((word) &~ (1 << (bit)))

/// Check if the given bit in \c word is 1 (returns nonzero) or 0 (returns zero).
#define getBit(word,bit) 			(((word) >> (bit)) & 1)

/// Set/clear the given bit in \c word to \c value.
#define changeBit(word,bit,value) 	((clearBit((word),(bit))) | ((value) << (bit)))

/** @} */

/**
 * \ingroup iofunctions
 *
 * @{
 */

#if 1
// Note: pinMode(), analogWrite() and digitalWrite() should be able to be called from setup()
// Likewise, thread launch should be able to be called from setup()
// Also, make volume change functions callable from render() thread -- as an aux task?

/**
 * \brief Read an audio input, specifying the frame number (when to read) and the channel.
 *
 * This function returns the value of an audio input, at the time indicated by \c frame.
 * The returned value ranges from -1 to 1.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to read the audio input. Valid values range
 * from 0 to (context->audioFrames - 1).
 * \param channel Which audio input to read. Valid values are between 0 and
 * (context->audioChannels - 1), typically 0 to 1 by default.
 * \return Value of the analog input, range  to 1.
 */
static inline float audioReadFrame(BeagleRTContext *context, int frame, int channel);

/**
 * \brief Write an audio output, specifying the frame number (when to write) and the channel.
 *
 * This function sets the value of an audio output, at the time indicated by \c frame. Valid
 * values are between -1 and 1.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to write the audio output. Valid values range
 * from 0 to (context->audioFrames - 1).
 * \param channel Which analog output to write. Valid values are between 0 and
 * (context->audioChannels - 1), typically 0 to 1 by default.
 * \param value Value to write to the output, range -1 to 1.
 */
static inline void audioWriteFrame(BeagleRTContext *context, int frame, int channel, float value);

/**
 * \brief Read an analog input, specifying the frame number (when to read) and the channel.
 *
 * This function returns the value of an analog input, at the time indicated by \c frame.
 * The returned value ranges from 0 to 1, corresponding to a voltage range of 0 to 4.096V.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to read the analog input. Valid values range
 * from 0 to (context->analogFrames - 1).
 * \param channel Which analog input to read. Valid values are between 0 and
 * (context->analogChannels - 1), typically 0 to 7 by default.
 * \return Value of the analog input, range 0 to 1.
 */
static inline float analogReadFrame(BeagleRTContext *context, int frame, int channel);

/**
 * \brief Write an analog output, specifying the frame number (when to write) and the channel.
 *
 * This function sets the value of an analog output, at the time indicated by \c frame. Valid
 * values are between 0 and 1, corresponding to the range 0 to 5V.
 *
 * The value written will persist for all future frames if BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST
 * is set in context->flags. This is the default behaviour.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to write the analog output. Valid values range
 * from 0 to (context->analogFrames - 1).
 * \param channel Which analog output to write. Valid values are between 0 and
 * (context->analogChannels - 1), typically 0 to 7 by default.
 * \param value Value to write to the output, range 0 to 1.
 */
static inline void analogWriteFrame(BeagleRTContext *context, int frame, int channel, float value);

/**
 * \brief Write an analog output, specifying the frame number (when to write) and the channel.
 *
 * This function sets the value of an analog output, at the time indicated by \c frame. Valid
 * values are between 0 and 1, corresponding to the range 0 to 5V.
 *
 * Unlike analogWriteFrame(), the value written will affect \b only the frame specified, with
 * future values unchanged. This is faster than analogWriteFrame() so is better suited
 * to applications where every frame will be written to a different value. If
 * BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST is not set within context->flags, then
 * analogWriteFrameOnce() and analogWriteFrame() are equivalent.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to write the analog output. Valid values range
 * from 0 to (context->analogFrames - 1).
 * \param channel Which analog output to write. Valid values are between 0 and
 * (context->analogChannels - 1), typically 0 to 7 by default.
 * \param value Value to write to the output, range 0 to 1.
 */
static inline void analogWriteFrameOnce(BeagleRTContext *context, int frame, int channel, float value);

/**
 * \brief Read a digital input, specifying the frame number (when to read) and the pin.
 *
 * This function returns the value of a digital input, at the time indicated by \c frame.
 * The value is 0 if the pin is low, and nonzero if the pin is high (3.3V).
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to read the digital input. Valid values range
 * from 0 to (context->digitalFrames - 1).
 * \param channel Which digital pin to read. 16 pins across the P8 and P9 headers of the
 * BeagleBone Black are available. See the constants P8_xx and P9_xx defined in
 * digital_gpio_mapping.h.
 * \return Value of the digital input.
 */
static inline int digitalReadFrame(BeagleRTContext *context, int frame, int channel);

/**
 * \brief Write a digital output, specifying the frame number (when to write) and the pin.
 *
 * This function sets the value of a digital output, at the time indicated by \c frame.
 * A value of 0 sets the pin low; any other value sets the pin high (3.3V).
 *
 * The value written will persist for all future frames.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to write the digital output. Valid values range
 * from 0 to (context->digitalFrames - 1).
 * \param channel Which digital output to write. 16 pins across the P8 and P9 headers of the
 * BeagleBone Black are available. See the constants P8_xx and P9_xx defined in
 * digital_gpio_mapping.h.
 * \param value Value to write to the output.
 */
static inline void digitalWriteFrame(BeagleRTContext *context, int frame, int channel, int value);

/**
 * \brief Write a digital output, specifying the frame number (when to write) and the pin.
 *
 * This function sets the value of a digital output, at the time indicated by \c frame.
 * A value of 0 sets the pin low; any other value sets the pin high (3.3V).
 *
 * Unlike digitalWriteFrame(), the value written will affect \b only the frame specified, with
 * future values unchanged. This is faster than digitalWriteFrame() so is better suited
 * to applications where every frame will be written to a different value.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to write the digital output. Valid values range
 * from 0 to (context->digitalFrames - 1).
 * \param channel Which digital output to write. 16 pins across the P8 and P9 headers of the
 * BeagleBone Black are available. See the constants P8_xx and P9_xx defined in
 * digital_gpio_mapping.h.
 * \param value Value to write to the output.
 */
static inline void digitalWriteFrameOnce(BeagleRTContext *context, int frame, int channel, int value);

/**
 * \brief Set the direction of a digital pin to input or output.
 *
 * This function sets the direction of a digital pin, at the time indicated by \c frame.
 * Valid values are \c INPUT and \c OUTPUT. All pins begin as inputs by default.
 *
 * The value written will persist for all future frames.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to set the pin direction. Valid values range
 * from 0 to (context->digitalFrames - 1).
 * \param channel Which digital output to write. 16 pins across the P8 and P9 headers of the
 * BeagleBone Black are available. See the constants P8_xx and P9_xx defined in
 * digital_gpio_mapping.h.
 * \param value Direction of the pin (\c INPUT or \c OUTPUT).
 */
static inline void pinModeFrame(BeagleRTContext *context, int frame, int channel, int mode);

/**
 * \brief Set the direction of a digital pin to input or output.
 *
 * This function sets the direction of a digital pin, at the time indicated by \c frame.
 * Valid values are \c INPUT and \c OUTPUT. All pins begin as inputs by default.
 *
 * The value written will affect only the specified frame.
 *
 * \param context The I/O data structure which is passed by BeagleRT to render().
 * \param frame Which frame (i.e. what time) to set the pin direction. Valid values range
 * from 0 to (context->digitalFrames - 1).
 * \param channel Which digital output to write. 16 pins across the P8 and P9 headers of the
 * BeagleBone Black are available. See the constants P8_xx and P9_xx defined in
 * digital_gpio_mapping.h.
 * \param value Direction of the pin (\c INPUT or \c OUTPUT).
 */
static inline void pinModeFrameOnce(BeagleRTContext *context, int frame, int channel, int mode);

/** @} */

#else

// Macros for accessing the analog values: usable _only_ within render()

// Read an Analog input from input pin p at frame f
#define analogRead(p, f) (analogIn[(f)*gNumAnalogChannels + (p)])
// Write an Analog output frame at output pin p, frame f, to value v
#define analogWriteFrame(p, f, v) (analogOut[(f)*gNumAnalogChannels + (p)] = (v))
#define analogWrite(pin, frame, value) \
(({do {\
	for (int _privateI=(frame); _privateI<numAnalogFrames; _privateI++){ \
		analogWriteFrame(pin,_privateI,value); \
	}\
	} while (0);}),(void)0)\


//digital API:
#define setDigitalDirectionFrame(pin,frame,direction) digital[(frame)]=changeBit(digital[(frame)],(pin),(direction)),void(0)
#define setDigitalDirection(pin,frame,direction)\
		(({do {\
			for(int _privateI=(frame); _privateI<numDigitalFrames; _privateI++)\
				setDigitalDirectionFrame(pin,_privateI,direction);\
			} while (0);}), (void)0)
#define digitalWriteAll(frame,value) digital[(frame)]=0xffff0000*(!(!value));
//sets the bit in the high word, clears the bit in the low word (just in case the direction was not previously set)
#define digitalWriteFrame(pin, frame, value) digital[(frame)]=( changeBit(digital[(frame)], (pin+16), (value)) & (0xffffffff-(1<<(pin))) ) //could have been done with two subsequent assignments
#define digitalWrite(pin, frame, value) \
	(({do {\
		for (int _privateI=(frame); _privateI<numDigitalFrames; _privateI++) \
			digitalWriteFrame(pin,_privateI,value); \
		} while (0);}),(void)0)\

#define digitalRead(pin, frame) ( getBit(digital[(frame)], pin+16) )

#endif

/**
 * \defgroup wiring Wiring language support
 *
 * These are functions found in the Wiring (Arduino) language which are not directly
 * related to I/O but are provided as a convenience.
 *
 * @{
 */

/**
 * \brief Linearly rescale a number from one range of values to another.
 *
 * This function linearly scales values of \c x such that the range in_min to
 * in_max at the input corresponds to the range out_min to out_max
 * at the output. Values outside this range are extrapolated.
 *
 * This function behaves identically to the function of the same name in Processing. It
 * is also similar to the corresponding function in Arduino, except that it supports floating
 * point values.
 *
 * \param x Input value to be mapped.
 * \param in_min Lower bound of the input range.
 * \param in_max Upper bound of the input range.
 * \param out_min Lower bound of the output range.
 * \param out_max Upper bound of the output range.
 * \return Rescaled value.
 */
static inline float map(float x, float in_min, float in_max, float out_min, float out_max);

/**
 * \brief Constrain a number to stay within a given range.
 *
 * This function constrains \c x to remain within the range min_val to
 * max_val. Values of \c x outside this range are clipped to the edges
 * of the range.
 *
 * This function behaves identically to the function of the same name in Processing. It
 * is also similar to the corresponding function in Arduino, except that it supports floating
 * point values.
 *
 * \param x Input value to be constrained.
 * \param min_val Minimum possible value.
 * \param max_val Maximum possible value.
 * \return Constrained value.
 */
static inline float constrain(float x, float min_val, float max_val);

/** @} */
// audioReadFrame()
//
// Returns the value of the given audio input at the given frame number.
static inline float audioReadFrame(BeagleRTContext *context, int frame, int channel) {
	return context->audioIn[frame * context->audioChannels + channel];
}

// audioWriteFrame()
//
// Sets a given audio output channel to a value for the current frame
static inline void audioWriteFrame(BeagleRTContext *context, int frame, int channel, float value) {
	context->audioOut[frame * context->audioChannels + channel] = value;
}

// analogReadFrame()
//
// Returns the value of the given analog input at the given frame number.
static inline float analogReadFrame(BeagleRTContext *context, int frame, int channel) {
	return context->analogIn[frame * context->analogChannels + channel];
}

// analogWriteFrame()
//
// Sets a given analog output channel to a value for the current frame and, if persistent outputs are
// enabled, for all subsequent frames
static inline void analogWriteFrame(BeagleRTContext *context, int frame, int channel, float value) {
	if(context->flags & BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST) {
		for(unsigned int f = frame; f < context->analogFrames; f++)
			context->analogOut[frame * context->analogChannels + channel] = value;
	}
	else
		context->analogOut[frame * context->analogChannels + channel] = value;
}

// analogWriteFrameOnce()
//
// Sets a given channel to a value for only the current frame
static inline void analogWriteFrameOnce(BeagleRTContext *context, int frame, int channel, float value) {
	context->analogOut[frame * context->analogChannels + channel] = value;
}

// digitalReadFrame()
//
// Returns the value of a given digital input at the given frame number
static inline int digitalReadFrame(BeagleRTContext *context, int frame, int channel) {
	return getBit(context->digital[frame], channel + 16);
}

// digitalWriteFrame()
//
// Sets a given digital output channel to a value for the current frame and all subsequent frames
static inline void digitalWriteFrame(BeagleRTContext *context, int frame, int channel, int value) {
	for(unsigned int f = frame; f < context->digitalFrames; f++) {
		if(value)
			context->digital[f] |= 1 << (channel + 16);
		else
			context->digital[f] &= ~(1 << (channel + 16));
	}
}

// digitalWriteFrameOnce()
//
// Sets a given digital output channel to a value for the current frame only
static inline void digitalWriteFrameOnce(BeagleRTContext *context, int frame, int channel, int value) {
	if(value)
		context->digital[frame] |= 1 << (channel + 16);
	else
		context->digital[frame] &= ~(1 << (channel + 16));
}

// pinModeFrame()
//
// Sets the direction of a digital pin for the current frame and all subsequent frames
static inline void pinModeFrame(BeagleRTContext *context, int frame, int channel, int mode) {
	for(unsigned int f = frame; f < context->digitalFrames; f++) {
		if(mode == INPUT)
			context->digital[f] |= (1 << channel);
		else
			context->digital[f] &= ~(1 << channel);
	}
}

// pinModeFrameOnce()
//
// Sets the direction of a digital pin for the current frame only
static inline void pinModeFrameOnce(BeagleRTContext *context, int frame, int channel, int mode) {
	if(mode == INPUT)
		context->digital[frame] |= (1 << channel);
	else
		context->digital[frame] &= ~(1 << channel);
}



// map()
//
// Scale an input value from one range to another. Works like its Wiring language equivalent.
// x is the value to scale; in_min and in_max are the input range; out_min and out_max
// are the output range.

static inline float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// constrain()
//
// Clips an input value to be between two end points
// x is the value to constrain; min_val and max_val are the range

static inline float constrain(float x, float min_val, float max_val)
{
	if(x < min_val) return min_val;
	if(x > max_val) return max_val;
	return x;
}

#endif /* UTILITIES_H_ */
