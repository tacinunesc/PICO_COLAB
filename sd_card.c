#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "ff.h"
#include "diskio.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_fonts.h"

#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12
#define BUZZER       21
#define BOTAO_A      5
#define BOTAO_B      6
#define JOY_X        26
#define JOY_Y        27
#define MIC_ADC      28

#define PIN_MISO     16
#define PIN_MOSI     19
#define PIN_SCK      18
#define PIN_CS       17

#define SDA_I2C      8
#define SCL_I2C      9
#define I2C_PORT     i2c0

#define AMOSTRAS_AUDIO 16000
uint16_t buffer_audio[AMOSTRAS_AUDIO];

FATFS fs;
FIL file;

void inicializar_sd() {
    spi_init(spi0, 1000 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);

    FRESULT fr = f_mount(&fs, "", 1);
    if (fr != FR_OK) {
        printf("Erro ao montar SD: %d\n", fr);
    } else {
        printf("Cartão SD montado.\n");
        fr = f_open(&file, "bitdoglab.csv", FA_WRITE | FA_OPEN_APPEND);
        if (fr == FR_OK) {
            f_puts("Tipo,Valor,Timestamp\n", &file);
            f_sync(&file);
        }
    }
}

void registrar_evento(const char* evento) {
    char linha[64];
    snprintf(linha, sizeof(linha), "Evento,%s,%lu\n", evento, to_ms_since_boot(get_absolute_time()));
    UINT bw;
    if (f_write(&file, linha, strlen(linha), &bw) == FR_OK) f_sync(&file);
}

float ler_temperatura_interna() {
    adc_select_input(4);
    uint16_t leitura = adc_read();
    float voltagem = leitura * (3.3f / 4096);
    return 27.0f - (voltagem - 0.706f) / 0.001721f;
}

void registrar_temperatura() {
    float temp = ler_temperatura_interna();
    char linha[64];
    snprintf(linha, sizeof(linha), "Temperatura,%.2f,%lu\n", temp, to_ms_since_boot(get_absolute_time()));
    UINT bw;
    if (f_write(&file, linha, strlen(linha), &bw) == FR_OK) f_sync(&file);
}

void exibir_oled(const char* mensagem) {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("EVENTO DETECTADO", Font_6x8, White);
    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString((char*)mensagem, Font_6x8, White);
    ssd1306_UpdateScreen();
}

void exibir_temperatura_oled(float temp) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Temp interna: %.2f C", temp);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("MONITORAMENTO", Font_6x8, White);
    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString(buffer, Font_6x8, White);
    ssd1306_UpdateScreen();
}

void capturar_audio_raw() {
    adc_select_input(2);
    FIL audio_file;
    FRESULT fr = f_open(&audio_file, "audio3.raw", FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("Erro ao abrir audio.raw: %d\n", fr);
        return;
    }

    for (int i = 0; i < AMOSTRAS_AUDIO; i++) {
        buffer_audio[i] = adc_read();
        UINT bw;
        f_write(&audio_file, &buffer_audio[i], sizeof(uint16_t), &bw);
        sleep_us(62);
    }

    f_sync(&audio_file);
    f_close(&audio_file);
    registrar_evento("AUDIO CAPTURADO");
    exibir_oled("AUDIO SALVO NO SD");
}


void detectar_som() {
    adc_select_input(2);
    uint32_t soma = 0;
    uint16_t pico = 0;
    const int N = 100;

    for (int i = 0; i < N; i++) {
        uint16_t amostra = adc_read();
        soma += amostra;
        if (amostra > pico) pico = amostra;
        sleep_us(100);
    }

    uint16_t media = soma / N;

    if (pico > 3500 || media > 2500)

    {
        capturar_audio_raw();
        registrar_evento("SOM DETECTADO");
        exibir_oled("SOM DETECTADO");
        gpio_put(BUZZER, 1);
        sleep_ms(200);
        gpio_put(BUZZER, 0);
    }
}


int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);

    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_I2C, GPIO_FUNC_I2C);
    gpio_set_function(SCL_I2C, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_I2C);
    gpio_pull_up(SCL_I2C);
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);     gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(BUZZER);       gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_init(BOTAO_A); gpio_set_dir(BOTAO_A, GPIO_IN); gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B); gpio_set_dir(BOTAO_B, GPIO_IN); gpio_pull_up(BOTAO_B);

    adc_init();
    adc_gpio_init(JOY_X);
    adc_gpio_init(JOY_Y);
    adc_gpio_init(MIC_ADC);
    adc_select_input(4);

    inicializar_sd();

    uint32_t ultimo_registro = 0;

    while (true) {
        bool a_pressionado = !gpio_get(BOTAO_A);
        bool b_pressionado = !gpio_get(BOTAO_B);

        if (a_pressionado && b_pressionado) {
            gpio_put(BUZZER, 1);
            registrar_evento("BUZZER ATIVADO");
            exibir_oled("BUZZER ATIVADO");
            sleep_ms(300);
            gpio_put(BUZZER, 0);
        } else if (a_pressionado) {
            gpio_put(LED_VERMELHO, 1);
            registrar_evento("BOTAO A PRESSIONADO");
            exibir_oled("BOTAO A PRESSIONADO");
            sleep_ms(300);
            gpio_put(LED_VERMELHO, 0);
        } else if (b_pressionado) {
            gpio_put(LED_VERDE, 1);
            registrar_evento("BOTAO B PRESSIONADO");
            exibir_oled("BOTAO B PRESSIONADO");
            sleep_ms(300);
            gpio_put(LED_VERDE, 0);
        }

        adc_select_input(0); uint16_t x = adc_read();
        adc_select_input(1); uint16_t y = adc_read();

        if (x < 1000 || x > 3000 || y < 1000 || y > 3000) {
            gpio_put(LED_AZUL, 1);
            registrar_evento("JOYSTICK MOVIDO");
            exibir_oled("JOYSTICK MOVIDO");
            sleep_ms(300);
            gpio_put(LED_AZUL, 0);
        }

        detectar_som();

        uint32_t agora = to_ms_since_boot(get_absolute_time());
        if (agora - ultimo_registro > 5000) {
            float temp = ler_temperatura_interna();
            registrar_temperatura();
            exibir_temperatura_oled(temp);
            ultimo_registro = agora;
        }

        sleep_ms(200);
    }
}
