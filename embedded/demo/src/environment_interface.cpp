/**
 * @file      environment_interface.cpp
 *
 * @brief     Interface implementation for interactions with the environment.
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

#include "environment_interface.h"

EnvironmentInterface::EnvironmentInterface( ) : has_date( false ), has_location( false ), delta_time_s( 0 ) {}

EnvironmentInterface::~EnvironmentInterface( ) {}

bool EnvironmentInterface::HasDate( ) const { return this->has_date; }

environment_date_time_t EnvironmentInterface::GetDateTime( ) const
{
    const time_t time_seconds = this->GetLocalTimeSeconds( ) + this->delta_time_s;
    return time_seconds;
}

void EnvironmentInterface::SetTimeFromGpsEpoch( const environment_date_time_t& time_from_gps_epoch )
{
    this->delta_time_s = time_from_gps_epoch - this->GetLocalTimeSeconds( );
    this->has_date     = true;
}

bool EnvironmentInterface::HasLocation( ) const { return this->has_location; }

environment_location_t EnvironmentInterface::GetLocation( ) const { return this->last_location; }

void EnvironmentInterface::SetLocation( const environment_location_t& location )
{
    this->last_location = location;
    this->has_location  = true;
}