/**
 * @file      demo_transceiver_gnss_autonomous.cpp
 *
 * @brief     Implementation of the GNSS autonomous demonstration.
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

#include "demo_transceiver_gnss_autonomous.h"
#include "lr1110_gnss.h"

DemoTransceiverGnssAutonomous::DemoTransceiverGnssAutonomous( DeviceTransceiver* device, SignalingInterface* signaling,
                                                              EnvironmentInterface*     environment,
                                                              AntennaSelectorInterface* antenna_selector,
                                                              TimerInterface*           timer,
                                                              CommunicationInterface*   communication_interface )
    : DemoTransceiverGnssInterface( device, signaling, environment, antenna_selector, timer, communication_interface )
{
}

DemoTransceiverGnssAutonomous::~DemoTransceiverGnssAutonomous( ) {}

void DemoTransceiverGnssAutonomous::CallScan( )
{
    const lr1110_gnss_date_t gnss_time =
        DemoTransceiverGnssInterface::GnssTimeFromEnvironment( this->GetEnvironment( )->GetDateTime( ) );

    lr1110_gnss_scan_autonomous(
        this->device->GetRadio( ), gnss_time,
        DemoTransceiverGnssInterface::TransceiverSearchModeFromDemo( this->GetSettings( ).option ),
        LR1110_GNSS_RESULTS_DOPPLER_ENABLE_MASK | LR1110_GNSS_RESULTS_DOPPLER_MASK |
            LR1110_GNSS_RESULTS_BIT_CHANGE_MASK,
        this->GetSettings( ).nb_satellites );
}
