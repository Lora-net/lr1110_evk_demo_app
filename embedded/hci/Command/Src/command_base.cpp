/**
 * @file      command_base.cpp
 *
 * @brief     Implementation of the HCI command base class.
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

#include "command_base.h"

CommandBase::CommandBase( DeviceBase* device, Hci& hci ) : device( device ), hci( &hci ), event( COMMAND_NO_EVENT ) {}

CommandBase::~CommandBase( ) {}

CommandEvent_t CommandBase::Execute( )
{
    const bool job_success = this->Job( );
    this->SendResponseBuffer( ( const uint8_t* ) &job_success, 1 );
    return this->event;
}

void CommandBase::SendResponseBuffer( const uint8_t* buffer, const uint16_t buffer_length )
{
    uint16_t response_code = this->GetComCode( );
    this->hci->SendResponse( response_code, buffer, buffer_length );
}

void CommandBase::SetEventStartDemo( CommandBaseDemoId_t demo_to_start )
{
    switch( demo_to_start )
    {
    case COMMAND_BASE_DEMO_WIFI_SCAN:
    {
        this->event = COMMAND_START_WIFI_SCAN_DEMO_EVENT;
        break;
    }

    case COMMAND_BASE_DEMO_WIFI_COUNTRY_CODE:
    {
        this->event = COMMAND_START_WIFI_COUNTRY_CODE_DEMO_EVENT;
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_AUTONOMOUS:
    {
        this->event = COMMAND_START_GNSS_AUTONOMOUS_DEMO_EVENT;
        break;
    }

    case COMMAND_BASE_DEMO_GNSS_ASSISTED:
    {
        this->event = COMMAND_START_GNSS_ASSISTED_DEMO_EVENT;
        break;
    }

    default:
    {
        this->event = COMMAND_NO_EVENT;
    }
    }
}

void CommandBase::SetEventStopDemo( ) { this->event = COMMAND_STOP_DEMO_EVENT; }

void CommandBase::SetEventResetDemo( ) { this->event = COMMAND_RESET_DEMO_EVENT; }

void CommandBase::SetNoEvent( ) { this->event = COMMAND_NO_EVENT; }
