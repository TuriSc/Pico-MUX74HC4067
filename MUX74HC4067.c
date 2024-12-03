#include <stdlib.h>
#include <pico/stdlib.h>
#include "MUX74HC4067.h"
#include "hardware/adc.h"

/**
 * Creates a new instance of the MUX74HC4067 multiplexer.
 *
 * @param en The enable pin of the multiplexer.
 * @param s0 The first control pin of the multiplexer.
 * @param s1 The second control pin of the multiplexer. -1 if not used.
 * @param s2 The third control pin of the multiplexer. -1 if not used.
 * @param s3 The fourth control pin of the multiplexer. -1 if not used.
 *
 * @return A pointer to the newly created MUX74HC4067 instance.
 */
mux74hc4067_t* mux74hc4067_create(uint8_t en, int8_t s0, int8_t s1, int8_t s2, int8_t s3) {
    mux74hc4067_t* mux = malloc(sizeof(mux74hc4067_t));
    mux->enable_pin = en;
    gpio_init(en);
    gpio_set_dir(en, GPIO_OUT);
    gpio_put(en, 1);  // Initially disables the connection of the SIG pin to the channels
    mux->enable_status = 0; // DISABLED

    mux->signal_pin_status = -1;
    mux->signal_pin = -1;
    mux->signal_mode = MODE_INPUT;
    mux->current_channel = 0;
    mux->num_of_control_pins = 1;
    mux->control_pin[0] = s0;

    mux->num_of_control_pins += ((mux->control_pin[1] = s1) != -1) ? 1 : 0;
    mux->num_of_control_pins += ((mux->control_pin[2] = s2) != -1) ? 1 : 0;
    mux->num_of_control_pins += ((mux->control_pin[3] = s3) != -1) ? 1 : 0;

    for (uint8_t i = 0; i < mux->num_of_control_pins; ++i) {
        gpio_init(mux->control_pin[i]);
        gpio_set_dir(mux->control_pin[i], GPIO_OUT);
    }

    return mux;
}

/**
 * Sets the current channel of the multiplexer and enables its connection with the SIG pin by default.
 *
 * @param mux The MUX74HC4067 instance to set the channel for.
 * @param pin The channel to set.
 * @param set Whether to enable the connection with the SIG pin (1) or not (0).
 */
void mux74hc4067_set_channel(mux74hc4067_t* mux, int8_t pin, uint8_t set) {
    gpio_put(mux->enable_pin, 1);
    mux->current_channel = pin;

    for (uint8_t i = 0; i < mux->num_of_control_pins; ++i) {
        gpio_put(mux->control_pin[i], pin & 0x01);
        pin >>= 1;
    }

    mux->enable_status = 1; // ENABLED

    if (set == 1) { // ENABLED
        gpio_put(mux->enable_pin, 0);
    }
}

/**
 * Enables the connection of the SIG pin with the currently set channel.
 *
 * @param mux The MUX74HC4067 instance to enable the connection for.
 */
void mux74hc4067_enable(mux74hc4067_t* mux) {
    mux->enable_status = 1; // ENABLED
    gpio_put(mux->enable_pin, 0);
}

/**
 * Disables the connection of the SIG pin with the currently set channel.
 *
 * @param mux The MUX74HC4067 instance to disable the connection for.
 */
void mux74hc4067_disable(mux74hc4067_t* mux) {
    mux->enable_status = 0; // DISABLED
    gpio_put(mux->enable_pin, 1);
}

/**
 * Configures the signal pin of the multiplexer.
 *
 * @param mux The MUX74HC4067 instance to configure the signal pin for.
 * @param sig The signal pin to configure.
 * @param mode The mode to set the signal pin to (MODE_INPUT, MODE_INPUT_PULLUP, or MODE_OUTPUT).
 * @param type The type of the signal pin (ANALOG or DIGITAL).
 * @param time The debounce time for the signal pin.
 */
void mux74hc4067_signal_pin(mux74hc4067_t* mux, uint8_t sig, uint8_t mode, uint8_t type, unsigned long time) {
    mux->signal_pin = sig;
    mux->signal_mode = mode;
    gpio_init(sig);

    if (mode == MODE_INPUT || mode == MODE_INPUT_PULLUP) {
        if (mode == MODE_INPUT_PULLUP) {
            gpio_set_dir(sig, GPIO_IN);
            gpio_pull_up(sig);
        } else { // MODE_INPUT
            gpio_put(sig, 0); // Disable pullup
            gpio_set_dir(sig, GPIO_IN);
        }

        mux->previousSteadyState = gpio_get(sig);
        mux->lastSteadyState = mux->previousSteadyState;
        mux->lastFlickerableState = mux->previousSteadyState;

        mux->debounceTime = time;
        mux->lastDebounceTime = 0;
    } else if (mode == MODE_OUTPUT) {
        gpio_set_dir(sig, GPIO_OUT);
    }

    if (type == ANALOG) {
        int8_t adc_channel;
        switch(sig) {
            case 28:
            adc_channel = 2;
            break;
            case 27:
            adc_channel = 1;
            break;
            case 26:
            adc_channel = 0;
            break;
            default:
            adc_channel = -1;
            break;
        }
        mux->adc_channel = adc_channel;
        mux->signal_pin_status = 0;
    } else if (type == DIGITAL) {
        mux->signal_pin_status = 1;
    }
    // TODO: implement (type == 2), and equivalent of pulseIn()
}

/**
 * Reads data from the requested channel of the multiplexer.
 *
 * @param mux The MUX74HC4067 instance to read data from.
 * @param chan_pin The channel to read data from.
 *
 * @return The data read from the channel.
 */
int16_t mux74hc4067_read(mux74hc4067_t* mux, int8_t chan_pin) {
    int16_t data;
    uint8_t last_channel;
    uint8_t last_en_status = mux->enable_status;

    if (chan_pin != -1) {
        last_channel = mux->current_channel;
        mux74hc4067_set_channel(mux, chan_pin, 1); // ENABLED
    }

    if (mux->signal_pin_status == 0) {
        adc_select_input(mux->adc_channel);
        data = adc_read();
    } else if (mux->signal_pin_status == 1) {
        data = gpio_get(mux->signal_pin);
    } else {
        data = -1;
    }

    if (chan_pin != -1) {
        mux74hc4067_set_channel(mux, last_channel, last_en_status);
    }

    return data;
}

/**
 * Writes data to the requested channel of the multiplexer.
 *
 * @param mux The MUX74HC4067 instance to write data to.
 * @param chan_pin The channel to write data to.
 * @param data The data to write to the channel.
 * @param type The type of the data (ANALOG or DIGITAL).
 * @note Only DIGITAL is currently supported.
 *
 * @return 0 on success, -1 on failure.
 */
int8_t mux74hc4067_write(mux74hc4067_t* mux, int8_t chan_pin, uint8_t data, int8_t type) {
    if (mux->signal_mode == MODE_INPUT || mux->signal_mode == MODE_INPUT_PULLUP) {
        return -1;
    }

    mux74hc4067_disable(mux);

    if (type == ANALOG) {
        // Not supported
        // TODO: implement PWM analog write
    } else if (type == DIGITAL) {
        gpio_put(mux->signal_pin, data);
    } else {
        if (mux->signal_pin_status == 0) {
        // Not supported, analog write
        } else if (mux->signal_pin_status == 1) {
            gpio_put(mux->signal_pin, data);
        }
    }

    mux74hc4067_set_channel(mux, chan_pin, 1); // ENABLED

    return 0;
}

/**
 * Checks the timing of the signal pin and updates its state accordingly.
 *
 * @param mux The MUX74HC4067 instance to check the timing for.
 */
void mux74hc4067_check_timing(mux74hc4067_t* mux) {
    if (mux->signal_pin_status == 1) {
        // read the state of the switch/button:
        int currentState = gpio_get(mux->signal_pin);

        unsigned long currentTime = get_absolute_time();

        // check to see if you just pressed the button
        // (i.e. the input went from 0 to 1), and you've waited long enough
        // since the last press to ignore any noise:

        // If the switch/button changed, due to noise or pressing:
        if (currentState != mux->lastFlickerableState) {
            // reset the debouncing timer
            mux->lastDebounceTime = currentTime;
            // save the the last flickerable state
            mux->lastFlickerableState = currentState;
        }

        if ((currentTime - mux->lastDebounceTime) >= mux->debounceTime) {
            // whatever the reading is at, it's been there for longer than the debounce
            // delay, so take it as the actual current state:

            // save the the steady state
            mux->previousSteadyState = mux->lastSteadyState;
            mux->lastSteadyState = currentState;
        }
    }
}

/**
 * Checks if a button connected to the multiplexer is released.
 *
 * @param mux The MUX74HC4067 instance to check the button state for.
 * @param chan_pin The channel to check the button state for.
 *
 * @return True if the button is released, false otherwise.
 */
bool mux74hc4067_is_released(mux74hc4067_t* mux, int8_t chan_pin) {
    bool data = false;
    uint8_t last_channel;
    uint8_t last_en_status = mux->enable_status;

    if (chan_pin != -1) {
        last_channel = mux->current_channel;
        mux74hc4067_set_channel(mux, chan_pin, 1); // ENABLED
    }

    if (mux->signal_pin_status == 1) {
        data = (mux->previousSteadyState == 0 && mux->lastSteadyState == 1);
    }

    return data;
}

/**
 * Checks if a button connected to the multiplexer is pressed.
 *
 * @param mux The MUX74HC4067 instance to check the button state for.
 * @param chan_pin The channel to check the button state for.
 *
 * @return True if the button is pressed, false otherwise.
 */
bool mux74hc4067_is_pressed(mux74hc4067_t* mux, int8_t chan_pin) {
    bool data = false;
    uint8_t last_channel;
    uint8_t last_en_status = mux->enable_status;

    if (chan_pin != -1) {
        last_channel = mux->current_channel;
        mux74hc4067_set_channel(mux, chan_pin, 1); // ENABLED
    }

    if (mux->signal_pin_status == 1) {
        data = (mux->previousSteadyState == 1 && mux->lastSteadyState == 0);
    }

    return data;
}

