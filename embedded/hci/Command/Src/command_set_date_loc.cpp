/**
 * @file      command_set_date_loc.cpp
 *
 * @brief     Implementation of the HCI command to set date and location class.
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

#include "command_set_date_loc.h"
#include "com_code.h"
#include "lr1110_gnss_types.h"

CommandSetDateLoc::CommandSetDateLoc( DeviceInterface* device, Hci& hci, EnvironmentInterface& environment )
    : CommandBase( device, hci ), environment( environment )
{
}

CommandSetDateLoc::~CommandSetDateLoc( ) {}

uint16_t CommandSetDateLoc::GetComCode( ) { return COM_CODE_SET_DATE_LOC; }

bool CommandSetDateLoc::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_size )
{
    bool                   configuration_success = false;
    lr1110_gnss_date_t     gnss_time             = 0;
    environment_location_t gnss_position         = { 0 };
    int32_t                longitude             = 0;
    int32_t                latitude              = 0;
    int32_t                altitude              = 0;
    if( buffer_size == 16 )
    {
        gnss_time = buffer[0] | ( buffer[1] << 8 ) | ( buffer[2] << 16 ) | ( buffer[3] << 24 );
        longitude = buffer[4] | ( buffer[5] << 8 ) | ( buffer[6] << 16 ) | ( buffer[7] << 24 );
        latitude  = buffer[8] | ( buffer[9] << 8 ) | ( buffer[10] << 16 ) | ( buffer[11] << 24 );
        altitude  = buffer[12] | ( buffer[13] << 8 ) | ( buffer[14] << 16 ) | ( buffer[15] << 24 );

        gnss_position.altitude  = ( float ) altitude / 1000.0f;
        gnss_position.latitude  = ( float ) latitude / 1000.0f;
        gnss_position.longitude = ( float ) longitude / 1000.0f;

        this->environment.SetTimeFromGpsEpoch( gnss_time );
        this->environment.SetLocation( gnss_position );
        configuration_success = true;
    }
    else
    {
        configuration_success = false;
    }
    return configuration_success;
}

bool CommandSetDateLoc::Job( )
{
    // Does nothing here as all is already done in configuration
    // That's because the time is provided by the host as absolute time
    // that must be set directly on command reception in case the
    // job execution is delayed.
    this->SetNoEvent( );
    return true;
}
