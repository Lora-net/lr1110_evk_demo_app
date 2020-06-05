/**
 * @file      communication_utils.cpp
 *
 * @brief     Implementation of the communication utilities.
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

#include "communication_utils.h"

CommunicationUtilsReceptionStatus_t CommunicationUtilsReceiveData( const uint16_t buffer_length_max, char* buffer,
                                                                   uint16_t&      buffer_length_received,
                                                                   const uint16_t timeout )
{
    uint32_t timer_count_ms      = system_time_GetTicker( );
    int      c                   = 0;
    uint16_t local_buffer_length = 0;
    system_uart_start_receiving( );
    while( true )
    {
        if( system_uart_is_readable( ) )
        {
            c                           = system_uart_receive_char( );
            buffer[local_buffer_length] = c;
            local_buffer_length++;
            if( c == 0 )
            {
                buffer_length_received = local_buffer_length;
                return COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OK;
            }
            timer_count_ms = system_time_GetTicker( );
        }
        if( buffer_length_received >= buffer_length_max )
        {
            buffer_length_received = local_buffer_length;
            return COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OVERFLOW;
        }
        if( ( system_time_GetTicker( ) - timer_count_ms ) > ( timeout ) )
        {
            buffer_length_received = local_buffer_length;
            return COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_TIMEOUT;
        }
    }
    system_uart_stop_receiving( );
}