/**
 * @file      demo_transceiver_radio_per.h
 *
 * @brief     Definition of Packet Error Rate demo.
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

#ifndef __DEMO_TRANSCEIVER_RADIO_PER_H__
#define __DEMO_TRANSCEIVER_RADIO_PER_H__

#include "demo_configuration.h"
#include "demo_transceiver_radio_interface.h"
#include "configuration.h"
#include "environment_interface.h"
#include "demo_per_results.h"

typedef enum
{
    DEMO_RADIO_PER_STATE_INIT,
    DEMO_RADIO_PER_STATE_SEND,
    DEMO_RADIO_PER_STATE_WAIT_FOR_TX_DONE,
    DEMO_RADIO_PER_STATE_SET_RX,
    DEMO_RADIO_PER_STATE_WAIT_FOR_RX_DONE,
    DEMO_RADIO_PER_STATE_STOP,
} demo_radio_per_state_t;

typedef enum
{
    DEMO_RADIO_PER_MODE_TX,
    DEMO_RADIO_PER_MODE_RX,
} demo_radio_per_mode_t;

class DemoTransceiverRadioPer : public DemoTransceiverRadioInterface
{
   public:
    DemoTransceiverRadioPer( DeviceTransceiver* device, SignalingInterface* signaling,
                             EnvironmentInterface* environment, CommunicationInterface* communication_interface,
                             demo_radio_per_mode_t mode );
    virtual ~DemoTransceiverRadioPer( );

    bool                            HasIntermediateResults( ) const override;
    const demo_radio_per_results_t* GetResult( ) const;

   protected:
    void SpecificRuntime( ) override;
    void SpecificStop( ) override;
    void LogInfo( ) const;
    void ClearRegisteredIrqs( ) const override;

   private:
    demo_radio_per_state_t   state;
    demo_radio_per_results_t results;
    uint8_t                  buffer[255];
    uint32_t                 nb_of_packets_remaining;
    uint32_t                 last_event;
    bool                     has_intermediate_results;
    demo_radio_per_mode_t    mode;
};

#endif  // __DEMO_TRANSCEIVER_RADIO_PER_H__
