// Configuração do pino do buzzer
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100

void pwm_init_buzzer();
void beep();