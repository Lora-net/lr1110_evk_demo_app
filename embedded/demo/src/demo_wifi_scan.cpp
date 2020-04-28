/**
 * @file      demo_wifi_scan.cpp
 *
 * @brief     Implementation of the Wi-Fi MAC address scan demonstration.
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

#include "demo_wifi_scan.h"
#include "lr1110_wifi.h"
#include <string.h>

#define WIFI_SCAN_ABORT_ON_TIMEOUT ( false )
#define DEMO_WIFI_MAX_RESULTS_PER_SCAN ( 20 )

DemoWifiScan::DemoWifiScan( radio_t* radio, SignalingInterface* signaling ) : DemoWifiInterface( radio, signaling ) {}

DemoWifiScan::~DemoWifiScan( ) {}

void DemoWifiScan::ExecuteScan( radio_t* radio )
{
    lr1110_wifi_scan( radio, this->settings.types, this->settings.channels, this->settings.scan_mode,
                      this->settings.max_results, this->settings.nbr_retrials, this->settings.timeout,
                      WIFI_SCAN_ABORT_ON_TIMEOUT );
}

void DemoWifiScan::FetchAndSaveResults( radio_t* radio )
{
    lr1110_wifi_basic_complete_result_t wifi_results_mac_addr[DEMO_WIFI_MAX_RESULTS_PER_SCAN] = { 0 };
    uint8_t                             nbr_results                                           = 0;

    lr1110_wifi_get_nb_results( radio, &nbr_results );
    const uint8_t max_results_to_fetch =
        ( nbr_results > DEMO_WIFI_MAX_RESULTS_PER_SCAN ) ? DEMO_WIFI_MAX_RESULTS_PER_SCAN : nbr_results;

    lr1110_wifi_read_basic_complete_results( this->radio, 0, max_results_to_fetch, wifi_results_mac_addr );

    AddScanToResults( LR1110_SYSTEM_REG_MODE_DCDC, this->results, wifi_results_mac_addr, max_results_to_fetch );
}

void DemoWifiScan::Configure( demo_wifi_settings_t& config ) { this->settings = config; }

void DemoWifiScan::AddScanToResults( const lr1110_system_reg_mode_t regMode, demo_wifi_scan_all_results_t& results,
                                     const lr1110_wifi_basic_complete_result_t* scan_result, const uint8_t nbr_results )
{
    for( uint8_t index = 0; ( index < nbr_results ) && ( results.nbrResults < DEMO_WIFI_MAX_RESULT_TOTAL ); index++ )
    {
        const lr1110_wifi_basic_complete_result_t* local_basic_result = &scan_result[index];
        results.results[results.nbrResults].channel =
            lr1110_extract_channel_from_info_byte( local_basic_result->channel_info_byte );

        results.results[results.nbrResults].type =
            lr1110_extract_signal_type_from_data_rate_info( local_basic_result->data_rate_info_byte );

        memcpy( results.results[results.nbrResults].mac_address, local_basic_result->mac_address,
                LR1110_WIFI_MAC_ADDRESS_LENGTH );

        results.results[results.nbrResults].rssi            = local_basic_result->rssi;
        results.results[results.nbrResults].country_code[0] = '?';
        results.results[results.nbrResults].country_code[1] = '?';
        results.nbrResults++;
    }
}