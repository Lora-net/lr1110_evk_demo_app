/**
 * @file      demo_wifi_country_code.h
 *
 * @brief     Definition of Wi-Fi country code scan demo.
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

#ifndef __DEMO_WIFI_COUNTRY_CODE_H__
#define __DEMO_WIFI_COUNTRY_CODE_H__

#include "demo_wifi_interface.h"

class DemoWifiCountryCode : public DemoWifiInterface
{
   public:
    DemoWifiCountryCode( radio_t* radio, SignalingInterface* signaling );
    virtual ~DemoWifiCountryCode( );

    void Configure( demo_wifi_country_code_settings_t& config );

   protected:
    virtual void ExecuteScan( radio_t* radio );
    virtual void FetchAndSaveResults( radio_t* radio );
    void         AddScanToResults( const lr1110_system_reg_mode_t regMode, demo_wifi_scan_all_results_t& results,
                                   const lr1110_wifi_channel_mask_t channels, const lr1110_wifi_country_code_t* scan_result,
                                   const uint8_t nbr_results );

   private:
    demo_wifi_country_code_settings_t settings;
};

#endif  // __DEMO_WIFI_COUNTRY_CODE_H__