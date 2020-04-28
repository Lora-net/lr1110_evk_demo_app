/**
 * @file      system_gpio.c
 *
 * @brief     MCU GPIO-related functions
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

#include "system_gpio.h"

#include "stm32l476xx.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_gpio.h"

static void system_gpio_init_input( GPIO_TypeDef* port, uint32_t pin, system_gpio_interrupt_t interrupt )
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    if( port == GPIOA )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOA );
    }
    else if( port == GPIOB )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOB );
    }
    else if( port == GPIOC )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOC );
    }
    else if( port == GPIOD )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOD );
    }

    GPIO_InitStruct.Pin  = pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init( port, &GPIO_InitStruct );

    if( interrupt != SYSTEM_GPIO_NO_INTERRUPT )
    {
        LL_EXTI_InitTypeDef EXTI_InitStruct = { 0 };

        LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_SYSCFG );
        LL_SYSCFG_SetEXTISource( LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE4 );

        EXTI_InitStruct.Line_0_31   = pin;
        EXTI_InitStruct.Line_32_63  = LL_EXTI_LINE_NONE;
        EXTI_InitStruct.LineCommand = ENABLE;
        EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
        switch( interrupt )
        {
        case SYSTEM_GPIO_RISING:
            EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
            break;
        case SYSTEM_GPIO_FALLING:
            EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
            break;
        case SYSTEM_GPIO_BOTH:
            EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
            break;
        default:
            break;
        }
        LL_EXTI_Init( &EXTI_InitStruct );

        if( pin == LL_GPIO_PIN_0 )
        {
            NVIC_EnableIRQ( EXTI0_IRQn );
            NVIC_SetPriority( EXTI0_IRQn, 0 );
        }
        else if( pin == LL_GPIO_PIN_1 )
        {
            NVIC_EnableIRQ( EXTI1_IRQn );
            NVIC_SetPriority( EXTI1_IRQn, 0 );
        }
        else if( pin == LL_GPIO_PIN_2 )
        {
            NVIC_EnableIRQ( EXTI2_IRQn );
            NVIC_SetPriority( EXTI2_IRQn, 0 );
        }
        else if( pin == LL_GPIO_PIN_3 )
        {
            NVIC_EnableIRQ( EXTI3_IRQn );
            NVIC_SetPriority( EXTI3_IRQn, 0 );
        }
        else if( pin == LL_GPIO_PIN_4 )
        {
            NVIC_EnableIRQ( EXTI4_IRQn );
            NVIC_SetPriority( EXTI4_IRQn, 0 );
        }
        else if( ( pin >= LL_GPIO_PIN_5 ) && ( pin <= LL_GPIO_PIN_9 ) )
        {
            NVIC_EnableIRQ( EXTI9_5_IRQn );
            NVIC_SetPriority( EXTI9_5_IRQn, 0 );
        }
        else if( ( pin >= LL_GPIO_PIN_10 ) && ( pin <= LL_GPIO_PIN_15 ) )
        {
            NVIC_EnableIRQ( EXTI15_10_IRQn );
            NVIC_SetPriority( EXTI15_10_IRQn, 0 );
        }
    }
}

static void system_gpio_init_output( GPIO_TypeDef* port, uint32_t pin, uint8_t initialState )
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    if( port == GPIOA )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOA );
    }
    else if( port == GPIOB )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOB );
    }
    else if( port == GPIOC )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOC );
    }
    else if( port == GPIOD )
    {
        LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOD );
    }

    if( initialState == 1 )
    {
        LL_GPIO_SetOutputPin( port, pin );
    }
    else
    {
        LL_GPIO_ResetOutputPin( port, pin );
    }

    GPIO_InitStruct.Pin        = pin;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_NO;
    LL_GPIO_Init( port, &GPIO_InitStruct );
}

void system_gpio_init( void )
{
    system_gpio_init_output( LR1110_LED_SCAN_PORT, LR1110_LED_SCAN_PIN, 0 );
    system_gpio_init_output( LR1110_LED_TX_PORT, LR1110_LED_TX_PIN, 0 );
    system_gpio_init_output( LR1110_LED_RX_PORT, LR1110_LED_RX_PIN, 0 );

    system_gpio_init_output( LR1110_RESET_PORT, LR1110_RESET_PIN, 1 );
    system_gpio_init_output( LR1110_NSS_PORT, LR1110_NSS_PIN, 1 );
    system_gpio_init_input( LR1110_IRQ_PORT, LR1110_IRQ_PIN, SYSTEM_GPIO_RISING );
    system_gpio_init_input( LR1110_BUSY_PORT, LR1110_BUSY_PIN, SYSTEM_GPIO_NO_INTERRUPT );

    system_gpio_init_output( DISPLAY_NSS_PORT, DISPLAY_NSS_PIN, 1 );
    system_gpio_init_output( DISPLAY_DC_PORT, DISPLAY_DC_PIN, 0 );

    system_gpio_init_input( TOUCH_IRQ_PORT, TOUCH_IRQ_PIN, SYSTEM_GPIO_BOTH );

    system_gpio_init_output( LR1110_LNA_PORT, LR1110_LNA_PIN, 1 );

    system_gpio_init_input( BUTTON_BLUE_PORT, BUTTON_BLUE_PIN, SYSTEM_GPIO_NO_INTERRUPT );

    // Initialize the NSS of unused flash device that may interact on the SPI
    // bus
    system_gpio_init_output( FLASH_NSS_PORT, FLASH_NSS_PIN, 1 );
}

void system_gpio_init_direction_state( const gpio_t gpio, const system_gpio_pin_direction_t direction,
                                       const system_gpio_pin_state_t state )
{
    switch( direction )
    {
    case SYSTEM_GPIO_PIN_DIRECTION_INPUT:
        system_gpio_init_input( gpio.port, gpio.pin, SYSTEM_GPIO_NO_INTERRUPT );
        break;
    case SYSTEM_GPIO_PIN_DIRECTION_OUTPUT:
        system_gpio_init_output( gpio.port, gpio.pin, state );
        break;
    default:
        break;
    }
}

void system_gpio_set_pin_state( gpio_t gpio, const system_gpio_pin_state_t state )
{
    switch( state )
    {
    case SYSTEM_GPIO_PIN_STATE_LOW:
        LL_GPIO_ResetOutputPin( gpio.port, gpio.pin );
        break;
    case SYSTEM_GPIO_PIN_STATE_HIGH:
        LL_GPIO_SetOutputPin( gpio.port, gpio.pin );
        break;
    default:
        break;
    }
}

system_gpio_pin_state_t system_gpio_get_pin_state( gpio_t gpio )
{
    if( LL_GPIO_IsInputPinSet( gpio.port, gpio.pin ) )
    {
        return SYSTEM_GPIO_PIN_STATE_HIGH;
    }
    else
    {
        return SYSTEM_GPIO_PIN_STATE_LOW;
    }
}

void system_gpio_wait_for_state( gpio_t gpio, system_gpio_pin_state_t state )
{
    if( state == SYSTEM_GPIO_PIN_STATE_LOW )
    {
        while( LL_GPIO_IsInputPinSet( gpio.port, gpio.pin ) )
        {
        };
    }
    else
    {
        while( !LL_GPIO_IsInputPinSet( gpio.port, gpio.pin ) )
        {
        };
    }
}
