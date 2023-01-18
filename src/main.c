#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

#define TINBUS_BUFFER_BITS (6)
#define TINBUS_BUFFER_SIZE (1 << TINBUS_BUFFER_BITS)
#define TINBUS_BUFFER_MASK (TINBUS_BUFFER_SIZE - 1)

static volatile uint8_t trace_data[TINBUS_BUFFER_SIZE] __attribute__((aligned(TINBUS_BUFFER_SIZE)));
static volatile uint8_t trace_head = 0;
static volatile uint8_t trace_tail = 0;

ISR(SPI_STC_vect) {
    if (trace_head != trace_tail) {
        SPDR = trace_data[trace_tail];
        trace_data[trace_tail] = SPDR;
        trace_tail += 1;
        trace_tail &= TINBUS_BUFFER_MASK;
    } else {
        SPCR &= ~(1 << SPIE);
    }
}

void spi_init(void) {
    DDRB |= (1 << PORTB2) | (1 << PORTB3) | (1 << PORTB5);
    PORTB |= (1 << PORTB3);

    SPCR = (1 << MSTR) | (1 << SPE) | (1 << SPR0) | (1 << SPR1) | (1 << CPHA) | (0 << CPOL);
}

void spi_write(uint8_t *data, uint8_t size) {
    cli();
    trace_data[trace_head] = 0xFE;
    trace_head += 1;
    trace_head &= TINBUS_BUFFER_MASK;
    for (uint8_t index = 0; index < size; index++) {
        trace_data[trace_head] = data[index];
        trace_head += 1;
        trace_head &= TINBUS_BUFFER_MASK;
        data[index] = trace_data[trace_head];
    }

    SPDR = 0xFF;
    SPCR |= (1 << SPIE);

    // if (SPCR & (1 << SPIE) == 0) {
    // SPCR |= (1 << SPIE);
    // SPDR = trace_data[trace_tail];
    // trace_tail += 1;
    // trace_tail &= TINBUS_BUFFER_MASK;
    // }
    sei();
}

int main(void) {

    DDRD |= (1 << PD7); // LED on pin PB2

    // spi_init();

    while (1) {
        // spi_write((uint8_t[]){0xa5, 0x3C, 0x5a}, 3);
        for (uint8_t i = 0; i < 5; i++) {
            PORTD &= ~(1 << PD7); // output off
        }

        PORTD |= (1 << PD7);  // output on
        PORTD &= ~(1 << PD7); // output off
    }

    return 0;
}