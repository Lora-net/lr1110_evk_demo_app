/**
 * @file      command_factory.cpp
 *
 * @brief     Implementation of the HCI command factory class.
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

#include "command_factory.h"
#include <stddef.h>

CommandFactory::CommandFactory( ) : n_command( 0 )
{
    for( uint16_t com_index = 0; com_index < SIZE_COMMAND_POOL; com_index++ )
    {
        this->commands[com_index] = NULL;
    }
}

CommandFactory::~CommandFactory( ) {}

bool CommandFactory::AddCommandToPool( CommandInterface& command )
{
    if( this->n_command < SIZE_COMMAND_POOL - 1 )
    {
        this->commands[this->n_command] = &command;
        this->n_command++;
        return true;
    }
    else
    {
        return false;
    }
}

CommandFactoryStatus_t CommandFactory::BuildCommandFromBuffer( const uint8_t* buffer, const uint16_t buffer_size,
                                                               CommandInterface** command )
{
    const uint16_t com_code             = buffer[0] + buffer[1] * 256;
    const uint16_t payload_size         = buffer[2] + buffer[3] * 256;
    const uint16_t expected_buffer_size = payload_size + 4;

    if( expected_buffer_size != buffer_size )
    {
        return COMMAND_FACTORY_BUILD_WRONG_SIZE;
    }

    bool search_success = this->SearchCommandInPool( com_code, command );
    if( !search_success )
    {
        return COMMAND_FACTORY_BUILD_UNKNOWN_COM_CODE;
    }

    bool configuration_success = ( *command )->ConfigureFromPayload( buffer + 4, buffer_size - 4 );
    if( !configuration_success )
    {
        return COMMAND_FACTORY_BUILD_ERROR_COMMAND_CONFIGURATION;
    }

    return COMMAND_FACTORY_BUILD_SUCCESS;
}

bool CommandFactory::SearchCommandInPool( const uint16_t com_code, CommandInterface** command )
{
    bool success = false;
    for( uint8_t index_command_pool = 0; index_command_pool < this->n_command; index_command_pool++ )
    {
        CommandInterface* local_command = this->commands[index_command_pool];
        if( local_command->GetComCode( ) == com_code )
        {
            ( *command ) = local_command;
            success      = true;
            break;
        }
    }
    return success;
}
