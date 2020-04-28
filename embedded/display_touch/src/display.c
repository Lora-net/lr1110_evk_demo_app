/**
 * @file      display.c
 *
 * \brief
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

#include "configuration.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_utils.h"
#include "system.h"
#include "display.h"

void display_send_command( const uint8_t command )
{
    LL_GPIO_ResetOutputPin( DISPLAY_DC_PORT, DISPLAY_DC_PIN );
    system_spi_write( SPI1, &command, 1 );
    LL_GPIO_SetOutputPin( DISPLAY_DC_PORT, DISPLAY_DC_PIN );
}

void display_send_data_8bit( const uint8_t data ) { system_spi_write( SPI1, &data, 1 ); }

void display_send_data( const uint16_t data )
{
    uint8_t dh = data >> 8;
    uint8_t dl = data & 0xff;

    system_spi_write( SPI1, &dh, 1 );
    system_spi_write( SPI1, &dl, 1 );
}

void display_init( void )
{
    LL_GPIO_ResetOutputPin( DISPLAY_NSS_PORT, DISPLAY_NSS_PIN );

    // ILI9341 init
    display_send_command( 0x11 );

    display_send_command( 0xCF );
    display_send_data_8bit( 0x00 );
    display_send_data_8bit( 0xc3 );
    display_send_data_8bit( 0X30 );

    display_send_command( 0xED );
    display_send_data_8bit( 0x64 );
    display_send_data_8bit( 0x03 );
    display_send_data_8bit( 0X12 );
    display_send_data_8bit( 0X81 );

    display_send_command( 0xE8 );
    display_send_data_8bit( 0x85 );
    display_send_data_8bit( 0x10 );
    display_send_data_8bit( 0x79 );

    display_send_command( 0xCB );
    display_send_data_8bit( 0x39 );
    display_send_data_8bit( 0x2C );
    display_send_data_8bit( 0x00 );
    display_send_data_8bit( 0x34 );
    display_send_data_8bit( 0x02 );

    display_send_command( 0xF7 );
    display_send_data_8bit( 0x20 );

    display_send_command( 0xEA );
    display_send_data_8bit( 0x00 );
    display_send_data_8bit( 0x00 );

    display_send_command( 0xC0 );    // Power control
    display_send_data_8bit( 0x22 );  // VRH[5:0]

    display_send_command( 0xC1 );    // Power control
    display_send_data_8bit( 0x11 );  // SAP[2:0];BT[3:0]

    display_send_command( 0xC5 );  // VCM control
    display_send_data_8bit( 0x3d );
    display_send_data_8bit( 0x20 );

    display_send_command( 0xC7 );    // VCM control2
    display_send_data_8bit( 0xAA );  // 0xB0

    display_send_command( 0x36 );    // Memory Access Control
    display_send_data_8bit( 0xC8 );  // display_send_data_8bit(0x08); if not inverted

    display_send_command( 0x3A );
    display_send_data_8bit( 0x55 );

    display_send_command( 0xB1 );
    display_send_data_8bit( 0x00 );
    display_send_data_8bit( 0x13 );

    display_send_command( 0xB6 );  // Display Function Control
    display_send_data_8bit( 0x0A );
    display_send_data_8bit( 0xA2 );

    display_send_command( 0xF6 );
    display_send_data_8bit( 0x01 );
    display_send_data_8bit( 0x30 );

    display_send_command( 0xF2 );  // 3Gamma Function Disable
    display_send_data_8bit( 0x00 );

    display_send_command( 0x26 );  // Gamma curve selected
    display_send_data_8bit( 0x01 );

    display_send_command( 0xE0 );  // Set Gamma
    display_send_data_8bit( 0x0F );
    display_send_data_8bit( 0x3F );
    display_send_data_8bit( 0x2F );
    display_send_data_8bit( 0x0C );
    display_send_data_8bit( 0x10 );
    display_send_data_8bit( 0x0A );
    display_send_data_8bit( 0x53 );
    display_send_data_8bit( 0XD5 );
    display_send_data_8bit( 0x40 );
    display_send_data_8bit( 0x0A );
    display_send_data_8bit( 0x13 );
    display_send_data_8bit( 0x03 );
    display_send_data_8bit( 0x08 );
    display_send_data_8bit( 0x03 );
    display_send_data_8bit( 0x00 );

    display_send_command( 0XE1 );  // Set Gamma
    display_send_data_8bit( 0x00 );
    display_send_data_8bit( 0x00 );

    display_send_data_8bit( 0x10 );
    display_send_data_8bit( 0x03 );
    display_send_data_8bit( 0x0F );
    display_send_data_8bit( 0x05 );
    display_send_data_8bit( 0x2C );
    display_send_data_8bit( 0xA2 );
    display_send_data_8bit( 0x3F );
    display_send_data_8bit( 0x05 );
    display_send_data_8bit( 0x0E );
    display_send_data_8bit( 0x0C );
    display_send_data_8bit( 0x37 );
    display_send_data_8bit( 0x3C );
    display_send_data_8bit( 0x0F );

    display_send_command( 0x11 );  // Exit Sleep

    display_send_command( 0x29 );  // Display on

    LL_GPIO_SetOutputPin( DISPLAY_NSS_PORT, DISPLAY_NSS_PIN );

    LL_mDelay( 5 );
}
