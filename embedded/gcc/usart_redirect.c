#include "system_uart.h"

int __io_putchar( int ch ) { return system_uart_send_char( ch ); }
int __io_getchar( void ) { return system_uart_receive_char( ); }
