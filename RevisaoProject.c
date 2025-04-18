// #include <stdio.h>
// #include "pico/stdlib.h"

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A

// Definições ADC
#define ADC_CENTER           2048      // valor central (joystick solto)
#define ADC_MAX              4095      // resolução 12 bits (0-4095)

// Parâmetros do display SSD1306
#define SSD1306_WIDTH         128
#define SSD1306_HEIGHT         64
#define SQUARE_SIZE             8      // tamanho do quadrado que representa o joystick

#define ADC_DEADZONE 200

// volatile int border_style = 0;

void display_init(ssd1306_t *ssd)
{
  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line

  ssd1306_init(ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(ssd); // Configura o display
  ssd1306_send_data(ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(ssd, false);
  ssd1306_send_data(ssd);
}

void adc_init_config() 
{
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
}

//Trecho para modo BOOTSEL com botão B
// #include "pico/bootrom.h"
// #define botaoB 6
// void gpio_irq_handler(uint gpio, uint32_t events)
// {
//   reset_usb_boot(0, 0);
// }

int main()
{
  ssd1306_t ssd; // Inicializa a estrutura do display

  // // Para ser utilizado o modo BOOTSEL com botão B
  // gpio_init(botaoB);
  // gpio_set_dir(botaoB, GPIO_IN);
  // gpio_pull_up(botaoB);
  // gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  // //Aqui termina o trecho para modo BOOTSEL com botão B

  // gpio_init(JOYSTICK_PB);
  // gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  // gpio_pull_up(JOYSTICK_PB); 

  // gpio_init(Botao_A);
  // gpio_set_dir(Botao_A, GPIO_IN);
  // gpio_pull_up(Botao_A);

  display_init(&ssd);

  adc_init_config();

  // uint16_t adc_value_x;
  // uint16_t adc_value_y;
  
  bool cor = true;
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
      // uint32_t duty_red = 0, duty_blue = 0;
      // if (diff_x > ADC_DEADZONE) {
      //     duty_red = ((diff_x - ADC_DEADZONE) * 4095) / (ADC_CENTER - ADC_DEADZONE);
      // }
      // if (diff_y > ADC_DEADZONE) {
      //     duty_blue = ((diff_y - ADC_DEADZONE) * 4095) / (ADC_CENTER - ADC_DEADZONE);
      // }
 
      // if (pwm_enabled) {
      //     pwm_set_gpio_level(PIN_LED_RED, duty_red);
      //     pwm_set_gpio_level(PIN_LED_BLUE, duty_blue);
      // }
      // else {
      //     // Se os PWM estiverem desativados, força o valor 0
      //     pwm_set_gpio_level(PIN_LED_RED, 0);
      //     pwm_set_gpio_level(PIN_LED_BLUE, 0);
      // }
 
      // --- Cálculo da posição do quadrado no display ---
      // Mapeia os valores ADC para a posição dentro da área útil do display
      uint8_t square_x = (adc_x * (SSD1306_WIDTH - SQUARE_SIZE)) / ADC_MAX;
      uint8_t square_y = ((ADC_MAX - adc_y) * (SSD1306_HEIGHT - SQUARE_SIZE)) / ADC_MAX;

      // --- Atualização do display SSD1306 ---
      // Limpa o buffer do display
      ssd1306_fill(&ssd, false);

      // Desenha a borda de acordo com o estilo atual (alternado pelo botão do joystick)
      // if (border_style == 0) {
      // // Estilo 0: borda completa (retângulo vazio)
      //     ssd1306_rect(&ssd, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, 1, 0);
      // }
      // else if (border_style == 1) {
      //     ssd1306_rect(&ssd, 0, 0, SSD1306_WIDTH, SSD1306_HEIGHT, 1, 0);
      //     ssd1306_rect(&ssd, 1, 1, SSD1306_WIDTH-2, SSD1306_HEIGHT-2, 1, 0);
      //     ssd1306_rect(&ssd, 2, 2, SSD1306_WIDTH-4, SSD1306_HEIGHT-4, 1, 0);
      // }
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
