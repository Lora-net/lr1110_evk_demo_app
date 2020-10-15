/**
 * @file      demo_manager_modem.cpp
 *
 * @brief     Implementation of the demonstration manager for a modem.
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

#include "demo_manager_modem.h"
#include "demo_modem_wifi.h"
#include "demo_modem_gnss_autonomous.h"
#include "demo_modem_gnss_assisted.h"
#include "demo_modem_radio_tx_continuous.h"
#include "demo_modem_radio_tx_cw.h"
#include "demo_modem_radio_rx_continuous.h"

DemoManagerModem::DemoManagerModem( DeviceModem* device, EnvironmentInterface* environment,
                                    AntennaSelectorInterface* antenna_selector, SignalingInterface* signaling,
                                    TimerInterface* timer, CommunicationInterface* communication_interface )
    : DemoManagerInterface( environment, antenna_selector, signaling, timer, communication_interface ), device( device )
{
}

DemoManagerModem::~DemoManagerModem( ) {}

void DemoManagerModem::Start( demo_type_t demo_type )
{
    if( demo_type != this->demo_type_current )
    {
        delete this->running_demo;
        this->running_demo = NULL;

        switch( demo_type )
        {
        case DEMO_TYPE_WIFI:
            this->running_demo = new DemoModemWifi( device, signaling, this->communication_interface );
            break;
        case DEMO_TYPE_GNSS_AUTONOMOUS:
            this->running_demo = new DemoModemGnssAutonomous( device, signaling, environment, antenna_selector, timer,
                                                              this->communication_interface );
            break;
        case DEMO_TYPE_GNSS_ASSISTED:
            this->running_demo = new DemoModemGnssAssisted( device, signaling, environment, antenna_selector, timer,
                                                            this->communication_interface );
            break;
        case DEMO_TYPE_RADIO_PER_TX:
        {
            this->running_demo = new DemoModemRadioTxContinuous( device, signaling, this->communication_interface );
            break;
        }
        case DEMO_TYPE_RADIO_PER_RX:
        {
            this->running_demo = new DemoModemRadioRxContinuous( device, signaling, this->communication_interface );
            break;
        }
        case DEMO_TYPE_TX_CW:
        {
            this->running_demo = new DemoModemRadioTxCw( device, signaling, this->communication_interface );
            break;
        }
        default:
            break;
        }

        this->demo_type_current = demo_type;
    }

    this->running_demo->Initialize( );
    this->running_demo->Reset( );

    switch( demo_type )
    {
    case DEMO_TYPE_WIFI:
        ( ( DemoModemWifi* ) this->running_demo )->Configure( this->demo_wifi_settings );
        break;
    case DEMO_TYPE_GNSS_AUTONOMOUS:
        ( ( DemoModemGnssAutonomous* ) this->running_demo )->Configure( this->demo_gnss_autonomous_settings );
        break;
    case DEMO_TYPE_GNSS_ASSISTED:
        ( ( DemoModemGnssAssisted* ) this->running_demo )->Configure( this->demo_gnss_assisted_settings );
        break;
    case DEMO_TYPE_RADIO_PER_TX:
    {
        ( ( DemoModemRadioTxContinuous* ) this->running_demo )->Configure( this->demo_radio_settings );
        break;
    }
    case DEMO_TYPE_RADIO_PER_RX:
    {
        ( ( DemoModemRadioRxContinuous* ) this->running_demo )->Configure( this->demo_radio_settings );
        break;
    }
    case DEMO_TYPE_TX_CW:
    {
        ( ( DemoModemRadioTxCw* ) this->running_demo )->Configure( this->demo_radio_settings );
        break;
    }
    default:
        break;
    }

    this->running_demo->Start( );
}

void* DemoManagerModem::GetResults( )
{
    switch( demo_type_current )
    {
    case DEMO_TYPE_WIFI:
        return ( void* ) ( ( DemoModemWifi* ) this->running_demo )->GetResult( );
    case DEMO_TYPE_GNSS_AUTONOMOUS:
        return ( void* ) ( ( DemoModemGnssAutonomous* ) this->running_demo )->GetResult( );
    case DEMO_TYPE_GNSS_ASSISTED:
        return ( void* ) ( ( DemoModemGnssAssisted* ) this->running_demo )->GetResult( );
    case DEMO_TYPE_RADIO_PER_RX:
    {
        return ( void* ) ( ( DemoModemRadioRxContinuous* ) this->running_demo )->GetResultsAndResetIntermediateFlag( );
    }
    case DEMO_TYPE_RADIO_PER_TX:
    {
        return ( void* ) ( ( DemoModemRadioTxContinuous* ) this->running_demo )->GetResults( );
    }
    case DEMO_TYPE_TX_CW:
    {
        return ( void* ) ( ( DemoModemRadioTxCw* ) this->running_demo )->GetResults( );
        break;
    }
    default:
        return NULL;
    }
}
