/**
 * @file      command_update_almanac.cpp
 *
 * @brief     Implementation of the HCI command to update almanac class.
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

#include "command_update_almanac.h"
#include "com_code.h"

CommandUpdateAlmanac::CommandUpdateAlmanac( DeviceBase* device, Hci& hci ) : CommandBase( device, hci )
{
    for( uint16_t index_buffer = 0; index_buffer < COMMAND_UPDATE_ALMANAC_BUFFER_SIZE; index_buffer++ )
    {
        this->almanac_buffer[index_buffer] = 0;
    }
}

CommandUpdateAlmanac::~CommandUpdateAlmanac( ) {}

uint16_t CommandUpdateAlmanac::GetComCode( ) { return COM_CODE_UPDATE_ALMANAC; }

bool CommandUpdateAlmanac::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool configuration_success = false;
    if( buffer_size == COMMAND_UPDATE_ALMANAC_BUFFER_SIZE )
    {
        for( uint8_t index = 0; index < COMMAND_UPDATE_ALMANAC_BUFFER_SIZE; index++ )
        {
            this->almanac_buffer[index] = buffer[index];
        }
        configuration_success = true;
    }
    else
    {
        configuration_success = false;
    }
    return configuration_success;
}

bool CommandUpdateAlmanac::Job( )
{
    bool job_success = false;
    this->device->UpdateAlmanac( this->almanac_buffer, COMMAND_UPDATE_ALMANAC_BUFFER_SIZE );
    job_success = true;
    return job_success;
}