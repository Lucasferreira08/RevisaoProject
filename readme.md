Este projeto integra periféricos da BitDogLab em um sistema interativo com as seguintes funcionalidades:

Contador na matriz de LEDs: controlado por botões A e B, com feedback sonoro via buzzer.
Display OLED: exibe um quadrado movido pelo joystick.
LED RGB: ajustado pelo joystick.
Comunicação UART: registra eventos.
Componentes: joystick (ADC), botões (interrupções e debounce), display OLED, matriz de LEDs, LED RGB (PWM) e buzzer (PWM e timer).

Uso: botões A e B ajustam o contador, o botão do joystick apaga a matriz, e o joystick move o quadrado e controla o LED RGB.