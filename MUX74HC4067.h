#ifndef MUX74HC4067_H_
#define MUX74HC4067_H_

#include <pico/stdlib.h>

// Pin modes
#define MODE_INPUT          0
#define MODE_INPUT_PULLUP   1
#define MODE_OUTPUT         2

// Signal types
#ifndef ANALOG
#define ANALOG 0
#endif

#ifndef DIGITAL
#define DIGITAL 1
#endif

// Not implemented: DIGITAL_PULSE 2

#ifndef DISABLED
#define DISABLED 0
#endif

#ifndef ENABLED
#define ENABLED 1
#endif

typedef struct {
    uint8_t enable_pin;
    uint8_t enable_status;
    uint8_t signal_pin;
    uint8_t signal_mode;
    uint8_t signal_pin_status;
    uint8_t current_channel;
    uint8_t num_of_control_pins;
    uint8_t control_pin[4];
    uint8_t previousSteadyState;
    uint8_t lastSteadyState;
    uint8_t lastFlickerableState;
    unsigned long debounceTime;
    unsigned long lastDebounceTime;
    int8_t adc_channel;
} mux74hc4067_t;

#ifdef __cplusplus
extern "C" {
#endif

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
mux74hc4067_t* mux74hc4067_create(uint8_t en, int8_t s0, int8_t s1, int8_t s2, int8_t s3);

/**
 * Sets the current channel of the multiplexer and enables its connection with the SIG pin by default.
 *
 * @param mux The MUX74HC4067 instance to set the channel for.
 * @param pin The channel to set.
 * @param set Whether to enable the connection with the SIG pin (1) or not (0).
 */
void mux74hc4067_set_channel(mux74hc4067_t* mux, int8_t pin, uint8_t set);

/**
 * Enables the connection of the SIG pin with the currently set channel.
 *
 * @param mux The MUX74HC4067 instance to enable the connection for.
 */
void mux74hc4067_enable(mux74hc4067_t* mux);

/**
 * Disables the connection of the SIG pin with the currently set channel.
 *
 * @param mux The MUX74HC4067 instance to disable the connection for.
 */
void mux74hc4067_disable(mux74hc4067_t* mux);

/**
 * Configures the signal pin of the multiplexer.
 *
 * @param mux The MUX74HC4067 instance to configure the signal pin for.
 * @param sig The signal pin to configure.
 * @param mode The mode to set the signal pin to (MODE_INPUT, MODE_INPUT_PULLUP, or MODE_OUTPUT).
 * @param type The type of the signal pin (ANALOG or DIGITAL).
 * @param time The debounce time for the signal pin.
 */
void mux74hc4067_signal_pin(mux74hc4067_t* mux, uint8_t sig, uint8_t mode, uint8_t type, unsigned long time);

/**
 * Reads data from the requested channel of the multiplexer.
 *
 * @param mux The MUX74HC4067 instance to read data from.
 * @param chan_pin The channel to read data from.
 *
 * @return The data read from the channel.
 */
int16_t mux74hc4067_read(mux74hc4067_t* mux, int8_t chan_pin);

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
int8_t mux74hc4067_write(mux74hc4067_t* mux, int8_t chan_pin, uint8_t data, int8_t type);

/**
 * Checks the timing of the signal pin and updates its state accordingly.
 *
 * @param mux The MUX74HC4067 instance to check the timing for.
 */
void mux74hc4067_check_timing(mux74hc4067_t* mux);

/**
 * Checks if a button connected to the multiplexer is released.
 *
 * @param mux The MUX74HC4067 instance to check the button state for.
 * @param chan_pin The channel to check the button state for.
 *
 * @return True if the button is released, false otherwise.
 */
bool mux74hc4067_is_released(mux74hc4067_t* mux, int8_t chan_pin);

/**
 * Checks if a button connected to the multiplexer is pressed.
 *
 * @param mux The MUX74HC4067 instance to check the button state for.
 * @param chan_pin The channel to check the button state for.
 *
 * @return True if the button is pressed, false otherwise.
 */
bool mux74hc4067_is_pressed(mux74hc4067_t* mux, int8_t chan_pin);

/**
 * Destroys a MUX74HC4067 instance and frees its resources.
 *
 * @param mux The MUX74HC4067 instance to destroy.
 */
void mux74hc4067_destroy(mux74hc4067_t* mux);

#ifdef __cplusplus
}
#endif

#endif  // MUX74HC4067_H_

