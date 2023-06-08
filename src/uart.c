
// taken from https://www.appelsiini.net/2011/simple-usart-with-avr-libc/

#include <avr/io.h>
#include <stdio.h>
#include <uart.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef BAUD
#define BAUD 9600
#endif

#include <util/setbaud.h>

void uart_init(void)
{
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
    {
        uart_putchar('\r', stream);
    }
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return 0;
}

int uart_getchar(FILE *stream)
{
    (void)(stream);
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

FILE open_uart_ouput()
{
    FILE f;
    memset(&f, 0, sizeof f);

    f.put = uart_putchar;
    f.flags = _FDEV_SETUP_WRITE;
    return f;
}

FILE open_uart_input()
{
    FILE f;
    memset(&f, 0, sizeof f);

    f.get = uart_getchar;
    f.flags = _FDEV_SETUP_READ;
    return f;
}