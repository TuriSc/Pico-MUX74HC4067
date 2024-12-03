# Pico MUX74HC4067

A C library for controlling the 74HC4067 multiplexer/demultiplexer on the Raspberry Pi Pico (RP2040) a nd Raspberry Pi Pico2 (RP2350). Multiple simultaneous muxers are supported.

This repository is a C version for Raspberry Pi Pico SDK of the MUX74HC4067 Arduino library, which was originally written by Nick Lamprianidis. The original library can be found at [nlamprian/MUX74HC4067/](https://github.com/nlamprian/MUX74HC4067/).

A notable difference from the original library is the lack of support for Arduino's AnalogWrite() function. Analog reading is available on GPIOs 26, 27, and 28.
The only other missing feature is the equivalent of Arduino's pulseIn() to measure an input pulse duration.


### Usage
An example application is provided.


### License
This repository is released under the MIT license, which is the same license as the original one.


### Version history
- 2024.12.03 - v1.0.0 - First release