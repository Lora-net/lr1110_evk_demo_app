/**
 * @file      demo_base.cpp
 *
 * @brief     Implementation of the components shared by all demonstrations.
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

#include "demo_interface.h"

volatile bool DemoInterface::has_received_interrupt = false;
bool          DemoInterface::is_initialized         = false;

DemoInterface::DemoInterface( DeviceInterface* device, SignalingInterface* signaling,
                              CommunicationInterface* communication_interface )
    : device( device ),
      signaling( signaling ),
      communication_interface( communication_interface ),
      is_waiting_for_interrupt( false ),
      status( DEMO_STATUS_SKIPPED )
{
}

void DemoInterface::ResetAndInitLr1110( ) { this->device->Init( ); }

bool DemoInterface::HasIntermediateResults( ) const { return false; }

DemoInterface::~DemoInterface( ) {}

void DemoInterface::Initialize( )
{
    if( is_initialized == false )
    {
        DemoInterface::ResetAndInitLr1110( );
        is_initialized = true;
    }
}

demo_status_t DemoInterface::GetStatus( ) const { return this->status; }

void DemoInterface::Start( )
{
    this->Reset( );
    this->status = DEMO_STATUS_RUNNING;
}

void DemoInterface::Stop( )
{
    this->status = DEMO_STATUS_STOPPED;
    this->SpecificStop( );
}

bool DemoInterface::IsStarted( ) const { return this->status == DEMO_STATUS_RUNNING; }

demo_status_t DemoInterface::Runtime( )
{
    this->is_waiting_for_interrupt = false;
    if( this->IsStarted( ) )
    {
        this->SpecificRuntime( );
    }
    return this->status;
}

void DemoInterface::InterruptHandler( const InterruptionInterface* interruption )
{
    this->SpecificInterruptHandler( interruption );
    has_received_interrupt = true;
}

void DemoInterface::SetWaitingForInterrupt( ) { this->is_waiting_for_interrupt = true; }

bool DemoInterface::IsWaitingForInterrupt( ) const { return this->is_waiting_for_interrupt; }

bool DemoInterface::InterruptHasRaised( )
{
    __disable_irq( );
    bool interrupt_flag          = this->has_received_interrupt;
    this->has_received_interrupt = false;
    __enable_irq( );
    return interrupt_flag;
}

void DemoInterface::Reset( )
{
    this->is_waiting_for_interrupt = false;
    if( this->has_received_interrupt )
    {
        this->has_received_interrupt = false;
        this->ClearRegisteredIrqs( );
    }
    this->status = DEMO_STATUS_SKIPPED;
}

void DemoInterface::Terminate( ) { this->status = DEMO_STATUS_TERMINATED; }
