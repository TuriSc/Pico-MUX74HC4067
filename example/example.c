/* Pico MUX74HC4067 example
*
* This example expects an analog sensor of some kind, or a potentiometer wiping between GND and 3.3V,
* on pin C0 of the demuxer, and LEDs on all other channel pins.
* Each LED will also need its resistor of an appropriate value.
*/

#include <stdio.h>
#include <pico/stdlib.h>
#include <MUX74HC4067.h>
#include "hardware/adc.h"
#include <stdio.h>

#define MUX_EN                    14
#define MUX_SIG                   26
#define MUX_S0                    13
#define MUX_S1                    12
#define MUX_S2                    11
#define MUX_S3                    10

#include <stdlib.h>
int main() {
    stdio_init_all();

    // Initialize the ADC if you're going to read analog values
    adc_init();

    // Instantiate the mux
    mux74hc4067_t* mux = mux74hc4067_create(MUX_EN, MUX_S0, MUX_S1, MUX_S2, MUX_S3);
    
    // Define the signal pin. Now it's an analog input
    mux74hc4067_signal_pin(mux, MUX_SIG, MODE_INPUT, ANALOG, 0);
    // Please note that you can only read analog values
    // using GPIOs 26, 27, and 28

    // Wait for the serial connection to be established to
    // reliably receive serial messages
    sleep_ms(2000);

    // Read the analog value of pin 0
    int16_t value_c0 = mux74hc4067_read(mux, 0);
    printf("C0: %d\n", value_c0);

    // Re-define the signal pin. In this example it's a digital output
    mux74hc4067_signal_pin(mux, MUX_SIG, MODE_OUTPUT, DIGITAL, 0);

    // Turn on a single LED for 1 second
    mux74hc4067_write(mux, 2, 1, DIGITAL);
    sleep_ms(1000);

    // Turn on multiple LEDs by rapidly flicking through them
    int16_t counter = 0;
    uint8_t leds_on[] = {1, 3, 7, 11, 15};
    while (++counter < 1000) {
        static uint8_t ticker;
        int8_t selected_led = leds_on[ticker++ % 5];
        if (selected_led > -1){
            mux74hc4067_write(mux, selected_led, 1, 0);
            sleep_ms(1);
        }
    }

    // Now cycle through all the LEDs, one at a time
    while (true) {
        for (uint8_t i = 0; i < 16; ++i) {
            mux74hc4067_set_channel(mux, i, 1); // ENABLED
            sleep_ms(250);
        }
        // Disconnect the SIG pin from any channel
        mux74hc4067_disable(mux);
        sleep_ms(500);
    }
    return 0;
}
