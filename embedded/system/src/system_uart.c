/**
 * @file      system_uart.c
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

#include "system_uart.h"
#include "stm32l4xx_ll_dma.h"
#include "callback.h"

#ifndef NULL
#define NULL ( 0 )
#endif

volatile static bool TxOnGoing = false;
volatile static bool RxDone    = false;

static Callback_t RxDoneCallback;
static Callback_t TxDoneCallback;

static void system_uart_dma_configure_tx( uint8_t* buffer, uint32_t buffer_size );
static void system_uart_dma_configure_rx( uint8_t* buffer, uint32_t buffer_size );

void system_uart_init( )
{
    LL_USART_InitTypeDef USART_InitStruct = { 0 };

    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_USART2 );

    LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOA );
    /**USART2 GPIO Configuration
    PA2   ------> USART2_TX
    PA3   ------> USART2_RX
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_2 | LL_GPIO_PIN_3;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_7;
    LL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    USART_InitStruct.BaudRate            = 921600;
    USART_InitStruct.DataWidth           = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits            = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity              = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection   = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling        = LL_USART_OVERSAMPLING_16;
    LL_USART_Init( USART2, &USART_InitStruct );
    LL_USART_ConfigAsyncMode( USART2 );
    LL_USART_Enable( USART2 );

    while( LL_USART_IsEnabled( USART2 ) == 0 )
        ;
}

int32_t system_uart_send_char( int32_t ch )
{
    while( !LL_USART_IsActiveFlag_TXE( USART2 ) )
        ;
    LL_USART_TransmitData8( USART2, ch & 0xFF );

    return ch;
}

void system_uart_start_receiving( void )
{
    LL_USART_ClearFlag_ORE( USART2 );
    LL_USART_EnableDirectionRx( USART2 );
}

void system_uart_stop_receiving( void ) { LL_USART_DisableDirectionRx( USART2 ); }

int32_t system_uart_receive_char( void )
{
    int32_t ret = -1;

    while( !LL_USART_IsActiveFlag_RXNE( USART2 ) )
        ;
    ret = LL_USART_ReceiveData8( USART2 );

    return ret;
}

uint8_t system_uart_is_readable( void ) { return LL_USART_IsActiveFlag_RXNE( USART2 ); }

/*
 * == Field Test related commands == *
 */

void system_uart_dma_init( void )
{
    /* DMA1 used for USART2 Transmission and Reception
     */
    /* (1) Enable the clock of DMA1 */
    LL_AHB1_GRP1_EnableClock( LL_AHB1_GRP1_PERIPH_DMA1 );

    /* (2) Configure NVIC for DMA transfer complete/error interrupts */
    NVIC_SetPriority( DMA1_Channel7_IRQn, 0 );
    NVIC_EnableIRQ( DMA1_Channel7_IRQn );
    NVIC_SetPriority( DMA1_Channel6_IRQn, 0 );
    NVIC_EnableIRQ( DMA1_Channel6_IRQn );

    /* (3) Configure the DMA functional parameters for transmission */
    LL_DMA_ConfigTransfer( DMA1, LL_DMA_CHANNEL_7,
                           LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
                               LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE |
                               LL_DMA_MDATAALIGN_BYTE );
    LL_DMA_SetPeriphRequest( DMA1, LL_DMA_CHANNEL_7, LL_DMA_REQUEST_2 );

    /* (4) Configure the DMA functional parameters for reception */
    LL_DMA_ConfigTransfer( DMA1, LL_DMA_CHANNEL_6,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_PRIORITY_HIGH | LL_DMA_MODE_NORMAL |
                               LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE |
                               LL_DMA_MDATAALIGN_BYTE );
    LL_DMA_SetPeriphRequest( DMA1, LL_DMA_CHANNEL_6, LL_DMA_REQUEST_2 );

    /* (5) Enable DMA transfer complete/error interrupts  */
    LL_DMA_EnableIT_TC( DMA1, LL_DMA_CHANNEL_7 );
    LL_DMA_EnableIT_TE( DMA1, LL_DMA_CHANNEL_7 );
    LL_DMA_EnableIT_TC( DMA1, LL_DMA_CHANNEL_6 );
    LL_DMA_EnableIT_TE( DMA1, LL_DMA_CHANNEL_6 );
}

void system_uart_dma_deinit( void )
{
    NVIC_DisableIRQ( DMA1_Channel7_IRQn );
    NVIC_DisableIRQ( DMA1_Channel6_IRQn );
    LL_DMA_DeInit( DMA1, LL_DMA_CHANNEL_6 );
    LL_DMA_DeInit( DMA1, LL_DMA_CHANNEL_7 );
    LL_DMA_DisableIT_TC( DMA1, LL_DMA_CHANNEL_7 );
    LL_DMA_DisableIT_TE( DMA1, LL_DMA_CHANNEL_7 );
    LL_DMA_DisableIT_TC( DMA1, LL_DMA_CHANNEL_6 );
    LL_DMA_DisableIT_TE( DMA1, LL_DMA_CHANNEL_6 );
}

void system_uart_dma_configure_tx( uint8_t* buffer, uint32_t buffer_size )
{
    LL_DMA_ConfigAddresses( DMA1, LL_DMA_CHANNEL_7, ( uint32_t ) buffer,
                            LL_USART_DMA_GetRegAddr( USART2, LL_USART_DMA_REG_DATA_TRANSMIT ),
                            LL_DMA_GetDataTransferDirection( DMA1, LL_DMA_CHANNEL_7 ) );
    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_7, buffer_size );
}

void system_uart_dma_configure_rx( uint8_t* buffer, uint32_t buffer_size )
{
    LL_DMA_ConfigAddresses( DMA1, LL_DMA_CHANNEL_6, LL_USART_DMA_GetRegAddr( USART2, LL_USART_DMA_REG_DATA_RECEIVE ),
                            ( uint32_t ) buffer, LL_DMA_GetDataTransferDirection( DMA1, LL_DMA_CHANNEL_6 ) );
    LL_DMA_SetDataLength( DMA1, LL_DMA_CHANNEL_6, buffer_size );
}

bool system_uart_send_buffer( uint8_t* data, uint16_t size )
{
    __disable_irq( );
    bool is_sending = false;
    if( TxOnGoing == false )
    {
        TxOnGoing = true;
        system_uart_dma_configure_tx( data, size );
        LL_USART_EnableDMAReq_TX( USART2 );
        LL_DMA_EnableChannel( DMA1, LL_DMA_CHANNEL_7 );
        is_sending = true;
    }
    else
    {
        is_sending = false;
    }
    __enable_irq( );
    return is_sending;
}

void system_uart_start_buffer_reception( const uint16_t size, uint8_t* rx_buffer )
{
    RxDone = false;
    system_uart_dma_configure_rx( rx_buffer, size );
    LL_USART_EnableDMAReq_RX( USART2 );
    LL_DMA_EnableChannel( DMA1, LL_DMA_CHANNEL_6 );
    // HAL_UART_Receive_DMA( &huart2, rx_buffer, size );
}

void system_uart_register_tx_done_callback( void* object, void ( *callback )( void* ) )
{
    TxDoneCallback.object   = object;
    TxDoneCallback.callback = callback;
}

void system_uart_register_rx_done_callback( void* object, void ( *callback )( void* ) )
{
    RxDoneCallback.object   = object;
    RxDoneCallback.callback = callback;
}

void system_uart_unregister_rx_done_callback( void )
{
    RxDoneCallback.object   = 0;
    RxDoneCallback.callback = 0;
}

void system_uart_unregister_tx_done_callback( void )
{
    TxDoneCallback.object   = 0;
    TxDoneCallback.callback = 0;
}

bool system_uart_is_tx_terminated( void ) { return ( TxOnGoing == false ) && LL_USART_IsActiveFlag_TC( USART2 ); }

void system_uart_reset( void )
{
    system_uart_dma_deinit( );
    LL_DMA_DisableChannel( DMA1, LL_DMA_CHANNEL_7 );
    LL_USART_DisableDMAReq_RX( USART2 );
    TxOnGoing = false;
    RxDone    = false;
    system_uart_dma_init( );
    // HAL_UART_Abort( &huart2 );
}

void system_uart_dma_tx_complete_callback( void )
{
    TxOnGoing = false;
    LL_DMA_DisableChannel( DMA1, LL_DMA_CHANNEL_7 );
    if( TxDoneCallback.object != NULL && TxDoneCallback.callback != NULL )
    {
        TxDoneCallback.callback( TxDoneCallback.object );
    }
}

void system_uart_dma_rx_complete_callback( void )
{
    RxDone = true;
    LL_DMA_DisableChannel( DMA1, LL_DMA_CHANNEL_6 );
    if( RxDoneCallback.object != NULL && RxDoneCallback.callback != NULL )
    {
        RxDoneCallback.callback( RxDoneCallback.object );
    }
}

void system_uart_dma_txrx_error( void ) {}

void system_uart_flush( void ) { LL_USART_RequestRxDataFlush( USART2 ); }
