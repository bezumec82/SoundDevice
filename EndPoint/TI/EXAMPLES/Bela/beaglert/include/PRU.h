/*
 * PRU.h
 *
 *  Created on: May 27, 2014
 *      Author: andrewm
 */

#ifndef PRU_H_
#define PRU_H_

#include <stdint.h>
#include <native/intr.h>
#include "../include/BeagleRT.h"

class PRU
{
private:
	static const unsigned int kPruGPIODACSyncPin;
	static const unsigned int kPruGPIOADCSyncPin;
	static const unsigned int kPruGPIOTestPin;
	static const unsigned int kPruGPIOTestPin2;
	static const unsigned int kPruGPIOTestPin3;

public:
	// Constructor
	PRU(BeagleRTContext *input_context);

	// Destructor
	~PRU();

	// Prepare the GPIO pins needed for the PRU
	int prepareGPIO(int include_test_pin, int include_led);

	// Clean up the GPIO at the end
	void cleanupGPIO();

	// Initialise and open the PRU
	int initialise(int pru_num, int frames_per_buffer,
				   int spi_channels, bool xenomai_test_pin = false);

	// Run the code image in pru_rtaudio_bin.h
	int start(char * const filename);

	// Loop: read and write data from the PRU
	void loop(RT_INTR *pru_interrupt, void *userData);

	// Wait for an interrupt from the PRU indicate it is finished
	void waitForFinish();

	// Turn off the PRU when done
	void disable();

	// For debugging:
	void setGPIOTestPin();
	void clearGPIOTestPin();

private:
	BeagleRTContext *context;	// Overall settings

	int pru_number;		// Which PRU we use
	bool running;		// Whether the PRU is running
	bool analog_enabled;  // Whether SPI ADC and DAC are used
	bool digital_enabled; // Whether digital is used
	bool gpio_enabled;	// Whether GPIO has been prepared
	bool led_enabled;	// Whether a user LED is enabled
	bool gpio_test_pin_enabled; // Whether the test pin was also enabled

	volatile uint32_t *pru_buffer_comm;
	uint16_t *pru_buffer_spi_dac;
	uint16_t *pru_buffer_spi_adc;
	uint32_t *pru_buffer_digital;
	int16_t *pru_buffer_audio_dac;
	int16_t *pru_buffer_audio_adc;

	float *last_analog_out_frame;
	uint32_t *digital_buffer0, *digital_buffer1, *last_digital_buffer;

	int xenomai_gpio_fd;	// File descriptor for /dev/mem for fast GPIO
	uint32_t *xenomai_gpio;	// Pointer to GPIO registers
};


#endif /* PRU_H_ */
