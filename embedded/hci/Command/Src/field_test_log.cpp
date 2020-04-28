/**
 * @file      field_test_log.cpp
 *
 * @brief     Implementation of the field test log singleton class.
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

#include "field_test_log.h"
#include "com_code.h"
#include <stdarg.h>
#include <stdio.h>

#define MIN( a, b ) ( ( a < b ) ? a : b )

FieldTestLog* FieldTestLog::instance = NULL;

FieldTestLog* FieldTestLog::GetOrCreateInstance( Hci& hci )
{
    if( !FieldTestLog::instance )
    {
        FieldTestLog::instance = new FieldTestLog( hci );
    }
    return FieldTestLog::instance;
}

FieldTestLog::FieldTestLog( Hci& hci ) : hci( hci ) {}

FieldTestLog::~FieldTestLog( ) {}

void FieldTestLog::TrySendLog( const char* fmt, ... )
{
    const uint16_t max_response_buffer_size                  = MAX_TRANSMITION_BUFFER - 4;
    uint8_t        response_buffer[max_response_buffer_size] = { '\0' };
    if( FieldTestLog::instance != NULL )
    {
        va_list args;
        va_start( args, fmt );
        uint16_t char_count = vsnprintf( ( char* ) response_buffer, max_response_buffer_size, fmt, args );
        FieldTestLog::instance->SendLog( response_buffer, MIN( char_count, max_response_buffer_size ) );
        va_end( args );
    }
}

void FieldTestLog::SendLog( const uint8_t* buffer, const uint16_t buffer_size )
{
    hci.SendResponse( LOG_RESPONSE_CODE, buffer, buffer_size );
}
