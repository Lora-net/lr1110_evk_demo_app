/**
 * @file      command_factory.h
 *
 * @brief     Definitions of the HCI command factory class.
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

#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include "command_interface.h"

#define SIZE_COMMAND_POOL 32

typedef enum
{
    COMMAND_FACTORY_BUILD_SUCCESS,
    COMMAND_FACTORY_BUILD_WRONG_SIZE,
    COMMAND_FACTORY_BUILD_UNKNOWN_COM_CODE,
    COMMAND_FACTORY_BUILD_ERROR_COMMAND_CONFIGURATION,
} CommandFactoryStatus_t;

class CommandFactory
{
   public:
    CommandFactory( );
    virtual ~CommandFactory( );

    bool AddCommandToPool( CommandInterface& command );

    CommandFactoryStatus_t BuildCommandFromBuffer( const uint8_t* buffer, const uint16_t buffer_size,
                                                   CommandInterface** command );

   protected:
    bool SearchCommandInPool( const uint16_t com_code, CommandInterface** command );

   private:
    CommandInterface* commands[SIZE_COMMAND_POOL];
    uint8_t           n_command;
};

#endif  // __COMMAND_FACTORY_H__
