#include "led_matrix.h"

// Número de LEDs na matriz (5x5)
#define NUM_PIXELS 25

// Pino de saída para controlar a matriz de LEDs
#define OUT_PIN 7

// Vetores para criar imagens na matriz de LEDs
// Cada vetor representa uma matriz 5x5, onde os valores são intensidades dos LEDs (0.0 a 1.0)
double desenho0[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         1.0, 0.0, 0.0, 0.0, 1.0, 
                         1.0, 0.0, 0.0, 0.0, 1.0,
                         1.0, 0.0, 0.0, 0.0, 1.0,
                         0.0, 1.0, 1.0, 1.0, 0.0};

double desenho1[25] =   {0.0, 0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 1.0, 0.0, 
                         1.0, 0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, 0.0,
                         0.0, 0.0, 1.0, 0.0, 0.0};

double desenho2[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         1.0, 0.0, 0.0, 0.0, 1.0, 
                         0.0, 0.0, 1.0, 1.0, 0.0,
                         0.0, 0.0, 0.0, 1.0, 0.0,
                         1.0, 1.0, 1.0, 1.0, 1.0};

double desenho3[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         1.0, 0.0, 0.0, 0.0, 0.0, 
                         0.0, 1.0, 1.0, 1.0, 0.0,
                         1.0, 0.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 1.0, 1.0, 0.0};

double desenho4[25] =   {0.0, 1.0, 0.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 1.0, 0.0, 
                         0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 1.0, 0.0};

double desenho5[25] =   {0.0, 1.0, 1.0, 1.0, 1.0,
                         0.0, 0.0, 0.0, 0.0, 1.0, 
                         0.0, 1.0, 1.0, 1.0, 0.0,
                         1.0, 0.0, 0.0, 0.0, 0.0,
                         1.0, 1.0, 1.0, 1.0, 0.0};

double desenho6[25] =   {1.0, 1.0, 1.0, 1.0, 1.0,
                         0.0, 0.0, 0.0, 0.0, 1.0, 
                         1.0, 1.0, 1.0, 1.0, 1.0,
                         1.0, 0.0, 0.0, 0.0, 1.0,
                         1.0, 1.0, 1.0, 1.0, 1.0};

double desenho7[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 0.0, 0.0, 
                         0.0, 0.0, 0.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 0.0, 0.0,
                         0.0, 0.0, 0.0, 1.0, 0.0};

double desenho8[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 1.0, 0.0, 
                         0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 1.0, 0.0,
                         0.0, 1.0, 1.0, 1.0, 0.0};

double desenho9[25] =   {0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 1.0, 0.0, 
                         0.0, 1.0, 1.0, 1.0, 0.0,
                         0.0, 1.0, 0.0, 0.0, 0.0,
                         0.0, 1.0, 1.0, 1.0, 0.0};

// Rotina para definir a intensidade das cores do LED (RGB)
uint32_t matrix_rgb(double b, double r, double g)
{
  unsigned char R, G, B;
  R = r * 255; // Converte a intensidade de vermelho para 8 bits
  G = g * 255; // Converte a intensidade de verde para 8 bits
  B = b * 255; // Converte a intensidade de azul para 8 bits
  return (G << 24) | (R << 16) | (B << 8); // Retorna o valor RGB no formato 32 bits
}

// Função para desenhar uma imagem na matriz de LEDs usando PIO
void desenho_pio(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b){
    for (int16_t i = 0; i < NUM_PIXELS; i++) {
        // Define a cor do LED com base no vetor de desenho
        valor_led = matrix_rgb(b=0.0, r=0.0, desenho[24-i]); // Usa apenas o verde (g) para simplicidade
        pio_sm_put_blocking(pio, sm, valor_led); // Envia o valor para a matriz de LEDs via PIO
    }
}

// Função principal para exibir a animação na matriz de LEDs
void main_animacao(uint num, PIO pio, uint sm)
{
    uint32_t valor_led;
    double r = 0.0, b = 0.0 , g = 0.0; // Cores RGB (apenas verde é usado aqui)

    // Seleciona o desenho com base no número fornecido
    switch (num) {
        case 0:
            desenho_pio(desenho0, valor_led, pio, sm, r, g, b);
            break;
        case 1:
            desenho_pio(desenho1, valor_led, pio, sm, r, g, b);
            break;
        case 2:
            desenho_pio(desenho2, valor_led, pio, sm, r, g, b);
            break;
        case 3:
            desenho_pio(desenho3, valor_led, pio, sm, r, g, b);
            break;
        case 4:
            desenho_pio(desenho4, valor_led, pio, sm, r, g, b);
            break;
        case 5:
            desenho_pio(desenho5, valor_led, pio, sm, r, g, b);
            break;
        case 6:
            desenho_pio(desenho6, valor_led, pio, sm, r, g, b);
            break;
        case 7:
            desenho_pio(desenho7, valor_led, pio, sm, r, g, b);
            break;
        case 8:
            desenho_pio(desenho8, valor_led, pio, sm, r, g, b);
            break;
        case 9:
            desenho_pio(desenho9, valor_led, pio, sm, r, g, b);
            break;
    }
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