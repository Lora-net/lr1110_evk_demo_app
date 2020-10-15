/**
 * @file      demo_transceiver_interface.cpp
 *
 * @brief     Implementation of transceiver demo class.
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

#include "demo_transceiver_interface.h"

DemoTransceiverInterface::DemoTransceiverInterface( DeviceTransceiver* device, SignalingInterface* signaling,
                                                    CommunicationInterface* communication_interface,
                                                    EnvironmentInterface*   environment )
    : DemoInterface( device, signaling, communication_interface ),
      environment( environment ),
      last_received_irq_mask( 0 ),
      last_irq_received_instant_ms( 0 )
{
}

void DemoTransceiverInterface::SpecificInterruptHandler( const InterruptionInterface* interruption )
{
    const InterruptionIrq* interrupt = dynamic_cast< const InterruptionIrq* >( interruption );
    if( interrupt != 0 )
    {
        this->last_received_irq_mask       = interrupt->get_irq_mask( );
        this->last_received_stat_1         = interrupt->get_stat_1( );
        this->last_irq_received_instant_ms = environment->GetLocalTimeMilliseconds( );
    }
}