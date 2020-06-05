/**
 *  @file
 *  @brief Main BeagleRT public API
 *
 *  Central control code for hard real-time audio on BeagleBone Black
 *  using PRU and Xenomai Linux extensions. This code began as part
 *  of the Hackable Instruments project (EPSRC) at Queen Mary University
 *  of London, 2013-14.
 *
 *  (c) 2014-15 Andrew McPherson, Victor Zappi and Giulio Moro,
 *  Queen Mary University of London
 */

/**
 * \mainpage
 *
 * BeagleRT is a hard-real-time, ultra-low latency audio and sensor environment for
 * BeagleBone Black, which works with the BeagleBone Audio Cape or a custom "BeagleRT Cape"
 * which incorporates stereo audio with 8x, 16-bit analog inputs and outputs.
 *
 * BeagleRT is based on the Xenomai real-time Linux extensions (http://xenomai.org) and
 * uses the BeagleBone %PRU subsystem to address the audio and sensor hardware.
 *
 * Further information can be found at http://beaglert.cc
 */


#ifndef BEAGLERT_H_
#define BEAGLERT_H_

#include <stdint.h>
#include <unistd.h>
#include <rtdk.h>
#include "digital_gpio_mapping.h"
#include <GPIOcontrol.h>

// Useful constants

/** \cond PRIVATE */
#define DBOX_CAPE					// New custom cape
#ifdef DBOX_CAPE
#define CODEC_I2C_ADDRESS  0x18		// Address of TLV320AIC3104 codec
#else
#define CODEC_I2C_ADDRESS  0x1B		// Address of TLV320AIC3106 codec
#endif

#define MAX_PRU_FILENAME_LENGTH 256
#define MAX_SERVERNAME_LENGTH 256
/** \endcond */

/**
 * \ingroup auxtask
 *
 * Xenomai priority level for audio processing. Maximum possible priority is 99.
 * In general, all auxiliary tasks should have a level lower than this unless for\
 * special purposes where the task needs to interrupt audio processing.
 */
#define BEAGLERT_AUDIO_PRIORITY		95

// Default volume levels

/**
 * \addtogroup levels
 *
 * @{
 */

/**
 * Default level of the audio DAC in decibels. See BeagleRT_setDACLevel().
 */
#define DEFAULT_DAC_LEVEL	0.0

/**
 * Default level of the audio ADC in decibels. See BeagleRT_setADCLevel().
 */
#define DEFAULT_ADC_LEVEL	-6.0


/**
 * Default level of the Programmable Gain Amplifier in decibels.
 */
#define DEFAULT_PGA_GAIN 16

/**
 * Default level of the headphone output in decibels. See BeagleRT_setHeadphoneLevel().
 */
#define DEFAULT_HP_LEVEL	-6.0
/** @} */

/**
 * Flag for BeagleRTContext. If set, indicates the audio and analog buffers are interleaved.
 */
#define BEAGLERT_FLAG_INTERLEAVED				(1 << 0)	// Set if buffers are interleaved
/**
 * Flag for BeagleRTContext. If set, indicates analog outputs persist for future frames.
 */
#define BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST	(1 << 1)	// Set if analog/digital outputs persist for future buffers

/**
 * \ingroup control
 * \brief Structure containing initialisation parameters for the real-time
 * audio control system.
 *
 * This structure is initialised using BeagleRT_defaultSettings(). Its contents
 * are used up through the point of calling
 * BeagleRT_initAudio() at which point it is no longer needed.
 */
typedef struct {
	// These items might be adjusted by the user:

	/// \brief Number of (analog) frames per period.
	///
	/// Number of audio frames depends on relative sample rates of the two. By default,
	/// audio is twice the sample rate, so has twice the period size.
	int periodSize;
	/// Whether to use the analog input and output
	int useAnalog;
	/// Whether to use the 16 programmable GPIOs
	int useDigital;
	/// How many channels for the ADC and DAC
	int numAnalogChannels;
	/// How many channels for the GPIOs
	int numDigitalChannels;

	/// Whether to begin with the speakers muted
	int beginMuted;
	/// Level for the audio DAC output
	float dacLevel;
	/// Level for the audio ADC input
	float adcLevel;
	/// Gains for the PGA, left and right channels
	float pgaGain[2];
	/// Level for the headphone output
	float headphoneLevel;

	/// The external .bin file to load. If empty will use PRU code from pru_rtaudio_bin.h
	char pruFilename[MAX_PRU_FILENAME_LENGTH];
	/// Whether to use verbose logging
	int verbose;

	// These items are application-dependent but should probably be
	// determined by the programmer rather than the user

	/// Whether audio/analog data should be interleaved
	int interleave;
	/// \brief Whether analog outputs should persist to future frames.
	///
	/// n.b. digital pins always persist, audio never does
	int analogOutputsPersist;

	// These items are hardware-dependent and should only be changed
	// to run on different hardware

	/// Where the codec can be found on the I2C bus
	int codecI2CAddress;
	/// Pin where amplifier mute can be found
	int ampMutePin;
	/// Port where the UDP server will listen
	int receivePort;
	/// Port where the UDP client will transmit
	int transmitPort;
	char serverName[MAX_SERVERNAME_LENGTH];
} BeagleRTInitSettings;


/**
 * \ingroup render
 * \brief Structure holding current audio and sensor settings and pointers to data buffers.
 *
 * This structure is passed to setup(), render() and cleanup(). It is
 * initialised in BeagleRT_initAudio() based on the contents of the BeagleRTInitSettings
 * structure.
 */
typedef struct {
	/// \brief Buffer holding audio input samples
	///
	/// This buffer may be in either interleaved or non-interleaved format,
	/// depending on the contents of the BeagleRTInitSettings structure.
	/// \b Note: this element is available in render() only.
	float *audioIn;

	/// \brief Buffer holding audio output samples
	///
	/// This buffer may be in either interleaved or non-interleaved format,
	/// depending on the contents of the BeagleRTInitSettings structure.
	/// \b Note: this element is available in render() only.
	float *audioOut;

	/// \brief Buffer holding analog input samples
	///
	/// This buffer may be in either interleaved or non-interleaved format,
	/// depending on the contents of the BeagleRTInitSettings structure.
	/// \b Note: this element is available in render() only.
	float *analogIn;

	/// \brief Buffer holding analog output samples
	///
	/// This buffer may be in either interleaved or non-interleaved format,
	/// depending on the contents of the BeagleRTInitSettings structure.
	/// \b Note: this element is available in render() only.
	float *analogOut;

	/// \brief Buffer holding digital input/output samples
	///
	/// \b Note: this element is available in render() only.
	uint32_t *digital;

	/// Number of audio frames per period
	uint32_t audioFrames;
	/// Number of audio channels (currently always 2)
	uint32_t audioChannels;
	/// Audio sample rate in Hz (currently always 44100.0)
	float audioSampleRate;

	/// \brief Number of analog frames per period
	///
	/// This will be 0 if analog I/O is disabled.
	uint32_t analogFrames;

	/// \brief Number of analog channels
	///
	/// This could take a value of 8, 4 or 2. This will be 0 if analog I/O is disabled.
	uint32_t analogChannels;

	/// \brief Analog sample rate in Hz
	///
	/// The analog sample rate depends on the number of analog channels used. If
	/// 8 channels are used, the sample rate is 22050. If 4 channels are used, the sample
	/// rate is 44100. If 2 channels are used, the sample rate is 88200. If analog I/O
	/// is disabled, the sample rate is 0.
	float analogSampleRate;

	/// Number of digital frames per period
	uint32_t digitalFrames;
	/// \brief Number of digital channels
	///
	/// Currently this will always be 16, unless digital I/O is disabled, in which case it will be 0.
	uint32_t digitalChannels;
	/// Digital sample rate in Hz (currently always 44100.0)
	float digitalSampleRate;

	/// \brief Number of elapsed audio samples since the start of rendering.
	///
	/// This holds the total number of audio samples as of the beginning of the current period. To
	/// find the current number of analog or digital samples elapsed, multiply by the ratio of the
	/// sample rates (e.g. half the number of analog samples will have elapsed if the analog sample
	/// rate is 22050).
	uint64_t audioSampleCount;

	/// \brief Other audio/sensor settings
	///
	/// Binary combination of flags including:
	///
	/// BEAGLERT_FLAG_INTERLEAVED: indicates the audio and analog buffers are interleaved
	///
	/// BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST: indicates that writes to the analog outputs will
	/// persist for future frames. If not set, writes affect one frame only.
	uint32_t flags;
} BeagleRTContext;

/** \ingroup auxtask
 *
 * Auxiliary task variable. Auxiliary tasks are created using createAuxiliaryTask() and
 * automatically cleaned up after cleanup() finishes.
 */
typedef void* AuxiliaryTask;	// Opaque data type to keep track of aux tasks

/** \ingroup render
 *
 * Flag that indicates when the audio will stop. Threads can poll this variable to indicate when
 * they should stop. Additionally, a program can set this to \c true
 * to indicate that audio processing should terminate. Calling BeagleRT_stopAudio()
 * has the effect of setting this to \c true.
 */
extern int gShouldStop;

// *** User-defined render functions ***

/**
 * \defgroup render User-defined render functions
 *
 * These three functions must be implemented by the developer in every BeagleRT program.
 * Typically they appear in their own .cpp source file.
 *
 * @{
 */

/**
 * \brief User-defined initialisation function which runs before audio rendering begins.
 *
 * This function runs once at the beginning of the program, after most of the system
 * initialisation has begun but before audio rendering starts. Use it to prepare any
 * memory or resources that will be needed in render().
 *
 * \param context Data structure holding information on sample rates, numbers of channels,
 * frame sizes and other state. Note: the buffers for audio, analog and digital data will
 * \b not yet be available to use. Do not attempt to read or write audio or sensor data
 * in setup().
 * \param userData An opaque pointer to an optional user-defined data structure. Whatever
 * is passed as the second argument to BeagleRT_initAudio() will appear here.
 *
 * \return true on success, or false if an error occurred. If no initialisation is
 * required, setup() should return true.
 */
bool setup(BeagleRTContext *context, void *userData);

/**
 * \brief User-defined callback function to process audio and sensor data.
 *
 * This function is called regularly by the system every time there is a new block of
 * audio and/or sensor data to process. Your code should process the requested samples
 * of data, store the results within \c context, and return.
 *
 * \param context Data structure holding buffers for audio, analog and digital data. The
 * structure also holds information on numbers of channels, frame sizes and sample rates,
 * which are guaranteed to remain the same throughout the program and to match what was
 * passed to setup().
 * \param userData An opaque pointer to an optional user-defined data structure. Will
 * be the same as the \c userData parameter passed to setup().
 */
void render(BeagleRTContext *context, void *userData);

/**
 * \brief User-defined cleanup function which runs when the program finishes.
 *
 * This function is called by the system once after audio rendering has finished, before the
 * program quits. Use it to release any memory allocated in setup() and to perform
 * any other required cleanup. If no initialisation is performed in setup(), then
 * this function will usually be empty.
 *
 * \param context Data structure holding information on sample rates, numbers of channels,
 * frame sizes and other state. Note: the buffers for audio, analog and digital data will
 * no longer be available to use. Do not attempt to read or write audio or sensor data
 * in cleanup().
 * \param userData An opaque pointer to an optional user-defined data structure. Will
 * be the same as the \c userData parameter passed to setup() and render().
 */
void cleanup(BeagleRTContext *context, void *userData);

/** @} */

/**
 * \defgroup control Control and command line functions
 *
 * These functions are used to initialise the BeagleRT settings, process arguments
 * from the command line, and start/stop the audio and sensor system.
 *
 * @{
 */

// *** Command-line settings ***

/**
 * \brief Initialise the data structure containing settings for BeagleRT.
 *
 * This function should be called in main() before parsing any command-line arguments. It
 * sets default values in the data structure which specifies the BeagleRT settings, including
 * frame sizes, numbers of channels, volume levels and other parameters.
 *
 * \param settings Structure holding initialisation data for BeagleRT.
 */
void BeagleRT_defaultSettings(BeagleRTInitSettings *settings);

/**
 * \brief Get long options from command line argument list, including BeagleRT standard options
 *
 * This function should be used in main() to process command line options, in place of the
 * standard library getopt_long(). Internally, it parses standard BeagleRT command-line options,
 * storing the results in the settings data structure. Any options which are not part of the
 * BeagleRT standard options will be returned, as they would normally be in getopt_long().
 *
 * \param argc Number of command line options, as passed to main().
 * \param argv Array of command line options, as passed to main().
 * \param customShortOptions List of short options to be parsed, analogous to getopt_long(). This
 * list should not include any characters already parsed as part of the BeagleRT standard options.
 * \param customLongOptions List of long options to parsed, analogous to getopt_long(). This
 * list should not include any long options already parsed as part of the BeagleRT standard options.
 * \param settings Data structure holding initialisation settings for BeagleRT. Any standard options
 * parsed will automatically update this data structure.
 *
 * \return Value of the next option parsed which is not a BeagleRT standard option, or -1 when the
 * argument list has been exhausted. Similar to the return value of getopt_long() except that BeagleRT
 * standard options are handled internally and not returned.
 */
int BeagleRT_getopt_long(int argc, char *argv[], const char *customShortOptions,
				   const struct option *customLongOptions, BeagleRTInitSettings *settings);

/**
 * \brief Print usage information for BeagleRT standard options.
 *
 * This function should be called from your code wherever you wish to print usage information for the
 * user. It will print usage information on BeagleRT standard options, after which you can print usage
 * information for your own custom options.
 */
void BeagleRT_usage();

/**
 * \brief Set level of verbose (debugging) printing.
 *
 * \param level Verbosity level of the internal BeagleRT system. 0 by default; higher values will
 * print more information. Presently all positive numbers produce the same level of printing.
 */
void BeagleRT_setVerboseLevel(int level);


// *** Audio control functions ***

/**
 * \brief Initialise audio and sensor rendering environment.
 *
 * This function prepares audio rendering in BeagleRT. It should be called from main() sometime
 * after command line option parsing has finished. It will initialise the rendering system, which
 * in the process will result in a call to the user-defined setup() function.
 *
 * \param settings Data structure holding system settings, including numbers of channels, frame sizes,
 * volume levels and other information.
 * \param userData An opaque pointer to a user-defined data structure which will be passed to
 * setup(), render() and cleanup(). You can use this to pass custom information
 * to the rendering functions, as an alternative to using global variables.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_initAudio(BeagleRTInitSettings *settings, void *userData);

/**
 * \brief Begin processing audio and sensor data.
 *
 * This function will start the BeagleRT audio/sensor system. After this function is called, the
 * system will make periodic calls to render() until BeagleRT_stopAudio() is called.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_startAudio();

/**
 * \brief Stop processing audio and sensor data.
 *
 * This function will stop the BeagleRT audio/sensor system. After this function returns, no further
 * calls to render() will be issued.
 */
int BeagleRT_startAuxiliaryTask(AuxiliaryTask it);
void BeagleRT_stopAudio();

/**
 * \brief Clean up resources from audio and sensor processing.
 *
 * This function should only be called after BeagleRT_stopAudio(). It will release any
 * internal resources for audio and sensor processing. In the process, it will call the
 * user-defined cleanup() function.
 */
void BeagleRT_cleanupAudio();

/** @} */

/**
 * \defgroup levels Audio level controls
 *
 * These functions control the input and output levels for the audio codec. If a BeagleRT program
 * does not call these functions, sensible default levels will be used.
 *
 * @{
 */

// *** Volume and level controls ***

/**
 * \brief Set the level of the audio DAC.
 *
 * This function sets the level of all audio outputs (headphone, line, speaker). It does
 * not affect the level of the (non-audio) analog outputs.
 *
 * \b Important: do not call this function from within render(), as it does not make
 * any guarantees on real-time performance.
 *
 * \param decibels Level of the DAC output. Valid levels range from -63.5 (lowest) to
 * 0 (highest) in steps of 0.5dB. Levels between increments of 0.5 will be rounded down.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_setDACLevel(float decibels);

/**
 * \brief Set the level of the audio ADC.
 *
 * This function sets the level of the audio input. It does not affect the level of the
 * (non-audio) analog inputs.
 *
 * \b Important: do not call this function from within render(), as it does not make
 * any guarantees on real-time performance.
 *
 * \param decibels Level of the ADC input. Valid levels range from -12 (lowest) to
 * 0 (highest) in steps of 1.5dB. Levels between increments of 1.5 will be rounded down.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_setADCLevel(float decibels);


/**
 * \brief Set the gain of the audio preamplifier.
 *
 * This function sets the level of the Programmable Gain Amplifier(PGA), which
 * amplifies the signal before the ADC.
 *
 * \b Important: do not call this function from within render(), as it does not make
 * any guarantees on real-time performance.
 *
 * \param decibels Level of the PGA Valid levels range from 0 (lowest) to
 * 59.5 (highest) in steps of 0.5dB. Levels between increments of 0.5 will be rounded.
 * \param channel Specifies which channel to apply the gain to. Channel 0 is left,
 * channel 1 is right
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_setPgaGain(float decibels, int channel);

/**
 * \brief Set the level of the onboard headphone amplifier.
 *
 * This function sets the level of the headphone output only (3-pin connector on the BeagleRT
 * cape or the output jack on the BeagleBone Audio Cape). It does not affect the level of the
 * speakers or the line out pads on the cape.
 *
 * \b Important: do not call this function from within render(), as it does not make
 * any guarantees on real-time performance.
 *
 * \param decibels Level of the DAC output. Valid levels range from -63.5 (lowest) to
 * 0 (highest) in steps of 0.5dB. Levels between increments of 0.5 will be rounded down.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_setHeadphoneLevel(float decibels);

/**
 * \brief Mute or unmute the onboard speaker amplifiers.
 *
 * This function mutes or unmutes the amplifiers on the BeagleRT cape. Whether the speakers begin
 * muted or unmuted depends on the BeagleRTInitSettings structure passed to BeagleRT_initAudio().
 *
 * \b Important: do not call this function from within render(), as it does not make
 * any guarantees on real-time performance.
 *
 * \param mute 0 to enable the speakers, nonzero to mute the speakers.
 *
 * \return 0 on success, or nonzero if an error occurred.
 */
int BeagleRT_muteSpeakers(int mute);

/** @} */

/**
 * \defgroup auxtask Auxiliary task support
 *
 * These functions are used to create separate real-time tasks (threads) which run at lower
 * priority than the audio processing. They can be used, for example, for large time-consuming
 * calculations which would take more than one audio frame length to process, or they could be
 * used to communicate with external hardware when that communication might block or be delayed.
 *
 * All auxiliary tasks used by the program should be created in setup(). The tasks
 * can then be scheduled at will within the render() function.
 *
 * @{
 */

// *** Functions for creating auxiliary tasks ***

/**
 * \brief Create a new auxiliary task.
 *
 * This function creates a new auxiliary task which, when scheduled, runs the function specified
 * in the first argument. Note that the task does not run until scheduleAuxiliaryTask() is called.
 * Auxiliary tasks should be created in setup() and never in render() itself.
 *
 * The second argument specifies the real-time priority. Valid values are between 0
 * and 99, and usually should be lower than \ref BEAGLERT_AUDIO_PRIORITY. Tasks with higher priority always
 * preempt tasks with lower priority.
 *
 * \param functionToCall Function which will run each time the auxiliary task is scheduled.
 * \param priority Xenomai priority level at which the task should run.
 * \param name Name for this task, which should be unique system-wide (no other running program should use this name).
 */
AuxiliaryTask BeagleRT_createAuxiliaryTask(void (*functionToCall)(void), int priority, const char *name);

/**
 * \brief Start an auxiliary task so that it can be run.
 *
 * This function will start an auxiliary task but will NOT schedule it.
 * It will also set a flag in the associate InternalAuxiliaryTask to flag the
 * task as "started", so that successive calls to the same function for a given AuxiliaryTask
 * have no effect.
 * The user should never be required to call this function directly, as it is called
 * by BeagleRT_scheduleAuxiliaryTask if needed (e.g.: if a task is scheduled in setup() )
 * or immediately after starting the audio thread.
 *
* \param task Task to start.
 */

int BeagleRT_startAuxiliaryTask(AuxiliaryTask task);
/**
 * \brief Run an auxiliary task which has previously been created.
 *
 * This function will schedule an auxiliary task to run. When the task runs, the function in the first
 * argument of createAuxiliaryTaskLoop() will be called.
 *
 * scheduleAuxiliaryTask() is typically called from render() to start a lower-priority task. The function
 * will not run immediately, but only once any active higher priority tasks have finished.
 *
 * \param task Task to schedule for running.
 */
void BeagleRT_scheduleAuxiliaryTask(AuxiliaryTask task);

/** @} */
#include <Utilities.h>

#endif /* BEAGLERT_H_ */
