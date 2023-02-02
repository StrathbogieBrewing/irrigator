#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

// #include "trace.h"

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

static const uint8_t slots[16] = {
    0b10010010,
    0b01000100,
    0b10100000,
    0b01000000,
    0b10010000,
    0b01001000,
    0b10100000,
    0b01000000,

    0b10010001,
    0b01000100,
    0b10100000,
    0b01000000,
    0b10010000,
    0b01001000,
    0b10100000,
    0b01000000,
};
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

int main(void) {

    // trace_init();
    uint8_t minute = 0;
    while (1) {

        // 1 minute (60 seconds) delay between minute changes
        for (uint16_t i = 0; i < (6 * 1); i++) {
            _delay_ms(10);
        }

        uint8_t slot = (minute >> 4);
        uint8_t valve = (minute >> 1) & 0x07;

        if(slots[slot] & (1 << valve)) {
            activate_valve(valve);
        } else {
            activate_valve(ALL_VALVES_OFF);
        }

        minute += 1;

        // trace_write((uint8_t[]){seconds, minute}, 2);
    }
    return 0;
}