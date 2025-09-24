# 🔘🟢🔵🔴🕹💾 SD Card atuando com Pico W (BitDogLab)
![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Raspberry%20Pi%20Pico-purple.svg)
![Sensor](https://img.shields.io/badge/SD-Card-blue.svg)

## Sobre o Projeto
Este projeto tem como objetivo desenvolver um firmware capaz de controlar e visualizar LEDs por meio do joystick e dos botões A e B, quando os botões são pressionados simultaneamente, o sistema aciona o buzzer. Além disso, o firmware realiza a captura de sons via microfone, armazenando os dados em um cartão SD e todos os eventos podem ser monitorados em tempo real através do display OLED.


## 🛠️ Estrutura do projeto
- sd_card.c – Programa principal em C que faz leitura dos movimentos dos botões e joystick
- Pasta inc - Onde esta localizada as informações da oled
- Pasta lib - Onde está localizada as bibliotecas do SD Card
- CMakeLists.txt – Configuração do build usando o Pico SDK

- ## 🔌 Requisitos
Hardware:

- Raspberry Pi Pico W
- SD Card
- LEDs
- Buzzer
- Joystick
- Microfone
- Temperatura interna

## ⚙️ Como usar
1- Clone o repositorio

2- Deploy no Pico W
 - Segure o botão BOOTSEL do Pico W e conecte-o ao PC
 - Clique no botão run no Vscode ou arraste o arquivo .u2 para dentro do disco removível que aparecer
 - O Pico irá reiniciar executando o firmware
   
## 🔧 Funcionamento do Código
O programa realiza as seguintes ações:

## PINAGEM
- LED_VERMELHO 13
- LED_VERDE    11
- LED_AZUL     12
- BUZZER       21
- BOTAO_A      5
- BOTAO_B      6
- JOY_X        26
- JOY_Y        27
- MIC_ADC      28
- PIN_MISO     16
- PIN_MOSI     19
- PIN_SCK      18
- PIN_CS       17
  
1. Inicialização dos periféricos
- Gpio: 16 | 19 | 18 | 17 | para o sd card
- Configura os pinos dos LEDs RGB, buzzer, joystick, microfone e botões

2. Leitura de componentes
A cada segundo, o sistema:

   * 🟢 Verde: botão B pressionado o LED VERDE será acionado
   * 🔴 Vermelho: Botão A pressionado o LED VERMELHO será acionado
   * 🔵 Azul: Movimentando o joystick o LED AZUL será acionado
   * 📢 Buzzer: Apertando os dois botões ao mesmo tempo o Buzzer será acionado
 
 
4. SD Card
-  A cada presença detectada é gravado no cartão SD o evento: movimento jostick, botão a ativado ou botão b ativado, em caso dos botões acionados ao mesmo tempo o evento gravado será o buzzer ativado, também é armazenado no cartão a temperatura interna da placa e sons detectados.

## 📦 Dependências

ssd1306.h e ssd1306_fonts.h para o display OLED

