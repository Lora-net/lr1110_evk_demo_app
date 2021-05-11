/**
 * @file      system_i2c.c
 *
 * @brief     MCU I2C-related functions
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

#include "system_i2c.h"

void system_i2c_init( void )
{
    LL_I2C_InitTypeDef I2C_InitStruct = { 0 };

    LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    LL_AHB2_GRP1_EnableClock( LL_AHB2_GRP1_PERIPH_GPIOB );
    /**I2C1 GPIO Configuration
    PB8   ------> I2C1_SCL
    PB9   ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin        = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode       = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed      = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull       = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate  = LL_GPIO_AF_4;
    LL_GPIO_Init( GPIOB, &GPIO_InitStruct );

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_I2C1 );

    /**I2C Initialization
     */
    LL_I2C_EnableAutoEndMode( I2C1 );
    LL_I2C_DisableOwnAddress2( I2C1 );
    LL_I2C_DisableGeneralCall( I2C1 );
    LL_I2C_EnableClockStretching( I2C1 );
    I2C_InitStruct.PeripheralMode  = LL_I2C_MODE_I2C;
    I2C_InitStruct.Timing          = 0x10909CEC;
    I2C_InitStruct.AnalogFilter    = LL_I2C_ANALOGFILTER_ENABLE;
    I2C_InitStruct.DigitalFilter   = 0;
    I2C_InitStruct.OwnAddress1     = 0;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
    I2C_InitStruct.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;
    LL_I2C_Init( I2C1, &I2C_InitStruct );
    LL_I2C_SetOwnAddress2( I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK );
}

void system_i2c_write( const uint8_t address, const uint8_t* buffer_in, const uint8_t length, const bool repeated )
{
    uint8_t i = 0;

    LL_I2C_HandleTransfer( I2C1, address, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND,
                           LL_I2C_GENERATE_START_WRITE );

    /* Loop until STOP flag is raised  */
    while( !LL_I2C_IsActiveFlag_STOP( I2C1 ) )
    {
        if( LL_I2C_IsActiveFlag_TXIS( I2C1 ) )
        {
            LL_I2C_TransmitData8( I2C1, buffer_in[i++] );
        }
    }

    LL_I2C_ClearFlag_STOP( I2C1 );
}

void system_i2c_read( const uint8_t address, uint8_t* buffer_out, const uint8_t length, const bool repeated )
{
    uint8_t i = 0;

    LL_I2C_HandleTransfer( I2C1, address, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND,
                           LL_I2C_GENERATE_START_READ );

    /* Loop until STOP flag is raised  */
    while( !LL_I2C_IsActiveFlag_STOP( I2C1 ) )
    {
        if( LL_I2C_IsActiveFlag_RXNE( I2C1 ) )
        {
            buffer_out[i++] = LL_I2C_ReceiveData8( I2C1 );
        }
    }

    LL_I2C_ClearFlag_STOP( I2C1 );
}
