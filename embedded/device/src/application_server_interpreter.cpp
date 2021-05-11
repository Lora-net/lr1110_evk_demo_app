/**
 * @file      application_server_interpreter.h
 *
 * @brief     Implementation of application server interpreter
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

#include "application_server_interpreter.h"

enum ApplicationServerTlvTag_e
{
    APPLICATION_SERVER_TLV_TAG_LED = 0x4F,
};

ApplicationServerEvent_t ApplicationServerInterpreter::parseFrame( const uint8_t* frame, uint16_t frame_length )
{
    if( frame_length < 2 )
    {
        return APPLICATION_SERVER_ERROR;
    }

    const uint8_t tlv_tag    = frame[0];
    const uint8_t tlv_length = frame[1];

    if( frame_length != ( tlv_length + 2 ) )
    {
        return APPLICATION_SERVER_ERROR;
    }

    const uint8_t* tlv_value = frame + 2;

    switch( tlv_tag )
    {
    case APPLICATION_SERVER_TLV_TAG_LED:
    {
        if( tlv_length != 1 )
        {
            return APPLICATION_SERVER_ERROR;
        }
        const uint8_t led_state = tlv_value[0];
        if( led_state == 0 )
        {
            return APPLICATION_SERVER_LED_OFF;
        }
        else if( led_state == 1 )
        {
            return APPLICATION_SERVER_LED_ON;
        }
        else if( led_state == 2 )
        {
            return APPLICATION_SERVER_LED_TOGGLE;
        }
        else
        {
            return APPLICATION_SERVER_ERROR;
        }
        break;
    }
    default:
    {
        return APPLICATION_SERVER_ERROR;
    }
    }
}