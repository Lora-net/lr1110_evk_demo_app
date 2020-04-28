/**
 * @file      system_lptim.c
 *
 * @brief     MCU low-power timer related functions
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

#include "system_lptim.h"
#include "stm32l4xx_ll_bus.h"

void system_lptim_init( )
{
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_LPTIM1 );

    NVIC_SetPriority( LPTIM1_IRQn, 0 );
    NVIC_EnableIRQ( LPTIM1_IRQn );

    LL_LPTIM_SetClockSource( LPTIM1, LL_LPTIM_CLK_SOURCE_INTERNAL );
    LL_LPTIM_SetPrescaler( LPTIM1, LL_LPTIM_PRESCALER_DIV16 );
    LL_LPTIM_SetPolarity( LPTIM1, LL_LPTIM_OUTPUT_POLARITY_REGULAR );
    LL_LPTIM_SetUpdateMode( LPTIM1, LL_LPTIM_UPDATE_MODE_IMMEDIATE );
    LL_LPTIM_SetCounterMode( LPTIM1, LL_LPTIM_COUNTER_MODE_INTERNAL );
    LL_LPTIM_TrigSw( LPTIM1 );
    LL_LPTIM_SetInput1Src( LPTIM1, LL_LPTIM_INPUT1_SRC_GPIO );
    LL_LPTIM_SetInput2Src( LPTIM1, LL_LPTIM_INPUT2_SRC_GPIO );

    LL_LPTIM_Enable( LPTIM1 );

    while( LL_LPTIM_IsEnabled( LPTIM1 ) != 1 )
    {
    }

    LL_LPTIM_ClearFLAG_ARRM( LPTIM1 );
    LL_LPTIM_EnableIT_ARRM( LPTIM1 );
}

void system_lptim_set_and_run( uint32_t ticks )
{
    LL_LPTIM_SetAutoReload( LPTIM1, ticks );
    LL_LPTIM_StartCounter( LPTIM1, LL_LPTIM_OPERATING_MODE_ONESHOT );
}