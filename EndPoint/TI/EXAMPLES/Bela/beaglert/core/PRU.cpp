/*
 * PRU.cpp
 *
 * Code for communicating with the Programmable Realtime Unit (PRU)
 * on the BeagleBone AM335x series processors. The PRU loads and runs
 * a separate code image compiled from an assembly file. Here it is
 * used to handle audio and SPI ADC/DAC data.
 *
 * This code is specific to the PRU code in the assembly file; for example,
 * it uses certain GPIO resources that correspond to that image.
 *
 *  Created on: May 27, 2014
 *      Author: andrewm
 */

#include "../include/PRU.h"
#include "../include/prussdrv.h"
#include "../include/pruss_intc_mapping.h"
#include "../include/digital_gpio_mapping.h"
#include "../include/GPIOcontrol.h"
#include "../include/BeagleRT.h"
#include "../include/pru_rtaudio_bin.h"

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Xenomai-specific includes
#include <sys/mman.h>
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

using namespace std;

#define PRU_MEM_MCASP_OFFSET 0x2000  // Offset within PRU-SHARED RAM
#define PRU_MEM_MCASP_LENGTH 0x2000  // Length of McASP memory, in bytes
#define PRU_MEM_DAC_OFFSET 0x0     // Offset within PRU0 RAM
#define PRU_MEM_DAC_LENGTH 0x2000  // Length of ADC+DAC memory, in bytes
#define PRU_MEM_COMM_OFFSET 0x0    // Offset within PRU-SHARED RAM
#define PRU_MEM_DIGITAL_OFFSET 0x1000 //Offset within PRU-SHARED RAM
#define MEM_DIGITAL_BUFFER1_OFFSET 0x400 //Start pointer to DIGITAL_BUFFER1, which is 256 words.
											// 256 is the maximum number of frames allowed
#define PRU_SHOULD_STOP 	0
#define PRU_CURRENT_BUFFER  1
#define PRU_BUFFER_FRAMES   2
#define PRU_SHOULD_SYNC     3
#define PRU_SYNC_ADDRESS    4
#define PRU_SYNC_PIN_MASK   5
#define PRU_LED_ADDRESS		6
#define PRU_LED_PIN_MASK	7
#define PRU_FRAME_COUNT		8
#define PRU_USE_SPI			9
#define PRU_SPI_NUM_CHANNELS 10
#define PRU_USE_DIGITAL    11

short int digitalPins[NUM_DIGITALS]={
		GPIO_NO_BIT_0,
		GPIO_NO_BIT_1,
		GPIO_NO_BIT_2,
		GPIO_NO_BIT_3,
		GPIO_NO_BIT_4,
		GPIO_NO_BIT_5,
		GPIO_NO_BIT_6,
		GPIO_NO_BIT_7,
		GPIO_NO_BIT_8,
		GPIO_NO_BIT_9,
		GPIO_NO_BIT_10,
		GPIO_NO_BIT_11,
		GPIO_NO_BIT_12,
		GPIO_NO_BIT_13,
		GPIO_NO_BIT_14,
		GPIO_NO_BIT_15,
};

#define PRU_SAMPLE_INTERVAL_NS 11338	// 88200Hz per SPI sample = 11.338us

#define GPIO0_ADDRESS 		0x44E07000
#define GPIO1_ADDRESS 		0x4804C000
#define GPIO_SIZE			0x198
#define GPIO_CLEARDATAOUT 	(0x190 / 4)
#define GPIO_SETDATAOUT 	(0x194 / 4)

#define TEST_PIN_GPIO_BASE	GPIO0_ADDRESS	// Use GPIO0(31) for debugging
#define TEST_PIN_MASK		(1 << 31)
#define TEST_PIN2_MASK		(1 << 26)

#define USERLED3_GPIO_BASE  GPIO1_ADDRESS // GPIO1(24) is user LED 3
#define USERLED3_PIN_MASK   (1 << 24)

const unsigned int PRU::kPruGPIODACSyncPin = 5;	// GPIO0(5); P9-17
const unsigned int PRU::kPruGPIOADCSyncPin = 48; // GPIO1(16); P9-15

const unsigned int PRU::kPruGPIOTestPin = 60;	// GPIO1(28); P9-12
const unsigned int PRU::kPruGPIOTestPin2 = 31;	// GPIO0(31); P9-13
const unsigned int PRU::kPruGPIOTestPin3 = 26;	// GPIO0(26); P8-14

extern int gShouldStop;
extern int gRTAudioVerbose;

// Constructor: specify a PRU number (0 or 1)
PRU::PRU(BeagleRTContext *input_context)
: context(input_context), pru_number(0), running(false), analog_enabled(false),
  digital_enabled(false), gpio_enabled(false), led_enabled(false),
  gpio_test_pin_enabled(false),
  pru_buffer_comm(0), pru_buffer_spi_dac(0), pru_buffer_spi_adc(0),
  pru_buffer_digital(0), pru_buffer_audio_dac(0), pru_buffer_audio_adc(0),
  xenomai_gpio_fd(-1), xenomai_gpio(0)
{

}

// Destructor
PRU::~PRU()
{
	if(running)
		disable();
	if(gpio_enabled)
		cleanupGPIO();
	if(xenomai_gpio_fd >= 0)
		close(xenomai_gpio_fd);
}

// Prepare the GPIO pins needed for the PRU
// If include_test_pin is set, the GPIO output
// is also prepared for an output which can be
// viewed on a scope. If include_led is set,
// user LED 3 on the BBB is taken over by the PRU
// to indicate activity
int PRU::prepareGPIO(int include_test_pin, int include_led)
{
	if(context->analogFrames != 0) {
		// Prepare DAC CS/ pin: output, high to begin
		if(gpio_export(kPruGPIODACSyncPin)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export DAC sync pin\n";
		}
		if(gpio_set_dir(kPruGPIODACSyncPin, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on DAC sync pin\n";
			return -1;
		}
		if(gpio_set_value(kPruGPIODACSyncPin, HIGH)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on DAC sync pin\n";
			return -1;
		}

		// Prepare ADC CS/ pin: output, high to begin
		if(gpio_export(kPruGPIOADCSyncPin)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export ADC sync pin\n";
		}
		if(gpio_set_dir(kPruGPIOADCSyncPin, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on ADC sync pin\n";
			return -1;
		}
		if(gpio_set_value(kPruGPIOADCSyncPin, HIGH)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on ADC sync pin\n";
			return -1;
		}

		analog_enabled = true;
	}

	if(context->digitalFrames != 0){
		for(unsigned int i = 0; i < context->digitalChannels; i++){
			if(gpio_export(digitalPins[i])) {
				if(gRTAudioVerbose)
					cerr << "Warning: couldn't export digital GPIO pin " << digitalPins[i] << "\n"; // this is left as a warning because if the pin has been exported by somebody else, can still be used
			}
			if(gpio_set_dir(digitalPins[i], INPUT_PIN)) {
				if(gRTAudioVerbose)
					cerr << "Error: Couldn't set direction on digital GPIO pin " << digitalPins[i] << "\n";
				return -1;
			}
		}
		digital_enabled = true;
	}

	if(include_test_pin) {
		// Prepare GPIO test output (for debugging), low to begin
		if(gpio_export(kPruGPIOTestPin)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export GPIO test pin\n";
		}
		if(gpio_set_dir(kPruGPIOTestPin, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on GPIO test pin\n";
			return -1;
		}
		if(gpio_set_value(kPruGPIOTestPin, LOW)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on GPIO test pin\n";
			return -1;
		}

		if(gpio_export(kPruGPIOTestPin2)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export GPIO test pin 2\n";
		}
		if(gpio_set_dir(kPruGPIOTestPin2, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on GPIO test pin 2\n";
			return -1;
		}
		if(gpio_set_value(kPruGPIOTestPin2, LOW)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on GPIO test pin 2\n";
			return -1;
		}

		if(gpio_export(kPruGPIOTestPin3)) {
			if(gRTAudioVerbose)
				cout << "Warning: couldn't export GPIO test pin 3\n";
		}
		if(gpio_set_dir(kPruGPIOTestPin3, OUTPUT_PIN)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set direction on GPIO test pin 3\n";
			return -1;
		}
		if(gpio_set_value(kPruGPIOTestPin3, LOW)) {
			if(gRTAudioVerbose)
				cout << "Couldn't set value on GPIO test pin 3\n";
			return -1;
		}
		gpio_test_pin_enabled = true;
	}

	if(include_led) {
		// Turn off system function for LED3 so it can be reused by PRU
		led_set_trigger(3, "none");
		led_enabled = true;
	}

	gpio_enabled = true;

	return 0;
}

// Clean up the GPIO at the end
void PRU::cleanupGPIO()
{
	if(!gpio_enabled)
		return;
	if(analog_enabled) {
		gpio_unexport(kPruGPIODACSyncPin);
		gpio_unexport(kPruGPIOADCSyncPin);
	}
	if(digital_enabled){
		for(unsigned int i = 0; i < context->digitalChannels; i++){
			gpio_unexport(digitalPins[i]);
		}
	}
	if(gpio_test_pin_enabled) {
		gpio_unexport(kPruGPIOTestPin);
		gpio_unexport(kPruGPIOTestPin2);
		gpio_unexport(kPruGPIOTestPin3);
	}
	if(led_enabled) {
		// Set LED back to default eMMC status
		// TODO: make it go back to its actual value before this program,
		// rather than the system default
		led_set_trigger(3, "mmc1");
	}
	gpio_enabled = gpio_test_pin_enabled = false;
}

// Initialise and open the PRU
int PRU::initialise(int pru_num, int frames_per_buffer, int spi_channels, bool xenomai_test_pin)
{
	uint32_t *pruMem = 0;

	if(!gpio_enabled) {
		rt_printf("initialise() called before GPIO enabled\n");
		return 1;
	}

	pru_number = pru_num;

    /* Initialize structure used by prussdrv_pruintc_intc   */
    /* PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h */
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

    /* Allocate and initialize memory */
    prussdrv_init();
    if(prussdrv_open(PRU_EVTOUT_0)) {
    	rt_printf("Failed to open PRU driver\n");
    	return 1;
    }

    /* Map PRU's INTC */
    prussdrv_pruintc_init(&pruss_intc_initdata);

    /* Map PRU memory to pointers */
	prussdrv_map_prumem (PRUSS0_SHARED_DATARAM, (void **)&pruMem);
    pru_buffer_comm = (uint32_t *)&pruMem[PRU_MEM_COMM_OFFSET/sizeof(uint32_t)];
	pru_buffer_audio_dac = (int16_t *)&pruMem[PRU_MEM_MCASP_OFFSET/sizeof(uint32_t)];

	/* ADC memory starts 2(ch)*2(buffers)*bufsize samples later */
	pru_buffer_audio_adc = &pru_buffer_audio_dac[4 * context->audioFrames];

	if(analog_enabled) {
		prussdrv_map_prumem (pru_number == 0 ? PRUSS0_PRU0_DATARAM : PRUSS0_PRU1_DATARAM, (void **)&pruMem);
		pru_buffer_spi_dac = (uint16_t *)&pruMem[PRU_MEM_DAC_OFFSET/sizeof(uint32_t)];

		/* ADC memory starts after N(ch)*2(buffers)*bufsize samples */
		pru_buffer_spi_adc = &pru_buffer_spi_dac[2 * context->analogChannels * context->analogFrames];
	}
	else {
		pru_buffer_spi_dac = pru_buffer_spi_adc = 0;
	}

	if(digital_enabled) {
		prussdrv_map_prumem (PRUSS0_SHARED_DATARAM, (void **)&pruMem);
		pru_buffer_digital = (uint32_t *)&pruMem[PRU_MEM_DIGITAL_OFFSET/sizeof(uint32_t)];
	}
	else {
		pru_buffer_digital = 0;
	}

    /* Set up flags */
    pru_buffer_comm[PRU_SHOULD_STOP] = 0;
    pru_buffer_comm[PRU_CURRENT_BUFFER] = 0;
    pru_buffer_comm[PRU_BUFFER_FRAMES] = context->analogFrames;
    pru_buffer_comm[PRU_SHOULD_SYNC] = 0;
    pru_buffer_comm[PRU_SYNC_ADDRESS] = 0;
    pru_buffer_comm[PRU_SYNC_PIN_MASK] = 0;
    if(led_enabled) {
    	pru_buffer_comm[PRU_LED_ADDRESS] = USERLED3_GPIO_BASE;
    	pru_buffer_comm[PRU_LED_PIN_MASK] = USERLED3_PIN_MASK;
    }
    else {
    	pru_buffer_comm[PRU_LED_ADDRESS] = 0;
    	pru_buffer_comm[PRU_LED_PIN_MASK] = 0;
    }
    if(analog_enabled) {
    	pru_buffer_comm[PRU_USE_SPI] = 1;
    	pru_buffer_comm[PRU_SPI_NUM_CHANNELS] = context->analogChannels;
    }
    else {
    	pru_buffer_comm[PRU_USE_SPI] = 0;
    	pru_buffer_comm[PRU_SPI_NUM_CHANNELS] = 0;
    }
    if(digital_enabled) {
    	pru_buffer_comm[PRU_USE_DIGITAL] = 1;
//TODO: add mask
    }
    else {
    	pru_buffer_comm[PRU_USE_DIGITAL] = 0;

    }

    /* Clear ADC and DAC memory.*/
    //TODO: this initialisation should only address the memory effectively used by these buffers, i.e.:depend on the number of frames
    //  (otherwise might cause issues if we move memory locations later on)
    if(analog_enabled) {
		for(int i = 0; i < PRU_MEM_DAC_LENGTH / 2; i++)
			pru_buffer_spi_dac[i] = 0;
		if(digital_enabled){
			for(int i = 0; i < PRU_MEM_DIGITAL_OFFSET*2; i++)
				pru_buffer_digital[i] = 0x0000ffff; // set to all inputs, to avoid unexpected spikes
		}
    }
	for(int i = 0; i < PRU_MEM_MCASP_LENGTH / 2; i++)
		pru_buffer_audio_dac[i] = 0;

	/* If using GPIO test pin for Xenomai (for debugging), initialise the pointer now */
	if(xenomai_test_pin && xenomai_gpio_fd < 0) {
		xenomai_gpio_fd = open("/dev/mem", O_RDWR);
		if(xenomai_gpio_fd < 0)
			rt_printf("Unable to open /dev/mem for GPIO test pin\n");
		else {
			xenomai_gpio = (uint32_t *)mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, xenomai_gpio_fd, TEST_PIN_GPIO_BASE);
			if(xenomai_gpio == MAP_FAILED) {
				rt_printf("Unable to map GPIO address for test pin\n");
				xenomai_gpio = 0;
				close(xenomai_gpio_fd);
				xenomai_gpio_fd = -1;
			}
		}
	}

	// Allocate audio buffers
	context->audioIn = (float *)malloc(2 * context->audioFrames * sizeof(float));
	context->audioOut = (float *)malloc(2 * context->audioFrames * sizeof(float));
	if(context->audioIn == 0 || context->audioOut == 0) {
		rt_printf("Error: couldn't allocate audio buffers\n");
		return 1;
	}

	// Allocate analog buffers
	if(analog_enabled) {
		context->analogIn = (float *)malloc(context->analogChannels * context->analogFrames * sizeof(float));
		context->analogOut = (float *)malloc(context->analogChannels * context->analogFrames * sizeof(float));
		last_analog_out_frame = (float *)malloc(context->analogChannels * sizeof(float));

		if(context->analogIn == 0 || context->analogOut == 0 || last_analog_out_frame == 0) {
			rt_printf("Error: couldn't allocate analog buffers\n");
			return 1;
		}

		memset(last_analog_out_frame, 0, context->analogChannels * sizeof(float));
	}

	// Allocate digital buffers
	digital_buffer0 = pru_buffer_digital;
	digital_buffer1 = pru_buffer_digital + MEM_DIGITAL_BUFFER1_OFFSET / sizeof(uint32_t);
	if(digital_enabled) {
		last_digital_buffer = (uint32_t *)malloc(context->digitalFrames * sizeof(uint32_t)); //temp buffer to hold previous states
		if(last_digital_buffer == 0) {
			rt_printf("Error: couldn't allocate digital buffers\n");
			return 1;
		}

		for(unsigned int n = 0; n < context->digitalFrames; n++){
			// Initialize lastDigitalFrames to all inputs
			last_digital_buffer[n] = 0x0000ffff;
		}
	}

	context->digital = digital_buffer0;

	return 0;
}

// Run the code image in the specified file
int PRU::start(char * const filename)
{
	/* Clear any old interrupt */
	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

	/* Load and execute binary on PRU */
	if(filename[0] == '\0') { //if the string is empty, load the embedded code
		if(gRTAudioVerbose)
			rt_printf("Using embedded PRU code\n");
		if(prussdrv_exec_code(pru_number, PRUcode, sizeof(PRUcode))) {
			rt_printf("Failed to execute PRU code\n");
			return 1;
		}
	} else {
		if(gRTAudioVerbose)
			rt_printf("Using PRU code from %s\n",filename);
		if(prussdrv_exec_program(pru_number, filename)) {
			rt_printf("Failed to execute PRU code from %s\n", filename);
			return 1;
		}
	}

    running = true;
    return 0;
}

// Main loop to read and write data from/to PRU
void PRU::loop(RT_INTR *pru_interrupt, void *userData)
{
#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
	RTIME irqTimeout = PRU_SAMPLE_INTERVAL_NS * 1024;	// Timeout for PRU interrupt: about 10ms, much longer than any expected period
#else
	// Polling interval is 1/4 of the period
	RTIME sleepTime = PRU_SAMPLE_INTERVAL_NS * (context->analogChannels / 2) * context->analogFrames / 4;
#endif

	uint32_t pru_audio_offset, pru_spi_offset;

	// Before starting, look at the last state of the analog and digital outputs which might
	// have been changed by the user during the setup() function. This lets us start with pin
	// directions and output values at something other than defaults.

	if(analog_enabled) {
		if(context->flags & BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST) {
			// Remember the content of the last_analog_out_frame
			for(unsigned int ch = 0; ch < context->analogChannels; ch++){
				last_analog_out_frame[ch] = context->analogOut[context->analogChannels * (context->analogFrames - 1) + ch];
			}
		}
	}

	if(digital_enabled) {
		for(unsigned int n = 0; n < context->digitalFrames; n++){
			last_digital_buffer[n] = context->digital[n];
		}
	}

	// TESTING
	// uint32_t testCount = 0;
	// RTIME startTime = rt_timer_read();

#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
	int result;
#else
	// Which buffer the PRU was last processing
	uint32_t lastPRUBuffer = 0;
#endif

	while(!gShouldStop) {
#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
		// Wait for PRU to move to change buffers;
		// PRU will send an interrupts which we wait for
		rt_intr_enable(pru_interrupt);
		while(!gShouldStop) {
			result = rt_intr_wait(pru_interrupt, irqTimeout);
			if(result >= 0)
				break;
			else if(result == -ETIMEDOUT)
				rt_printf("Warning: PRU timeout!\n");
			else {
				rt_printf("Error: wait for interrupt failed (%d)\n", result);
				gShouldStop = 1;
			}
		}

		// Clear pending PRU interrupt
		prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);
#else
		// Poll
		while(pru_buffer_comm[PRU_CURRENT_BUFFER] == lastPRUBuffer && !gShouldStop) {
			rt_task_sleep(sleepTime);
		}

		lastPRUBuffer = pru_buffer_comm[PRU_CURRENT_BUFFER];
#endif

		if(gShouldStop)
			break;

		// Check which buffer we're on-- will have been set right
		// before the interrupt was asserted
		if(pru_buffer_comm[PRU_CURRENT_BUFFER] == 1) {
			// PRU is on buffer 1. We read and write to buffer 0
			pru_audio_offset = 0;
			pru_spi_offset = 0;
			if(digital_enabled)
				context->digital = digital_buffer0;
		}
		else {
			// PRU is on buffer 0. We read and write to buffer 1
			pru_audio_offset = context->audioFrames * 2;
			pru_spi_offset = context->analogFrames * context->analogChannels;
			if(digital_enabled)
				context->digital = digital_buffer1;
		}

		// FIXME: some sort of margin is needed here to prevent the audio
		// code from completely eating the Linux system
		// testCount++;
		//rt_task_sleep(sleepTime*4);
		//rt_task_sleep(sleepTime/4);

		if(xenomai_gpio != 0) {
			// Set the test pin high
			xenomai_gpio[GPIO_SETDATAOUT] = TEST_PIN_MASK;
		}

		// Convert short (16-bit) samples to float
		// TODO: NEON
		for(unsigned int n = 0; n < 2 * context->audioFrames; n++)
			context->audioIn[n] = (float)pru_buffer_audio_adc[n + pru_audio_offset] / 32768.0f;

		if(analog_enabled) {
			// TODO: NEON
			for(unsigned int n = 0; n < context->analogChannels * context->analogFrames; n++)
				context->analogIn[n] = (float)pru_buffer_spi_adc[n + pru_spi_offset] / 65536.0f;

			if(context->flags & BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST) {
				// Initialize the output buffer with the values that were in the last frame of the previous output
				for(unsigned int ch = 0; ch < context->analogChannels; ch++){
					for(unsigned int n = 0; n < context->analogFrames; n++){
						context->analogOut[n * context->analogChannels + ch] = last_analog_out_frame[ch];
					}
				}
			}
			else {
				// Outputs are 0 unless set otherwise
				memset(context->analogOut, 0, context->analogChannels * context->analogFrames * sizeof(float));
			}
		}

        if(digital_enabled){
			// Use past digital values to initialize the array properly.
			// For each frame:
			// - pins previously set as outputs will keep the output value they had in the last frame of the previous buffer,
			// - pins previously set as inputs will carry the newly read input value

			for(unsigned int n = 0; n < context->digitalFrames; n++){
				uint16_t inputs = last_digital_buffer[n] & 0xffff; // half-word, has 1 for inputs and 0 for outputs

				uint16_t outputs = ~inputs; // half-word has 1 for outputs and 0 for inputs;
				context->digital[n] = (last_digital_buffer[context->digitalFrames - 1] & (outputs << 16)) | // keep output values set in the last frame of the previous buffer
									   (context->digital[n] & (inputs << 16))   | // inputs from current context->digital[n];
									   (last_digital_buffer[n] & (inputs));     // keep pin configuration from previous context->digital[n]
//                    context->digital[n]=digitalBufferTemp[n]; //ignores inputs
			}
		}

		// Call user render function
        // ***********************
		render(context, userData);
		// ***********************

		if(analog_enabled) {
			if(context->flags & BEAGLERT_FLAG_ANALOG_OUTPUTS_PERSIST) {
				// Remember the content of the last_analog_out_frame
				for(unsigned int ch = 0; ch < context->analogChannels; ch++){
					last_analog_out_frame[ch] = context->analogOut[context->analogChannels * (context->analogFrames - 1) + ch];
				}
			}

			// Convert float back to short for SPI output
			for(unsigned int n = 0; n < context->analogChannels * context->analogFrames; n++) {
				int out = context->analogOut[n] * 65536.0f;
				if(out < 0) out = 0;
				else if(out > 65535) out = 65535;
				pru_buffer_spi_dac[n + pru_spi_offset] = (uint16_t)out;
			}
		}

		if(digital_enabled) { // keep track of past digital values
			for(unsigned int n = 0; n < context->digitalFrames; n++){
				last_digital_buffer[n] = context->digital[n];
			}
		}

        // Convert float back to short for audio
		// TODO: NEON
		for(unsigned int n = 0; n < 2 * context->audioFrames; n++) {
			int out = context->audioOut[n] * 32768.0f;
			if(out < -32768) out = -32768;
			else if(out > 32767) out = 32767;
			pru_buffer_audio_dac[n + pru_audio_offset] = (int16_t)out;
		}

		// Increment total number of samples that have elapsed
		context->audioSampleCount += context->audioFrames;

		if(xenomai_gpio != 0) {
			// Set the test pin high
			xenomai_gpio[GPIO_CLEARDATAOUT] = TEST_PIN_MASK;
		}

		// FIXME: TESTING!!
		// if(testCount > 100000)
		//	break;
	}

#ifdef BEAGLERT_USE_XENOMAI_INTERRUPTS
	// Turn off the interrupt for the PRU if it isn't already off
	rt_intr_disable(pru_interrupt);
#endif

	// FIXME: TESTING
	// RTIME endTime = rt_timer_read();
	// RTIME diffTime = endTime - startTime;
	// rt_printf("%d blocks elapsed in %f seconds, %f Hz block rate\n", testCount, ((float)diffTime / 1.0e9), (float)testCount / ((float)diffTime / 1.0e9));

	// Tell PRU to stop
	pru_buffer_comm[PRU_SHOULD_STOP] = 1;

	// Wait two buffer lengths for the PRU to finish
	rt_task_sleep(PRU_SAMPLE_INTERVAL_NS * context->analogFrames * 4 * 2);

	// Clean up after ourselves
	free(context->audioIn);
	free(context->audioOut);

	if(analog_enabled) {
		free(context->analogIn);
		free(context->analogOut);
		free(last_analog_out_frame);
	}

	if(digital_enabled) {
		free(last_digital_buffer);
	}

	context->audioIn = context->audioOut = 0;
	context->analogIn = context->analogOut = 0;
	context->digital = 0;
}

// Wait for an interrupt from the PRU indicate it is finished
void PRU::waitForFinish()
{
	if(!running)
		return;
    prussdrv_pru_wait_event (PRU_EVTOUT_0);
	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
}

// Turn off the PRU when done
void PRU::disable()
{
    /* Disable PRU and close memory mapping*/
    prussdrv_pru_disable(pru_number);
    prussdrv_exit();
	running = false;
}

// Debugging
void PRU::setGPIOTestPin()
{
	if(!xenomai_gpio)
		return;
	xenomai_gpio[GPIO_SETDATAOUT] = TEST_PIN2_MASK;
}

void PRU::clearGPIOTestPin()
{
	if(!xenomai_gpio)
		return;
	xenomai_gpio[GPIO_CLEARDATAOUT] = TEST_PIN2_MASK;
}
