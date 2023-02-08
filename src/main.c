#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "trace.h"

/* AVR GPIO helper macros */
// clang-format off
#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_SET_BIT(type, port, bit) (type##port |= (1 << bit))
#define GPIO_CLEAR_BIT(type, port, bit) (type##port &= ~(1 << bit))
#define GPIO_GET_BIT(type, port, bit) ((type##port & (1 << bit)) ? 1 : 0)
#define GPIO_MODE(pin, mode) do {if(mode) GPIO_SET_BIT(DDR, pin); else GPIO_CLEAR_BIT(DDR, pin);} while(0)
#define GPIO_SET(pin, value) do {if(value) GPIO_SET_BIT(PORT, pin); else GPIO_CLEAR_BIT(PORT, pin);} while(0)
#define GPIO_GET(pin) (GPIO_GET_BIT(PIN, pin))
// clang-format on

#define VALVE_0 B, 0
#define VALVE_1 B, 1
#define VALVE_2 D, 2
#define VALVE_3 D, 3
#define VALVE_4 D, 4
#define VALVE_5 D, 5
#define VALVE_6 D, 6
#define VALVE_7 D, 7

#define ALL_VALVES_OFF 8

#define VBAT_ADC_CHANNEL 0

// clang-format off
static const uint8_t slots[16] = {
    0b10100000,
    0b01010000,
    0b10001000,
    0b01000100,
    0b10100000,
    0b01010000,
    0b10000010,
    0b01000001,
         
    0b10100000,
    0b01010000,
    0b10001000,
    0b01000100,
    0b10100000,
    0b01010000,
    0b10000000,
    0b01000000,
};
// clang-format on

void activate_valve(uint8_t valve) {
    GPIO_MODE(VALVE_0, GPIO_OUTPUT);
    GPIO_MODE(VALVE_1, GPIO_OUTPUT);
    GPIO_MODE(VALVE_2, GPIO_OUTPUT);
    GPIO_MODE(VALVE_3, GPIO_OUTPUT);
    GPIO_MODE(VALVE_4, GPIO_OUTPUT);
    GPIO_MODE(VALVE_5, GPIO_OUTPUT);
    GPIO_MODE(VALVE_6, GPIO_OUTPUT);
    GPIO_MODE(VALVE_7, GPIO_OUTPUT);

    GPIO_SET(VALVE_0, valve == 0);
    GPIO_SET(VALVE_1, valve == 1);
    GPIO_SET(VALVE_2, valve == 2);
    GPIO_SET(VALVE_3, valve == 3);
    GPIO_SET(VALVE_4, valve == 4);
    GPIO_SET(VALVE_5, valve == 5);
    GPIO_SET(VALVE_6, valve == 6);
    GPIO_SET(VALVE_7, valve == 7);
}

/* AVR adc helper function*/
// set ADC prescaler for 50-200 KHz
#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
#define ADC_ADPS 7
#elif F_CPU >= 8000000 // 8 MHz / 64 = 125 KHz
#define ADC_ADPS 6
#elif F_CPU >= 4000000 // 4 MHz / 32 = 125 KHz
#define ADC_ADPS 5
#elif F_CPU >= 2000000 // 2 MHz / 16 = 125 KHz
#define ADC_ADPS 4
#elif F_CPU >= 1000000 // 1 MHz / 8 = 125 KHz
#define ADC_ADPS 3
#else // 128 kHz / 2 = 64 KHz -> This is the closest you can get, the prescaler is 2
#define ADC_ADPS 1
#endif

uint16_t adc_get(uint8_t channel) {
    ADCSRA = (1 << ADEN) | ADC_ADPS;
    ADMUX = (1 << REFS0) | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC))
        ;
    return ADC;
}

int main(void) {
    uint8_t minute = 0;
    uint32_t vbat_filter = 0;
    uint32_t vbat_mv = 0;

    // trace_init();
    activate_valve(ALL_VALVES_OFF);

    while (1) {

        // 1 minute (60 seconds) delay between minute changes
        for (uint16_t i = 0; i < 60; i++) {

            for (uint16_t i = 0; i < 100; i++) {
                _delay_ms(10); // 1 second delay
            }

            vbat_filter -= vbat_filter >> 4UL;
            vbat_filter += (uint32_t)adc_get(VBAT_ADC_CHANNEL);
            vbat_mv = ((vbat_filter >> 4UL) * 7104L) >> 8;

            // trace_write((uint8_t[]){(uint8_t)(vbat_mv / 100), (uint8_t)(vbat_mv % 100)}, 2);
            // vbat_mv = ((uint32_t)adc_get(VBAT_ADC_CHANNEL) * 7104UL) / 256UL;
            // if (vbat_mv > 13400UL) {
            //     activate_valve(0);
            // } else {
            //     activate_valve(1);
            // }
        }

        if (vbat_mv > 13400UL) {
            minute += 1;
        } else {
            minute = 0;
        }

        uint8_t slot = (minute >> 4);
        uint8_t valve = (minute >> 1) & 0x07;

        if (slots[slot] & (1 << valve)) {
            activate_valve(valve);

        } else {
            activate_valve(ALL_VALVES_OFF);
        }
    }
    return 0;
}