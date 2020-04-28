/**
 * @file      system_it.c
 *
 * @brief     MCU interrupt-related functions
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

/* Includes ------------------------------------------------------------------*/
#include "system_it.h"
#include "stm32l4xx_ll_dma.h"
#include "configuration.h"
#include <stdbool.h>
#include "system_time.h"
#include "system_lptim.h"
#include "system_uart.h"

extern void SupervisorInterruptHandlerGui( bool is_down );
extern void SupervisorInterruptHandlerDemo( void );
extern void TimerHasElapsed( void );
extern void lv_tick_inc( uint32_t );

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler( void ) {}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler( void )
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler( void )
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler( void )
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler( void )
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while( 1 )
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler( void ) {}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler( void ) {}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler( void ) {}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler( void )
{
    system_time_IncreaseTicker( );
    lv_tick_inc( 1 );
}

/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (EXTI), for the */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles external lines 10 to 15 interrupt request.
 * @param  None
 * @retval None
 */
extern void DemoBaseInterruptHandler( void );

void EXTI4_IRQHandler( void )
{
    LL_EXTI_ClearFlag_0_31( LL_EXTI_LINE_4 );
    SupervisorInterruptHandlerDemo( );
}

/**
 * @brief  This function handles external lines 10 to 15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI15_10_IRQHandler( void )
{
    if( LL_EXTI_IsActiveFlag_0_31( LL_EXTI_LINE_10 ) )
    {
        bool is_down = false;

        LL_EXTI_ClearFlag_0_31( LL_EXTI_LINE_10 );
        is_down = !LL_GPIO_IsInputPinSet( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN );
        SupervisorInterruptHandlerGui( is_down );
    }
}

/**
 * @brief  This function handles DMA1 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel7_IRQHandler( void )
{
    if( LL_DMA_IsActiveFlag_TC7( DMA1 ) )
    {
        LL_DMA_ClearFlag_GI7( DMA1 );
        /* Call function Transmission complete Callback */
        system_uart_dma_tx_complete_callback( );
    }
    else if( LL_DMA_IsActiveFlag_TE7( DMA1 ) )
    {
        /* Call Error function */
        system_uart_dma_txrx_error( );
    }
}

/**
 * @brief  This function handles DMA1 interrupt request.
 * @param  None
 * @retval None
 */
void DMA1_Channel6_IRQHandler( void )
{
    if( LL_DMA_IsActiveFlag_TC6( DMA1 ) )
    {
        LL_DMA_ClearFlag_GI6( DMA1 );
        /* Call function Reception complete Callback */
        system_uart_dma_rx_complete_callback( );
    }
    else if( LL_DMA_IsActiveFlag_TE6( DMA1 ) )
    {
        /* Call Error function */
        system_uart_dma_txrx_error( );
    }
}

/**
 * @brief This function handles LPTIM1 global interrupt.
 */
void LPTIM1_IRQHandler( void )
{
    /* Check whether Autoreload match interrupt is pending */
    if( LL_LPTIM_IsActiveFlag_ARRM( LPTIM1 ) == 1 )
    {
        /* Clear the Autoreload match interrupt flag */
        LL_LPTIM_ClearFLAG_ARRM( LPTIM1 );

        TimerHasElapsed( );
    }
}
