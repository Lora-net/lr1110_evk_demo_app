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

DemoBase::DemoBase( radio_t* radio, SignalingInterface* signaling )
    : radio( radio ),
      signaling( signaling ),
      is_waiting_for_interrupt( false ),
      started( false ),
      status( DEMO_STATUS_SKIPPED )
{
}

void DemoBase::ResetAndInitLr1110( const radio_t* radio )
{
    lr1110_system_reset( radio );

    lr1110_system_set_reg_mode( radio, LR1110_SYSTEM_REG_MODE_DCDC );

    lr1110_system_rfswitch_cfg_t rf_switch_setup = { 0 };
    rf_switch_setup.enable                       = DEMO_COMMON_RF_SWITCH_ENABLE;
    rf_switch_setup.standby                      = DEMO_COMMON_RF_SWITCH_STANDBY;
    rf_switch_setup.tx                           = DEMO_COMMON_RF_SWITCH_TX;
    rf_switch_setup.rx                           = DEMO_COMMON_RF_SWITCH_RX;
    rf_switch_setup.wifi                         = DEMO_COMMON_RF_SWITCH_WIFI;
    rf_switch_setup.gnss                         = DEMO_COMMON_RF_SWITCH_GNSS;
    lr1110_system_set_dio_as_rf_switch( radio, &rf_switch_setup );

    lr1110_system_set_tcxo_mode( radio, LR1110_SYSTEM_TCXO_CTRL_3_0V, 500 );

    lr1110_system_cfg_lfclk( radio, LR1110_SYSTEM_LFCLK_XTAL, true );

    lr1110_system_clear_errors( radio );
    lr1110_system_calibrate( radio, 0x3F );

    uint16_t errors;
    lr1110_system_get_errors( radio, &errors );
    lr1110_system_clear_errors( radio );
    lr1110_system_clear_irq_status( radio, LR1110_SYSTEM_IRQ_ALL_MASK );
}

bool DemoBase::HasIntermediateResults( ) const { return false; }

DemoBase::~DemoBase( ) {}

void DemoBase::Initialize( )
{
    if( is_initialized == false )
    {
        DemoBase::ResetAndInitLr1110( this->radio );
        is_initialized = true;
    }
}

demo_status_t DemoBase::GetStatus( ) const { return this->status; }

void DemoBase::Enable( ) { this->status = DEMO_STATUS_PENDING; }

void DemoBase::Disable( ) { this->status = DEMO_STATUS_SKIPPED; }

void DemoBase::Start( )
{
    if( this->IsPending( ) )
    {
        this->Reset( );
        this->status           = DEMO_STATUS_RUNNING;
        this->started          = true;
        DemoBase::running_demo = this;
    }
}

void DemoBase::Stop( )
{
    this->started = false;
    this->status  = DEMO_STATUS_STOPPED;
    this->SpecificStop( );
    DemoBase::running_demo = nullptr;
}

bool DemoBase::IsStarted( ) const { return this->started; }

bool DemoBase::IsPending( ) const { return this->status == DEMO_STATUS_PENDING; }

demo_status_t DemoBase::Runtime( )
{
    this->is_waiting_for_interrupt = false;
    if( this->started )
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
    this->started = false;
    this->status  = DEMO_STATUS_SKIPPED;
}

void DemoBase::Terminate( ) { this->status = DEMO_STATUS_TERMINATED; }
