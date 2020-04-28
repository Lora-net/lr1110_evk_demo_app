/**
 * @file      hci.cpp
 *
 * @brief     Implementation of the HCI class.
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

#include "hci.h"
#include "system_uart.h"
#include "com_code.h"
#include <string.h>

#define COMCODE_SIZE 2
#define LENGTH_SIZE 2
#define LIMIT_OPERAND_RECEIVE_S ( 1 )

Hci::Hci( CommandFactory& factory, const EnvironmentInterface& environment )
    : can_run( false ),
      state( HCI_STATE_INIT ),
      has_command( false ),
      count_command_received( 0 ),
      count_frame_sent( 0 ),
      count_error( 0 ),
      command_factory( &factory ),
      buffer_length( 0 ),
      environment( environment ),
      operand_start_time( 0 )
{
}

Hci::~Hci( ) {}

void Hci::Start( )
{
    system_uart_start_receiving( );
    system_uart_dma_init( );
    system_uart_register_rx_done_callback( static_cast< void* >( this ), Hci::CallBackRxWrapper );
    system_uart_register_tx_done_callback( static_cast< void* >( this ), Hci::CallBackTxWrapper );
    this->can_run = true;
}

void Hci::Stop( )
{
    system_uart_dma_deinit( );
    system_uart_unregister_rx_done_callback( );
    system_uart_unregister_tx_done_callback( );
    this->can_run = false;
}

void Hci::Reset( )
{
    this->state                  = HCI_STATE_INIT;
    this->can_run                = false;
    this->last_command_received  = NULL;
    this->has_command            = false;
    this->count_command_received = 0;
    this->count_frame_sent       = 0;
    this->count_error            = 0;
    this->operand_start_time     = 0;
}

bool Hci::HasNewCommand( ) const { return this->has_command; }

CommandInterface* Hci::FetchCommand( )
{
    this->has_command = false;
    return this->last_command_received;
}

void Hci::RestartBufferReception( )
{
    this->buffer_length = 0;
    system_uart_start_buffer_reception( COMCODE_SIZE + LENGTH_SIZE, this->buffer );
}

void Hci::Runtime( )
{
    if( !this->can_run )
    {
        return;
    }
    switch( this->state )
    {
    case HCI_STATE_INIT:
    {
        this->RestartBufferReception( );
        state = HCI_STATE_WAIT_COMCODE_SIZE;
        break;
    }

    case HCI_STATE_WAIT_COMCODE_SIZE:
    {
        break;
    }

    case HCI_STATE_WAIT_OPERAND:
    {
        if( this->environment.GetLocalTimeSeconds( ) - this->operand_start_time > LIMIT_OPERAND_RECEIVE_S )
        {
            // Error: timeout while receiving operand
            this->state = HCI_STATE_ERROR;
        }
        break;
    }

    case HCI_STATE_BUILD_COMMAND:
    {
        CommandFactoryStatus_t factory_status = this->command_factory->BuildCommandFromBuffer(
            this->buffer, this->buffer_length, &this->last_command_received );
        if( factory_status == COMMAND_FACTORY_BUILD_SUCCESS )
        {
            this->state = HCI_STATE_COMMAND_RECEIVED;
        }
        else
        {
            this->state = HCI_STATE_ERROR;
        }
        break;
    }

    case HCI_STATE_COMMAND_RECEIVED:
    {
        this->has_command = true;
        this->count_command_received++;
        this->RestartBufferReception( );
        this->state = HCI_STATE_WAIT_COMCODE_SIZE;
        break;
    }

    case HCI_STATE_ERROR:
    {
        this->count_error++;
        system_uart_reset( );
        this->SendError( 0x00 );
        this->state = HCI_STATE_INIT;
        break;
    }

        // default:
    }
}

void Hci::SendError( const uint16_t error_code ) { SendResponse( ERROR_CODE_EVENT, error_code ); }

void Hci::EventNotify( ) { this->SendResponse( RESP_CODE_EVENT ); }

void Hci::SendResponse( const uint16_t resp_code, const uint8_t* payload, const uint16_t payload_length )
{
    if( !this->can_run )
    {
        return;
    }
    const uint16_t buffer_tx_length = 4 + payload_length;
    if( buffer_tx_length > MAX_TRANSMITION_BUFFER )
    {
        return;
    }

    while( !system_uart_is_tx_terminated( ) )
    {
    }  // Wait for previous Tx to be terminated otherwise one would be changing
       // the tx buffer while it is transmitting

    this->buffer_tx[0] = ( uint8_t )( resp_code & 0x00FF );
    this->buffer_tx[1] = ( uint8_t )( ( resp_code & 0xFF00 ) >> 8 );
    this->buffer_tx[2] = ( uint8_t )( payload_length & 0x00FF );
    this->buffer_tx[3] = ( uint8_t )( ( payload_length & 0xFF00 ) >> 8 );
    memcpy( this->buffer_tx + 4, payload, payload_length );

    this->SendFrame( this->buffer_tx, buffer_tx_length );
}

void Hci::SendResponse( const uint16_t resp_code )
{
    uint8_t fake_buffer = 0;
    this->SendResponse( resp_code, &fake_buffer, 0 );
}

void Hci::SendResponse( const uint16_t resp_code, const uint8_t value ) { this->SendResponse( resp_code, &value, 1 ); }

void Hci::SendFrame( uint8_t* buffer, const uint16_t buffer_length )
{
    if( !this->can_run )
    {
        return;
    }
    // bool send_success = system_uart_send_buffer(buffer, buffer_length);
    // if(!send_success){
    //   this->count_error++;
    // }
    while( !system_uart_send_buffer( buffer, buffer_length ) )
    {
    }
}

void Hci::CallbackRx( )
{
    switch( this->state )
    {
    case HCI_STATE_WAIT_COMCODE_SIZE:
    {
        uint16_t length     = buffer[2] + buffer[3] * 256;
        this->buffer_length = 4 + length;
        if( length > 0 )
        {
            if( length < MAX_RECEPTION_BUFFER - 4 )
            {
                system_uart_start_buffer_reception( length, this->buffer + COMCODE_SIZE + LENGTH_SIZE );
                this->operand_start_time = this->environment.GetLocalTimeSeconds( );
                this->state              = HCI_STATE_WAIT_OPERAND;
            }
            else
            {
                // Error: trying to receive a payload that would overflow the
                // reception buffer
                this->state = HCI_STATE_ERROR;
            }
        }
        else
        {
            this->state = HCI_STATE_BUILD_COMMAND;
        }

        break;
    }

    case HCI_STATE_WAIT_OPERAND:
    {
        this->state = HCI_STATE_BUILD_COMMAND;
        break;
    }
    default:
        break;
    }
}

void Hci::CallbackTx( ) { this->count_frame_sent++; }

void Hci::CallBackRxWrapper( void* self ) { static_cast< Hci* >( self )->CallbackRx( ); }

void Hci::CallBackTxWrapper( void* self ) { static_cast< Hci* >( self )->CallbackTx( ); }

uint16_t Hci::GetCounterError( ) const { return this->count_error; }

uint16_t Hci::GetCounterCommandReceived( ) const { return this->count_command_received; }

uint16_t Hci::GetCounterFrameSent( ) const { return this->count_frame_sent; }
