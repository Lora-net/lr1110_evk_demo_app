/**
 * @file      demo_modem_wifi.h
 *
 * @brief     Definition of the demo modem wifi class.
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

#ifndef __DEMO_MODEM_WIFI_H__
#define __DEMO_MODEM_WIFI_H__

#include "demo_configuration.h"
#include "demo_modem_interface.h"
#include "lr1110_modem_system.h"
#include "lr1110_modem_wifi.h"
#include "demo_wifi_types.h"

typedef enum
{
    DEMO_MODEM_WIFI_INIT,
    DEMO_MODEM_WIFI_SCAN,
    DEMO_MODEM_WIFI_WAIT_FOR_SCAN,
    DEMO_MODEM_WIFI_TERMINATED,
} demo_modem_wifi_state_t;

class DemoModemWifi : public DemoModemInterface
{
   public:
    DemoModemWifi( DeviceModem* device, SignalingInterface* signaling,
                   CommunicationInterface* communication_interface );

    void Reset( ) override;
    void SpecificRuntime( ) override;
    void SpecificStop( ) override;

    void Configure( demo_wifi_settings_t& config );

    const demo_wifi_scan_all_results_t* GetResult( ) const;

   protected:
    virtual void ExecuteScan( radio_t* radio );
    virtual void FetchAndSaveResults( const uint8_t* buffer, uint16_t buffer_length );
    virtual void ParseAndSaveBasicCompleteResults( const uint8_t* buffer, uint16_t buffer_length );
    virtual void ParseAndSaveBasicMacChannelTypeResults( const uint8_t* buffer, uint16_t buffer_length );
    static void  AddScanToResults( const lr1110_modem_system_reg_mode_t regMode, demo_wifi_scan_all_results_t& results,
                                   const lr1110_modem_wifi_basic_complete_result_t* scan_result,
                                   const uint8_t                                    nbr_results );
    static void  AddScanToResults( const lr1110_modem_system_reg_mode_t regMode, demo_wifi_scan_all_results_t& results,
                                   const lr1110_modem_wifi_basic_mac_type_channel_result_t* scan_result,
                                   const uint8_t                                            nbr_results );

    /*!
     * \brief Compute consumption based on cumulative timings
     *
     * \return Consumption in micro ampere second (uas)
     */
    static uint32_t              ComputeConsumption( const lr1110_modem_system_reg_mode_t          regMode,
                                                     const lr1110_modem_wifi_cumulative_timings_t& timing );
    demo_wifi_scan_all_results_t results;

    static lr1110_modem_wifi_result_format_t ResultFormatFromSetting( demo_wifi_result_type_t result_setting );
    static demo_wifi_timings_t               demo_wifi_timing_from_modem(
                      const lr1110_modem_wifi_cumulative_timings_t& modem_timings );
    static lr1110_modem_wifi_mode_t             modem_wifi_mode_from_demo( const demo_wifi_mode_t& demo_wifi_mode );
    static lr1110_modem_wifi_signal_type_scan_t modem_wifi_scan_type_from_demo(
        const demo_wifi_signal_type_scan_t& demo_wifi_scan_type );
    static demo_wifi_signal_type_t demo_wifi_types_from_modem(
        lr1110_modem_wifi_signal_type_result_t wifi_type_result );

   private:
    demo_wifi_settings_t    settings;
    demo_modem_wifi_state_t state;
};

#endif  // __DEMO_MODEM_WIFI_H__