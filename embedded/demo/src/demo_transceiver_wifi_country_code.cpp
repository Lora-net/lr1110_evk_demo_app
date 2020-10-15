/**
 * @file      demo_transceiver_wifi_country_code.cpp
 *
 * @brief     Implementation of the Wi-Fi country code scan demonstration.
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

#include "demo_transceiver_wifi_country_code.h"
#include <string.h>

#define DEMO_WIFI_MAX_RESULTS_PER_SCAN ( 20 )

DemoTransceiverWifiCountryCode::DemoTransceiverWifiCountryCode( DeviceTransceiver*      device,
                                                                SignalingInterface*     signaling,
                                                                CommunicationInterface* communication_interface,
                                                                EnvironmentInterface*   environment )
    : DemoTransceiverWifiInterface( device, signaling, communication_interface, environment )
{
}

DemoTransceiverWifiCountryCode::~DemoTransceiverWifiCountryCode( ) {}

void DemoTransceiverWifiCountryCode::Configure( demo_wifi_country_code_settings_t& config ) { this->settings = config; }

void DemoTransceiverWifiCountryCode::ExecuteScan( radio_t* radio )
{
    lr1110_wifi_search_country_code( radio, this->settings.channels, this->settings.max_results,
                                     this->settings.nbr_retrials, this->settings.timeout,
                                     this->settings.does_abort_on_timeout );
}

void DemoTransceiverWifiCountryCode::FetchAndSaveResults( radio_t* radio )
{
    lr1110_wifi_country_code_t wifi_results_country_code[DEMO_WIFI_MAX_RESULTS_PER_SCAN] = { 0 };
    uint8_t                    nbr_results                                               = 0;

    lr1110_wifi_get_nb_country_code_results( radio, &nbr_results );
    const uint8_t max_results_to_fetch =
        ( nbr_results > DEMO_WIFI_MAX_RESULTS_PER_SCAN ) ? DEMO_WIFI_MAX_RESULTS_PER_SCAN : nbr_results;
    lr1110_wifi_read_country_code_results( radio, 0, max_results_to_fetch, wifi_results_country_code );

    AddScanToResults( LR1110_SYSTEM_REG_MODE_DCDC, this->results, this->settings.channels, wifi_results_country_code,
                      max_results_to_fetch );
}

void DemoTransceiverWifiCountryCode::AddScanToResults( const lr1110_system_reg_mode_t    regMode,
                                                       demo_wifi_scan_all_results_t&     results,
                                                       const lr1110_wifi_channel_mask_t  channels,
                                                       const lr1110_wifi_country_code_t* scan_result,
                                                       const uint8_t                     nbr_results )
{
    for( uint8_t index = 0; ( index < nbr_results ) && ( results.nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        results.results[results.nbrResults].channel =
            lr1110_wifi_extract_channel_from_info_byte( scan_result[index].channel_info_byte );

        results.results[results.nbrResults].type = DEMO_WIFI_TYPE_B;

        memcpy( results.results[results.nbrResults].mac_address, scan_result[index].mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        results.results[results.nbrResults].rssi            = 0;
        results.results[results.nbrResults].country_code[0] = scan_result[index].country_code[0];
        results.results[results.nbrResults].country_code[1] = scan_result[index].country_code[1];
        results.nbrResults++;
    }
}
