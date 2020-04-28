/**
 * @file      system_uart.h
 *
 * @brief     MCU UART-related functions header file
 *
 * Revised BSD License
 * Copyright Semtech Corporation 2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _SYSTEM_UART_H
#define _SYSTEM_UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "stm32l476xx.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_usart.h"

void    system_uart_init( void );
int32_t system_uart_send_char( int32_t ch );
void    system_uart_start_receiving( void );
void    system_uart_stop_receiving( void );
int32_t system_uart_receive_char( void );
uint8_t system_uart_is_readable( void );
void    system_uart_flush( void );

void system_uart_dma_init( void );
void system_uart_dma_deinit( void );
bool system_uart_send_buffer( uint8_t* data, uint16_t size );
bool system_uart_is_tx_terminated( void );
void system_uart_start_buffer_reception( const uint16_t size, uint8_t* rx_buffer );
void system_uart_register_rx_done_callback( void* object, void ( *callback )( void* ) );
void system_uart_register_tx_done_callback( void* object, void ( *callback )( void* ) );
void system_uart_unregister_rx_done_callback( void );
void system_uart_unregister_tx_done_callback( void );
void system_uart_reset( void );
void system_uart_dma_tx_complete_callback( void );
void system_uart_dma_rx_complete_callback( void );
void system_uart_dma_txrx_error( void );

#ifdef __cplusplus
}
#endif

#endif
