/**
 * @file      command_status.cpp
 *
 * @brief     Implementation of the HCI command to get status class.
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

#include "command_status.h"
#include "com_code.h"

CommandStatus::CommandStatus( Hci& hci ) : hci( &hci ) {}

CommandStatus::~CommandStatus( ) {}

CommandEvent_t CommandStatus::Execute( )
{
    uint8_t buffer_response[6] = { 0 };

    const uint8_t  counter_error      = hci->GetCounterError( );
    const uint16_t counter_message_rx = hci->GetCounterCommandReceived( );
    const uint16_t counter_frame_tx   = hci->GetCounterFrameSent( );
    const uint16_t response_code      = this->GetComCode( );

    buffer_response[0] = counter_error & 0x00FF;
    buffer_response[1] = ( counter_error & 0xFF00 ) >> 8;
    buffer_response[2] = counter_message_rx & 0x00FF;
    buffer_response[3] = ( counter_message_rx & 0xFF00 ) >> 8;
    buffer_response[4] = counter_frame_tx & 0x00FF;
    buffer_response[5] = ( counter_frame_tx & 0xFF00 ) >> 8;

    this->hci->SendResponse( response_code, buffer_response, 6 );
    return COMMAND_NO_EVENT;
}

uint16_t CommandStatus::GetComCode( ) { return COM_CODE_STATUS; }

bool CommandStatus::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_length )
{
    bool success = false;
    if( buffer_length == 0 )
    {
        success = true;
    }
    else
    {
        success = false;
    }
    return success;
}
