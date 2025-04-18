#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "pio_matrix.pio.h"

// Initializes the keypad
uint32_t matrix_rgb(double b, double r, double g);

// Checks if a specific key is pressed
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b);

// Main loop to monitor keypad and run animations
void main_animacao(uint num, PIO pio, uint sm);

#endif // LED_ANIMATION_H