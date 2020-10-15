/**
 * @file      demo_modem_radio_rx_continuous.h
 *
 * @brief     Definition of the Continuous Packet Reception with Modem demonstration.
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

#ifndef __DEMO_MODEM_RADIO_RX_CONTINUOUS_H__
#define __DEMO_MODEM_RADIO_RX_CONTINUOUS_H__

#include "demo_modem_interface.h"
#include "demo_configuration.h"
#include "demo_per_results.h"

typedef struct
{
    uint32_t                   frequency;
    lr1110_modem_tst_mode_sf_t sf;
    lr1110_modem_tst_mode_bw_t bw;
    lr1110_modem_tst_mode_cr_t cr;
} demo_radio_rx_continuous_settings_t;

typedef enum
{
    DEMO_MODEM_RADIO_RX_CONTINUOUS_STATE_INIT,
    DEMO_MODEM_RADIO_RX_CONTINUOUS_STATE_RUNNING,
} demo_modem_raxdio_rx_continuous_state_t;

class DemoModemRadioRxContinuous : public DemoModemInterface
{
   public:
    DemoModemRadioRxContinuous( DeviceModem* device, SignalingInterface* signaling,
                                CommunicationInterface* communication_interface );
    virtual ~DemoModemRadioRxContinuous( );

    void                      SpecificRuntime( ) override;
    void                      SpecificStop( ) override;
    void                      Configure( demo_radio_settings_t& settings );
    bool                      HasIntermediateResults( ) const override;
    demo_radio_per_results_t* GetResultsAndResetIntermediateFlag( );

   protected:
    static bool                             convert_radio_settings( const demo_radio_settings_t*         radio_settings,
                                                                    demo_radio_rx_continuous_settings_t* demo_radio_rx_continuous_settings );
    demo_radio_rx_continuous_settings_t     settings;
    demo_modem_raxdio_rx_continuous_state_t state;
    bool                                    has_intermediate_results;
    demo_radio_per_results_t                results;
};

#endif  // __DEMO_MODEM_RADIO_RX_CONTINUOUS_H__