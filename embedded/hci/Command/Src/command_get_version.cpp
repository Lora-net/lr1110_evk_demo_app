/**
 * @file      command_get_version.cpp
 *
 * @brief     Implementation of the HCI get version command class.
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

#include "command_get_version.h"
#include "com_code.h"
#include "version.h"
#include "demo.h"
#include <stdio.h>

#define VERSION_STRING_MAX_LEN 272

CommandGetVersion::CommandGetVersion( radio_t* radio, Hci& hci ) : hci( &hci ), radio( radio ) {}

CommandGetVersion::~CommandGetVersion( ) {}

bool CommandGetVersion::ConfigureFromPayload( const uint8_t* buffer, const uint16_t buffer_length )
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

CommandEvent_t CommandGetVersion::Execute( )
{
    char buffer_response[VERSION_STRING_MAX_LEN] = { 0 };

    const uint8_t buffer_len = snprintf(
        buffer_response, VERSION_STRING_MAX_LEN,
        "%s;%s;0x%02x;0x%02x;0x%04x;0x%04x;%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
        version_handler->version_sw, version_handler->version_driver,
        version_handler->version_chip_hw, version_handler->version_chip_type,
        version_handler->version_chip_fw, version_handler->almanac_crc,
        version_handler->chip_uid[0], version_handler->chip_uid[1],
        version_handler->chip_uid[2], version_handler->chip_uid[3],
        version_handler->chip_uid[4], version_handler->chip_uid[5],
        version_handler->chip_uid[6], version_handler->chip_uid[7] );

    this->hci->SendResponse( this->GetComCode( ), ( uint8_t* ) buffer_response, buffer_len );

    return COMMAND_NO_EVENT;
}

uint16_t CommandGetVersion::GetComCode( ) { return COM_CODE_GET_VERSION; }

void CommandGetVersion::SetVersion( version_handler_t* version ) { this->version_handler = version; }
