//taken from https://www.appelsiini.net/2011/simple-usart-with-avr-libc/
#ifndef UARTLIB_H
#define UARTLIB_H


#include <avr/io.h>
#include <stdio.h>

int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

void uart_init(void);

FILE open_uart_ouput();
FILE open_uart_input();

#endif