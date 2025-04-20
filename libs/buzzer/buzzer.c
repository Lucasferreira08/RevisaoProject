#include "buzzer.h"

// Função de interrupção com debouncing aprimorado
void pwm_init_buzzer()
{
    // Configurar o pino como saída de PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

int64_t buzzer_stop_alarm_callback(alarm_id_t id, void *user_data) {
    pwm_set_gpio_level(BUZZER_PIN, 0);

    return 0;
}

// Definição de uma função para emitir um beep com duração especificada
void beep() {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(BUZZER_PIN, 2048);

    // Temporização
    add_alarm_in_ms(500, buzzer_stop_alarm_callback, NULL, false);
}