// Configuração do pino do buzzer
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 21

static uint pwm_slice_num;
static uint pwm_channel;

void pwm_init_buzzer(uint pin);
void beep(int frequency);