#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "trace.h"

/* AVR GPIO helper macros */
#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_SET_BIT(type, port, bit) (type##port |= (1 << bit))
#define GPIO_CLEAR_BIT(type, port, bit) (type##port &= ~(1 << bit))
#define GPIO_GET_BIT(type, port, bit) ((type##port & (1 << bit)) ? 1 : 0)
#define GPIO_SET(pin, value) (value ? GPIO_SET_BIT(PORT, pin) : GPIO_CLEAR_BIT(PORT, pin))
#define GPIO_GET(pin) (GPIO_GET_BIT(PIN, pin))
#define GPIO_MODE(pin, mode) (mode ? GPIO_SET_BIT(DDR, pin) : GPIO_CLEAR_BIT(DDR, pin))

#define VALVE_4 D, 4
#define VALVE_5 D, 5
#define VALVE_6 D, 6
#define VALVE_7 D, 7

int main(void) {
    GPIO_MODE(VALVE_4, GPIO_OUTPUT);
    GPIO_MODE(VALVE_5, GPIO_OUTPUT);
    GPIO_MODE(VALVE_6, GPIO_OUTPUT);
    GPIO_MODE(VALVE_7, GPIO_OUTPUT);
    trace_init();
    uint8_t seconds = 0;
    while (1) {
        // 4 second delay
        for (uint8_t i = 0; i < 200; i++) {
            _delay_ms(20);
        }
        seconds++;
        uint8_t phase = seconds >> 5;
        switch (phase) {
        case 0:
            GPIO_SET(VALVE_4, 1);
            GPIO_SET(VALVE_5, 0);
            GPIO_SET(VALVE_6, 0);
            GPIO_SET(VALVE_7, 0);
            break;
        case 1:
            GPIO_SET(VALVE_4, 0);
            GPIO_SET(VALVE_5, 1);
            GPIO_SET(VALVE_6, 0);
            GPIO_SET(VALVE_7, 0);
            break;
        case 2:
            GPIO_SET(VALVE_4, 0);
            GPIO_SET(VALVE_5, 0);
            GPIO_SET(VALVE_6, 1);
            GPIO_SET(VALVE_7, 0);
            break;
        case 3:
            GPIO_SET(VALVE_4, 0);
            GPIO_SET(VALVE_5, 0);
            GPIO_SET(VALVE_6, 0);
            GPIO_SET(VALVE_7, 1);
            break;
        default:
            GPIO_SET(VALVE_4, 0);
            GPIO_SET(VALVE_5, 0);
            GPIO_SET(VALVE_6, 0);
            GPIO_SET(VALVE_7, 0);
            break;
        }
        trace_write((uint8_t[]){seconds, phase}, 2);
    }
    return 0;
}