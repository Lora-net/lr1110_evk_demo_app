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

#include "demo_base.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void DemoBaseInterruptHandler( void ) { DemoBase::InterruptHandler( ); }
#ifdef __cplusplus
}
#endif

volatile bool DemoBase::has_received_interrupt = false;
bool          DemoBase::is_initialized         = false;
DemoBase*     DemoBase::running_demo           = nullptr;

DemoBase::DemoBase( DeviceBase* device, SignalingInterface* signaling, CommunicationInterface* communication_interface )
    : device( device ),
      signaling( signaling ),
      is_waiting_for_interrupt( false ),
      status( DEMO_STATUS_SKIPPED ),
      communication_interface( communication_interface )
{
}

void DemoBase::ResetAndInitLr1110( ) { this->device->ResetAndInit( ); }

bool DemoBase::HasIntermediateResults( ) const { return false; }

DemoBase::~DemoBase( ) {}

void DemoBase::Initialize( )
{
    if( is_initialized == false )
    {
        DemoBase::ResetAndInitLr1110( );
        is_initialized = true;
    }
}

demo_status_t DemoBase::GetStatus( ) const { return this->status; }

void DemoBase::Start( )
{
    this->Reset( );
    this->status           = DEMO_STATUS_RUNNING;
    DemoBase::running_demo = this;
}

void DemoBase::Stop( )
{
    this->status = DEMO_STATUS_STOPPED;
    this->SpecificStop( );
    DemoBase::running_demo = nullptr;
}

bool DemoBase::IsStarted( ) const { return this->status == DEMO_STATUS_RUNNING; }

demo_status_t DemoBase::Runtime( )
{
    this->is_waiting_for_interrupt = false;
    if( this->IsStarted( ) )
    {
        this->SpecificRuntime( );
    }
    return this->status;
}

void DemoBase::InterruptHandler( )
{
    if( running_demo )
    {
        running_demo->SpecificInterruptHandler( );
    }
    has_received_interrupt = true;
}

void DemoBase::SetWaitingForInterrupt( ) { this->is_waiting_for_interrupt = true; }

bool DemoBase::IsWaitingForInterrupt( ) const { return this->is_waiting_for_interrupt; }

bool DemoBase::InterruptHasRaised( )
{
    __disable_irq( );
    bool interrupt_flag          = this->has_received_interrupt;
    this->has_received_interrupt = false;
    __enable_irq( );
    return interrupt_flag;
}

void DemoBase::Reset( )
{
    this->is_waiting_for_interrupt = false;
    if( this->has_received_interrupt )
    {
        this->has_received_interrupt = false;
        this->ClearRegisteredIrqs( );
    }
    this->status = DEMO_STATUS_SKIPPED;
}

void DemoBase::Terminate( ) { this->status = DEMO_STATUS_TERMINATED; }
