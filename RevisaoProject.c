#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "buzzer.h"
#include "ssd1306.h"
#include "font.h"
#include "led_matrix.h"

#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick

#define PIN_LED_BLUE         12    // LED Azul (PWM) – controlado pelo eixo Y do joystick
#define PIN_LED_RED          13    // LED Vermelho (PWM) – controlado pelo eixo X do joystick

#define ADC_DEADZONE 200           // Dead zone around joystick center (adjust as needed)

// Define os pinos dos botões A e B
#define BUTTON_A 5
#define BUTTON_B 6

#define OUT_PIN 7

// Definições ADC
#define ADC_CENTER           2048      // valor central (joystick solto)
#define ADC_MAX              4095      // resolução 12 bits (0-4095)

// Parâmetros do display SSD1306
#define SSD1306_WIDTH         128
#define SSD1306_HEIGHT         64
#define SQUARE_SIZE             8      // tamanho do quadrado que representa o joystick

// Variáveis globais para controle de tempo e estado
static volatile uint32_t last_time = 0; // Armazena o último tempo de interrupção
static volatile uint a = 0; // Variável de estado que será incrementada/decrementada pelos botões

// Variáveis para o PIO (Programmable I/O) e state machine (máquina de estados)
PIO pio;
uint sm;

// Função de interrupção com debouncing aprimorado
void gpio_irq_handler(uint gpio, uint32_t events)
{
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos

    // Verifica se passaram pelo menos 250ms desde a última interrupção (debouncing)
    if (current_time - last_time > 250000) // 250 ms para evitar pulos
    {
        // Se o botão A foi pressionado e 'a' é menor que 9, incrementa 'a'
        if (gpio == BUTTON_A && a < 9) {
            a++;
        } 
        else if (gpio == BUTTON_A && a == 9) 
        {
            beep(440);
        }
        // Se o botão B foi pressionado e 'a' é maior que 0, decrementa 'a'
        else if (gpio == BUTTON_B && a > 0) {
            a--;
        }
        else if (gpio == BUTTON_B && a == 0) 
        {
            beep(440);
        }
        else if (gpio == JOYSTICK_PB) 
        {
            main_animacao(-1, pio, sm);
            last_time = current_time;
            printf("Matriz de led desligada.\n");
            return;    
        }

        // Exibe o valor atual de 'a' no console
        printf("Valor exibido na matriz de led: %d\n", a);

        // Atualiza o último tempo de interrupção
        last_time = current_time;

        // Chama a função de animação principal com o valor atual de 'a'
        main_animacao(a, pio, sm);
    }
}

// Função para configurar os pinos
void pinos_config() 
{
    stdio_init_all(); // Inicializa a comunicação serial (para printf)

    // Configura o pino do botão A como entrada com pull-up
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    // Configura o pino do botão B como entrada com pull-up
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);

    // Configura o pino do botão B como entrada com pull-up
    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // LED Azul (PWM)
    gpio_init(PIN_LED_BLUE);
    gpio_set_dir(PIN_LED_BLUE, GPIO_OUT);
    // LED Vermelho (PWM)
    gpio_init(PIN_LED_RED);
    gpio_set_dir(PIN_LED_RED, GPIO_OUT);

    // Habilita a interrupção para os botões A e B na borda de descida (quando o botão é pressionado)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
}

void pwm_main_config() 
 {
     uint slice_blue = pwm_gpio_to_slice_num(PIN_LED_BLUE);
     uint channel_blue = pwm_gpio_to_channel(PIN_LED_BLUE);
     pwm_set_wrap(slice_blue, 4095);  // define valor de wrap (PERÍODO PWM)
     pwm_set_chan_level(slice_blue, channel_blue, 0);  // inicia com brilho 0
     pwm_set_enabled(slice_blue, true);
     gpio_set_function(PIN_LED_BLUE, GPIO_FUNC_PWM);
 
     uint slice_red = pwm_gpio_to_slice_num(PIN_LED_RED);
     uint channel_red = pwm_gpio_to_channel(PIN_LED_RED);
     pwm_set_wrap(slice_red, 4095);
     pwm_set_chan_level(slice_red, channel_red, 0);
     pwm_set_enabled(slice_red, true);
     gpio_set_function(PIN_LED_RED, GPIO_FUNC_PWM);
 }

int main()
{
  pio = pio0;
  ssd1306_t ssd; // Inicializa a estrutura do display

  pinos_config();
  display_init(&ssd);
  pwm_main_config();

  sm = pio_config(pio);

  main_animacao(a, pio, sm);

  while (true)
  {
      adc_select_input(0);
      uint16_t adc_y = adc_read();
 
      adc_select_input(1);
      uint16_t adc_x = adc_read();

      // --- Cálculo dos níveis PWM para os LEDs ---
      // A ideia é que, quando o joystick estiver no centro (2048), o LED fique apagado.
      // Quanto maior o desvio (seja para mais ou para menos), maior a intensidade.
      uint16_t diff_x = (adc_x > ADC_CENTER) ? (adc_x - ADC_CENTER) : (ADC_CENTER - adc_x);
      uint16_t diff_y = (adc_y > ADC_CENTER) ? (adc_y - ADC_CENTER) : (ADC_CENTER - adc_y);

      // Aplica dead zone e calcula duty cycle
      uint32_t duty_red = 0, duty_blue = 0;
      if (diff_x > ADC_DEADZONE) {
            duty_red = ((diff_x - ADC_DEADZONE) * 4095) / (ADC_CENTER - ADC_DEADZONE);
      }
      if (diff_y > ADC_DEADZONE) {
            duty_blue = ((diff_y - ADC_DEADZONE) * 4095) / (ADC_CENTER - ADC_DEADZONE);
      }

      pwm_set_gpio_level(PIN_LED_RED, duty_red);
      pwm_set_gpio_level(PIN_LED_BLUE, duty_blue);
 
      // --- Cálculo da posição do quadrado no display ---
      // Mapeia os valores ADC para a posição dentro da área útil do display
      uint8_t square_x = (adc_x * (SSD1306_WIDTH - SQUARE_SIZE)) / ADC_MAX;
      uint8_t square_y = ((ADC_MAX - adc_y) * (SSD1306_HEIGHT - SQUARE_SIZE)) / ADC_MAX;

      // --- Atualização do display SSD1306 ---
      // Limpa o buffer do display
      ssd1306_fill(&ssd, false);

      ssd1306_rect(&ssd, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, 1, 0);

      // Desenha o quadrado móvel representando a posição do joystick
      // Nota: A função ssd1306_rect espera (top, left, width, height),
      // portanto, square_y é o 'top' e square_x é o 'left'
      ssd1306_rect(&ssd, square_y, square_x, SQUARE_SIZE, SQUARE_SIZE, 1, 1);

      // Atualiza o display (envia o buffer via I2C)
      ssd1306_send_data(&ssd);

      // Pequeno atraso para não sobrecarregar a CPU (e ajustar taxa de atualização)
      sleep_ms(50);
  }
}
