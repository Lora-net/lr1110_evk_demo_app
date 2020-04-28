/**
 * @file      hci.h
 *
 * @brief     Definition of the HCI command base class.
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

#ifndef __HCI__
#define __HCI__

#include "command_factory.h"
#include "command_interface.h"
#include "environment_interface.h"
#include <stdint.h>

#define MAX_RECEPTION_BUFFER 64
#define MAX_TRANSMITION_BUFFER 512

typedef enum
{
    HCI_STATE_INIT,
    HCI_STATE_WAIT_COMCODE_SIZE,
    HCI_STATE_WAIT_OPERAND,
    HCI_STATE_BUILD_COMMAND,
    HCI_STATE_COMMAND_RECEIVED,
    HCI_STATE_ERROR,
} HciState_t;

class Hci
{
   public:
    explicit Hci( CommandFactory& factory, const EnvironmentInterface& environment );
    ~Hci( );

    bool              HasNewCommand( ) const;
    CommandInterface* FetchCommand( );

    void Runtime( );
    void Start( );
    void Reset( );
    void Stop( );

    void EventNotify( );
    void SendResponse( const uint16_t resp_code, const uint8_t* payload, const uint16_t payload_length );
    void SendResponse( const uint16_t resp_code );
    void SendResponse( const uint16_t resp_code, const uint8_t value );
    void SendError( const uint16_t error_code );

    uint16_t GetCounterError( ) const;
    uint16_t GetCounterCommandReceived( ) const;
    uint16_t GetCounterFrameSent( ) const;

   protected:
    void RestartBufferReception( void );
    void SendFrame( uint8_t* buffer, const uint16_t buffer_length );

    void CallbackRx( );
    void CallbackTx( );

    static void CallBackRxWrapper( void* self );
    static void CallBackTxWrapper( void* self );

   private:
    bool                        can_run;
    volatile HciState_t         state;
    bool                        has_command;
    uint16_t                    count_command_received;
    volatile uint16_t           count_frame_sent;
    uint16_t                    count_error;
    CommandInterface*           last_command_received;
    CommandFactory*             command_factory;
    uint8_t                     buffer[MAX_RECEPTION_BUFFER];
    uint16_t                    buffer_length;
    uint8_t                     buffer_tx[MAX_TRANSMITION_BUFFER];
    const EnvironmentInterface& environment;
    volatile time_t             operand_start_time;
};

#endif  // __HCI__
