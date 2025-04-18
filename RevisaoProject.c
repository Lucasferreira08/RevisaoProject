#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "ssd1306.h"
#include "font.h"
#include "led_matrix.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick

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

// Configuração do pino do buzzer
#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

static uint pwm_slice_num;
static uint pwm_channel;

// Variáveis globais para controle de tempo e estado
static volatile uint32_t last_time = 0; // Armazena o último tempo de interrupção
static volatile uint a = 0; // Variável de estado que será incrementada/decrementada pelos botões

// Variáveis para o PIO (Programmable I/O) e state machine (máquina de estados)
PIO pio;
uint sm;

// Função de interrupção com debouncing aprimorado
void pwm_init_buzzer(uint pin)
{
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    pwm_slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_channel = pwm_gpio_to_channel(BUZZER_PIN);
    
    pwm_config config = pwm_get_default_config();
    pwm_init(pwm_slice_num, &config, false); // Inicia desligado
}

int64_t buzzer_stop_alarm_callback(alarm_id_t id, void *user_data) {
    pwm_set_enabled(pwm_slice_num, false);
    return 0;
}

// Definição de uma função para emitir um beep com duração especificada
void beep(int frequency) {
    if (frequency <= 0) {
        pwm_set_enabled(pwm_slice_num, false);
        return;
    }

    uint32_t sys_clk = clock_get_hz(clk_sys); // Obtém clock do sistema (125 MHz padrão)
    uint32_t div = 1;
    uint32_t wrap;

    // Calcula div e wrap para melhor resolução
    while ((sys_clk / (div * frequency)) > 65535 && div < 256) div++;
    wrap = (sys_clk / (div * frequency)) - 1;

    pwm_set_clkdiv_int_frac(pwm_slice_num, div, 0);
    pwm_set_wrap(pwm_slice_num, wrap);
    pwm_set_chan_level(pwm_slice_num, pwm_channel, wrap * 0.75f); // 75% duty cycle para volume máximo
    pwm_set_enabled(pwm_slice_num, true);

    add_alarm_in_ms(50, buzzer_stop_alarm_callback, NULL, false);
}

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

        // Exibe o valor atual de 'a' no console
        printf("A = %d\n", a);

        // Atualiza o último tempo de interrupção
        last_time = current_time;

        // Chama a função de animação principal com o valor atual de 'a'
        main_animacao(a, pio, sm);
    }
}

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

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);

    // Habilita a interrupção para os botões A e B na borda de descida (quando o botão é pressionado)
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
}

void adc_init_config() 
{
  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
}

// Função para configurar o PIO (Programmable I/O)
uint pio_config(PIO pio) 
{
    set_sys_clock_khz(128000, false); // Configura o clock do sistema para 128 MHz

    // Adiciona o programa PIO para controle da matriz de LEDs
    uint offset = pio_add_program(pio, &pio_matrix_program);

    // Obtém uma state machine (máquina de estados) não utilizada
    uint sm = pio_claim_unused_sm(pio, true);

    // Inicializa o programa PIO na state machine com o pino de saída definido
    pio_matrix_program_init(pio, sm, offset, OUT_PIN);

    return sm; // Retorna a state machine configurada
}

int main()
{
  pio = pio0;

  ssd1306_t ssd; // Inicializa a estrutura do display

  display_init(&ssd);

  adc_init_config();

  pinos_config();
  sm = pio_config(pio);

  main_animacao(a, pio, sm);

  while (true)
  {
      adc_select_input(0);
      uint16_t adc_y = adc_read();
 
      adc_select_input(1);
      uint16_t adc_x = adc_read();
 
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
