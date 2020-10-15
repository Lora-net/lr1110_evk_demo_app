/**
 * @file      lr1110_modem_hal.c
 *
 * @brief     HAL implementation for LR1110 modem radio chip
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

#include "lr1110_modem_hal.h"
#include "configuration.h"
#include "system.h"

static lr1110_modem_hal_status_t lr1110_modem_hal_wait_on_busy( const void* radio, uint32_t timeout_ms );
static lr1110_modem_hal_status_t lr1110_modem_hal_wait_on_unbusy( const void* radio, uint32_t timeout_ms );

lr1110_modem_hal_status_t lr1110_modem_hal_reset( const void* radio )
{
    radio_t* radio_local = ( radio_t* ) radio;

    system_gpio_set_pin_state( radio_local->reset, SYSTEM_GPIO_PIN_STATE_LOW );
    system_time_wait_ms( 1 );
    system_gpio_set_pin_state( radio_local->reset, SYSTEM_GPIO_PIN_STATE_HIGH );
    system_time_wait_ms( 100 );
    return LR1110_MODEM_HAL_STATUS_OK;
}

lr1110_modem_hal_status_t lr1110_modem_hal_wakeup( const void* radio )
{
    radio_t* radio_local = ( radio_t* ) radio;

    if( lr1110_modem_hal_wait_on_busy( radio_local, 1000 ) == LR1110_MODEM_HAL_STATUS_OK )
    {
        // Wakeup radio
        system_gpio_set_pin_state( radio_local->nss, 0 );
        system_gpio_set_pin_state( radio_local->nss, 1 );
    }

    // Wait on busy pin for 100 ms
    return lr1110_modem_hal_wait_on_unbusy( radio_local, 1000 );
}

lr1110_modem_hal_status_t lr1110_modem_hal_read( const void* radio, const uint8_t* cbuffer,
                                                 const uint16_t cbuffer_length, uint8_t* rbuffer,
                                                 const uint16_t rbuffer_length )
{
    radio_t* radio_local = ( radio_t* ) radio;

    if( lr1110_modem_hal_wakeup( radio_local ) == LR1110_MODEM_HAL_STATUS_OK )
    {
        uint8_t                   crc          = 0;
        uint8_t                   crc_received = 0;
        lr1110_modem_hal_status_t status;

        // NSS low
        system_gpio_set_pin_state( radio_local->nss, 0 );

        // Send CMD
        system_spi_write( radio_local->spi, cbuffer, cbuffer_length );

        // Compute and send CRC
        crc = lr1110_modem_compute_crc( 0xFF, cbuffer, cbuffer_length );

        system_spi_write( radio_local->spi, &crc, 1 );

        // NSS high
        system_gpio_set_pin_state( radio_local->nss, 1 );

        // Wait on busy pin up to 1000 ms
        if( lr1110_modem_hal_wait_on_busy( radio_local, 1000 ) != LR1110_MODEM_HAL_STATUS_OK )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }

        // Send dummy byte to retrieve RC & CRC

        // NSS low
        system_gpio_set_pin_state( radio_local->nss, 0 );

        // read RC
        system_spi_read( radio_local->spi, &status, 1 );
        // status = ( lr1110_modem_hal_status_t ) system_spi_write_read( radio_local->spi, 0 );

        system_spi_read( radio_local->spi, rbuffer, rbuffer_length );
        // for( uint16_t i = 0; i < data_length; i++ )
        // {
        //     data[i] = system_spi_write_read( radio_local->spi_id, 0 );
        // }

        system_spi_read( radio_local->spi, &crc_received, 1 );
        // crc_received = system_spi_write_read( radio_local->spi_id, 0 );

        // NSS high
        system_gpio_set_pin_state( radio_local->nss, 1 );

        // Compute response crc
        crc = lr1110_modem_compute_crc( 0xFF, ( uint8_t* ) &status, 1 );
        crc = lr1110_modem_compute_crc( crc, rbuffer, rbuffer_length );

        if( crc != crc_received )
        {
            // change the response code
            status = LR1110_MODEM_HAL_STATUS_BAD_FRAME;
        }
        // Wait on busy pin up to 1000 ms
        if( lr1110_modem_hal_wait_on_unbusy( radio_local, 1000 ) != LR1110_MODEM_HAL_STATUS_OK )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }

        // Wait on busy pin up to 1000 ms
        if( lr1110_modem_hal_wait_on_busy( radio_local, 1000 ) != LR1110_MODEM_HAL_STATUS_OK )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }
        else
        {
            return status;
        }
    }

    return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
}

lr1110_modem_hal_status_t lr1110_modem_hal_write( const void* radio, const uint8_t* cbuffer,
                                                  const uint16_t cbuffer_length, const uint8_t* cdata,
                                                  const uint16_t cdata_length )
{
    radio_t* radio_local = ( radio_t* ) radio;

    if( lr1110_modem_hal_wakeup( radio_local ) == LR1110_MODEM_HAL_STATUS_OK )
    {
        uint8_t                   crc          = 0;
        uint8_t                   crc_received = 0;
        lr1110_modem_hal_status_t status;

        // NSS low
        system_gpio_set_pin_state( radio_local->nss, 0 );

        // Send CMD
        system_spi_write( radio_local->spi, cbuffer, cbuffer_length );

        // Send Data
        system_spi_write( radio_local->spi, cdata, cdata_length );

        // Compute and send CRC
        crc = lr1110_modem_compute_crc( 0xFF, cbuffer, cbuffer_length );
        crc = lr1110_modem_compute_crc( crc, cdata, cdata_length );

        system_spi_write( radio_local->spi, &crc, 1 );

        // NSS high
        system_gpio_set_pin_state( radio_local->nss, 1 );

        // Wait on busy pin up to 1000 ms
        if( lr1110_modem_hal_wait_on_busy( radio_local, 1000 ) != LR1110_MODEM_HAL_STATUS_OK )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }

        // Send dummy byte to retrieve RC & CRC

        // NSS low
        system_gpio_set_pin_state( radio_local->nss, 0 );

        // read RC
        system_spi_read( radio_local->spi, &status, 1 );
        system_spi_read( radio_local->spi, &crc_received, 1 );

        // Compute response crc
        crc = lr1110_modem_compute_crc( 0xFF, ( uint8_t* ) &status, 1 );

        // NSS high
        system_gpio_set_pin_state( radio_local->nss, 1 );

        if( crc != crc_received )
        {
            // change the response code
            status = LR1110_MODEM_HAL_STATUS_BAD_FRAME;
        }

        // Don't wait on busy in these following cases
        // 0x0602 - LR1110_MODEM_GROUP_ID_MODEM / LR1110_MODEM_RESET_CMD
        // 0x0603 - LR1110_MODEM_GROUP_ID_MODEM / LR1110_MODEM_FACTORY_RESET_CMD

        if( ( ( ( cbuffer[0] << 8 ) | cbuffer[1] ) != 0x0602 ) && ( ( ( cbuffer[0] << 8 ) | cbuffer[1] ) != 0x0603 ) )
        {
            // Wait on busy pin up to 1000 ms
            if( lr1110_modem_hal_wait_on_unbusy( radio_local, 1000 ) != LR1110_MODEM_HAL_STATUS_OK )
            {
                return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
            }
            else
            {
                return status;
            }
        }
        else
        {
            return status;
        }
    }

    return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
}

lr1110_modem_hal_status_t lr1110_modem_hal_write_read( const void* radio, const uint8_t* cbuffer, uint8_t* rbuffer,
                                                       const uint16_t length )
{
    radio_t* radio_local = ( radio_t* ) radio;

    system_gpio_wait_for_state( radio_local->busy, SYSTEM_GPIO_PIN_STATE_LOW );

    system_gpio_set_pin_state( radio_local->nss, SYSTEM_GPIO_PIN_STATE_LOW );
    system_spi_write_read( radio_local->spi, cbuffer, rbuffer, length );
    system_gpio_set_pin_state( radio_local->nss, SYSTEM_GPIO_PIN_STATE_HIGH );

    return LR1110_MODEM_HAL_STATUS_OK;
}

lr1110_modem_hal_status_t lr1110_modem_hal_wait_on_busy( const void* radio, uint32_t timeout_ms )
{
    radio_t*       radio_local = ( radio_t* ) radio;
    const uint32_t start_time  = system_time_GetTicker( );
    while( system_gpio_get_pin_state( radio_local->busy ) == 0 )
    {
        const uint32_t actual_time = system_time_GetTicker( );
        if( ( actual_time - start_time ) > timeout_ms )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }
    }
    return LR1110_MODEM_HAL_STATUS_OK;
}

lr1110_modem_hal_status_t lr1110_modem_hal_wait_on_unbusy( const void* radio, uint32_t timeout_ms )
{
    radio_t*       radio_local = ( radio_t* ) radio;
    const uint32_t start_time  = system_time_GetTicker( );
    while( system_gpio_get_pin_state( radio_local->busy ) == 1 )
    {
        const uint32_t actual_time = system_time_GetTicker( );
        if( ( actual_time - start_time ) > timeout_ms )
        {
            return LR1110_MODEM_HAL_STATUS_BUSY_TIMEOUT;
        }
    }
    return LR1110_MODEM_HAL_STATUS_OK;
}