#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#include "trace.h"

#define XSTR(x) #x
#define STR(s) XSTR(s)

#define PORT_DDR(port) DDR##port
#define PORT_PORT(port) PORT##port

#define VALVE_PORT D

#define VALVE_4 4
#define VALVE_5 5
#define VALVE_6 6
#define VALVE_7 7

#define SET_PIN_MODE_OUTPUT(port, pin) PORT_DDR(port) |= (1 << pin)
#define SET_PIN_MODE_INPUT(port, pin) PORT_DDR(port) &= ~(1 << pin)
#define SET_PIN_HIGH(port, pin) PORT_PORT(port) |= (1 << pin)
#define SET_PIN_LOW(port, pin) PORT_PORT(port) &= ~(1 << pin)

int main(void) {

    SET_PIN_MODE_OUTPUT(VALVE_PORT, VALVE_4);
    SET_PIN_MODE_OUTPUT(VALVE_PORT, VALVE_5);
    SET_PIN_MODE_OUTPUT(VALVE_PORT, VALVE_6);
    SET_PIN_MODE_OUTPUT(VALVE_PORT, VALVE_7);

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
            SET_PIN_HIGH(VALVE_PORT, VALVE_4);
            SET_PIN_LOW(VALVE_PORT, VALVE_5);
            SET_PIN_LOW(VALVE_PORT, VALVE_6);
            SET_PIN_LOW(VALVE_PORT, VALVE_7);
            break;
        case 1:
            SET_PIN_LOW(VALVE_PORT, VALVE_4);
            SET_PIN_HIGH(VALVE_PORT, VALVE_5);
            SET_PIN_LOW(VALVE_PORT, VALVE_6);
            SET_PIN_LOW(VALVE_PORT, VALVE_7);
            break;
        case 2:
            SET_PIN_LOW(VALVE_PORT, VALVE_4);
            SET_PIN_LOW(VALVE_PORT, VALVE_5);
            SET_PIN_HIGH(VALVE_PORT, VALVE_6);
            SET_PIN_LOW(VALVE_PORT, VALVE_7);
            break;
        case 3:
            SET_PIN_LOW(VALVE_PORT, VALVE_4);
            SET_PIN_LOW(VALVE_PORT, VALVE_5);
            SET_PIN_LOW(VALVE_PORT, VALVE_6);
            SET_PIN_HIGH(VALVE_PORT, VALVE_7);
            break;
        default:
            SET_PIN_LOW(VALVE_PORT, VALVE_4);
            SET_PIN_LOW(VALVE_PORT, VALVE_5);
            SET_PIN_LOW(VALVE_PORT, VALVE_6);
            SET_PIN_LOW(VALVE_PORT, VALVE_7);
            break;
        }

        trace_write((uint8_t[]){seconds, phase}, 2);
    }
    return 0;
}