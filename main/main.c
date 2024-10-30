#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// Configurações de pinos
#define MIC_PIN 26         // Entrada ADC (conectada ao microfone)
#define SPEAKER_PIN 15     // Saída PWM (conectada ao filtro e speaker)
#define BUTTON_PIN 14      // Entrada digital (botão)

// Configurações do sistema
#define SAMPLE_RATE 44100  // Taxa de amostragem em Hz

// Inicializa o PWM
void init_pwm(uint slice_num) {
    pwm_set_clkdiv(slice_num, 1.0f); // Define o divisor de clock para ajustar a frequência do PWM
    pwm_set_wrap(slice_num, 255); // Define o ciclo do PWM
    pwm_set_gpio_level(SPEAKER_PIN, 0);
    pwm_set_enabled(slice_num, true);
}

// Função principal
int main() {
    // Inicializa GPIO
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Inicializa ADC e seleciona o pino do microfone
    adc_init();
    adc_gpio_init(MIC_PIN);
    adc_select_input(0);  // Escolhe o canal do ADC para MIC_PIN

    // Inicializa PWM
    gpio_set_function(SPEAKER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SPEAKER_PIN);
    init_pwm(slice_num);

    // Configuração de temporização
    const uint64_t delay = 1000000 / SAMPLE_RATE;

    while (true) {
        // Verifica se o botão está pressionado
        if (gpio_get(BUTTON_PIN) == 0) {
            // Captura o sinal de áudio do microfone
            uint16_t adc_value = adc_read();

            // Converte o valor ADC para PWM (escala de 8 bits)
            uint8_t pwm_value = adc_value >> 4;
            pwm_set_gpio_level(SPEAKER_PIN, pwm_value);

            // Atraso para manter a taxa de amostragem
            sleep_us(delay);
        } else {
            // Se o botão não estiver pressionado, para o PWM
            pwm_set_gpio_level(SPEAKER_PIN, 0);
        }
    }
}
