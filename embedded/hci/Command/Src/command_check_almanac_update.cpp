/**
 * @file      command_check_almanac_update.cpp
 *
 * @brief     Implementation of the HCI command to set almanac update class.
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

#include "command_check_almanac_update.h"
#include "com_code.h"

#define COMMAND_CHECK_ALMANAC_UPDATE_BUFFER_SIZE ( 4 )

CommandCheckAlmanacUpdate::CommandCheckAlmanacUpdate( DeviceBase* device, Hci& hci )
    : CommandBase( device, hci ), expected_crc( 0 )
{
}

CommandCheckAlmanacUpdate::~CommandCheckAlmanacUpdate( ) {}

uint16_t CommandCheckAlmanacUpdate::GetComCode( ) { return COM_CODE_CHECK_ALMANAC_UPDATE; }

bool CommandCheckAlmanacUpdate::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool configuration_success = false;
    if( buffer_size == COMMAND_CHECK_ALMANAC_UPDATE_BUFFER_SIZE )
    {
        this->expected_crc    = buffer[0] + ( buffer[1] << 8 ) + ( buffer[2] << 16 ) + ( buffer[3] << 24 );
        configuration_success = true;
    }
    else
    {
        configuration_success = false;
    }
    return configuration_success;
}

bool CommandCheckAlmanacUpdate::Job( )
{
    bool job_success = false;
    job_success      = this->device->checkAlmanacUpdate( this->expected_crc );
    return job_success;
}