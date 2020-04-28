/**
 * @file      demo_tx_cw.h
 *
 * @brief     Definition of Continuous Wave Transmit demo.
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

#ifndef __DEMO_TX_CW_H__
#define __DEMO_TX_CW_H__

#include "demo_base.h"

typedef enum
{
    DEMO_TX_CW_STATE_INIT,
    DEMO_TX_CW_STATE_RUNNING,
} demo_tx_cw_state_t;

class DemoTxCw : public DemoBase
{
   public:
    DemoTxCw( radio_t* radio, SignalingInterface* signaling );
    virtual ~DemoTxCw( );

    void Configure( demo_radio_settings_t& settings );

   protected:
    virtual void SpecificRuntime( );
    virtual void SpecificStop( );
    virtual void SpecificInterruptHandler( );
    virtual void ClearRegisteredIrqs( ) const;

    void ConfigureRadio( ) const;
    void StartSendMessage( ) const;
    void StartReceptionMessage( ) const;

   private:
    demo_radio_settings_t settings;
    demo_tx_cw_state_t    state;
};

#endif  // __DEMO_TX_CW_H__