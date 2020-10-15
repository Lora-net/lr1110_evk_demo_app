/**
 * @file      demo_configuration.h
 *
 * @brief     Demonstration configurations for GNSS and Wi-Fi captures.
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

#ifndef __DEMO_CONFIGURATION_H__
#define __DEMO_CONFIGURATION_H__

#include "lr1110_wifi.h"
#include "lr1110_radio_types.h"
#include "lr1110_modem_lorawan.h"
#include "demo_wifi_types.h"
#include "demo_gnss_types.h"

typedef enum
{
    DEMO_SYSTEM_RFSW0_HIGH = ( 1 << 0 ),
    DEMO_SYSTEM_RFSW1_HIGH = ( 1 << 1 ),
    DEMO_SYSTEM_RFSW2_HIGH = ( 1 << 2 ),
    DEMO_SYSTEM_RFSW3_HIGH = ( 1 << 3 ),
    DEMO_SYSTEM_RFSW4_HIGH = ( 1 << 4 ),
} demo_system_rf_switch_t;

#define DEMO_ASSISTANCE_LOCATION_LATITUDE ( 45.1 )
#define DEMO_ASSISTANCE_LOCATION_LONGITUDE ( 5.7 )
#define DEMO_ASSISTANCE_LOCATION_ALTITUDE ( 100 )

#define DEMO_COMMON_RF_SWITCH_ENABLE \
    ( DEMO_SYSTEM_RFSW0_HIGH | DEMO_SYSTEM_RFSW1_HIGH | DEMO_SYSTEM_RFSW2_HIGH | DEMO_SYSTEM_RFSW3_HIGH )
#define DEMO_COMMON_RF_SWITCH_STANDBY 0x00
#define DEMO_COMMON_RF_SWITCH_RX ( DEMO_SYSTEM_RFSW0_HIGH )
#define DEMO_COMMON_RF_SWITCH_TX ( DEMO_SYSTEM_RFSW0_HIGH | DEMO_SYSTEM_RFSW1_HIGH )
#define DEMO_COMMON_RF_SWITCH_TX_HP ( DEMO_SYSTEM_RFSW1_HIGH )
#define DEMO_COMMON_RF_SWITCH_WIFI ( DEMO_SYSTEM_RFSW3_HIGH )
#define DEMO_COMMON_RF_SWITCH_GNSS ( DEMO_SYSTEM_RFSW2_HIGH )

#define DEMO_WIFI_CHANNELS_DEFAULT \
    ( ( 1 << LR1110_WIFI_CHANNEL_11 ) + ( 1 << LR1110_WIFI_CHANNEL_6 ) + ( 1 << LR1110_WIFI_CHANNEL_1 ) )
#define DEMO_WIFI_TYPE_SCAN_DEFAULT DEMO_WIFI_SETTING_TYPE_B
#define DEMO_WIFI_MODE_DEFAULT DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET
#define DEMO_WIFI_NBR_RETRIALS_DEFAULT 5
#define DEMO_WIFI_MAX_RESULTS_DEFAULT 15
#define DEMO_WIFI_TIMEOUT_IN_MS_DEFAULT 110
#define DEMO_WIFI_RESULT_TYPE_DEFAULT ( DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE )
#define DEMO_WIFI_DOES_ABORT_ON_TIMEOUT_DEFAULT ( false )

#define DEMO_GNSS_AUTONOMOUS_OPTION_DEFAULT ( DEMO_GNSS_OPTION_DEFAULT )
#define DEMO_GNSS_AUTONOMOUS_CAPTURE_MODE_DEFAULT ( DEMO_GNSS_SINGLE_SCAN_MODE )
#define DEMO_GNSS_AUTONOMOUS_N_SATELLLITE_DEFAULT ( 0 )
#define DEMO_GNSS_AUTONOMOUS_ANTENNA_SELECTION_DEFAULT ( DEMO_GNSS_NO_ANTENNA_SELECTION )
#define DEMO_GNSS_AUTONOMOUS_CONSTELLATION_MASK_DEFAULT ( DEMO_GNSS_GPS_MASK | DEMO_GNSS_BEIDOU_MASK )

#define DEMO_GNSS_ASSISTED_OPTION_DEFAULT ( DEMO_GNSS_OPTION_DEFAULT )
#define DEMO_GNSS_ASSISTED_CAPTURE_MODE_DEFAULT ( DEMO_GNSS_SINGLE_SCAN_MODE )
#define DEMO_GNSS_ASSISTED_N_SATELLLITE_DEFAULT ( 0 )
#define DEMO_GNSS_ASSISTED_ANTENNA_SELECTION_DEFAULT ( DEMO_GNSS_NO_ANTENNA_SELECTION )
#define DEMO_GNSS_ASSISTED_CONSTELLATION_MASK_DEFAULT ( DEMO_GNSS_GPS_MASK | DEMO_GNSS_BEIDOU_MASK )

#define DEMO_RADIO_RF_FREQUENCY_DEFAULT ( 868200000 )
#define DEMO_RADIO_TX_POWER_DEFAULT ( 14 )
#define DEMO_RADIO_PAYLOAD_LENGTH_DEFAULT ( 20 )
#define DEMO_RADIO_NB_OF_PACKET_DEFAULT ( 10 )
#define DEMO_RADIO_PA_RAMP_TIME_DEFAULT ( LR1110_RADIO_RAMP_240_US )
#define DEMO_RADIO_PA_DUTY_CYCLE_DEFAULT ( 4 )
#define DEMO_RADIO_PA_HP_SEL_DEFAULT ( 0 )
#define DEMO_RADIO_PA_REG_SUPPLY_DEFAULT ( LR1110_RADIO_PA_REG_SUPPLY_VREG )
#define DEMO_RADIO_PA_SEL_DEFAULT ( LR1110_RADIO_PA_SEL_LP )
#define DEMO_RADIO_PACKET_TYPE_DEFAULT ( LR1110_RADIO_PKT_TYPE_LORA )
#define DEMO_RADIO_LORA_BW_DEFAULT ( LR1110_RADIO_LORA_BW_250 )
#define DEMO_RADIO_LORA_SF_DEFAULT ( LR1110_RADIO_LORA_SF7 )
#define DEMO_RADIO_LORA_CR_DEFAULT ( LR1110_RADIO_LORA_CR_4_5 )
#define DEMO_RADIO_LORA_IQ_DEFAULT ( LR1110_RADIO_LORA_IQ_STANDARD )
#define DEMO_RADIO_LORA_CRC_DEFAULT ( LR1110_RADIO_LORA_CRC_ON )
#define DEMO_RADIO_LORA_HDR_DEFAULT ( LR1110_RADIO_LORA_PKT_EXPLICIT )
#define DEMO_RADIO_GFSK_BITRATE_DEFAULT ( 150000 )
#define DEMO_RADIO_GFSK_FDEV_DEFAULT ( 50000 )
#define DEMO_RADIO_GFSK_MODSHAPE_DEFAULT ( LR1110_RADIO_GFSK_PULSE_SHAPE_BT_05 )
#define DEMO_RADIO_GFSK_RX_BW_DEFAULT ( LR1110_RADIO_GFSK_BW_312000 )
#define DEMO_RADIO_GFSK_ADR_FILTERING_DEFAULT ( LR1110_RADIO_GFSK_ADDRESS_FILTERING_DISABLE )
#define DEMO_RADIO_GFSK_CRC_DEFAULT ( LR1110_RADIO_GFSK_CRC_2_BYTES )
#define DEMO_RADIO_GFSK_DC_FREE_DEFAULT ( LR1110_RADIO_GFSK_DC_FREE_OFF )
#define DEMO_RADIO_GFSK_HDR_DEFAULT ( LR1110_RADIO_GFSK_PKT_VAR_LEN )
#define DEMO_RADIO_GFSK_PBL_DETECT_DEFAULT ( LR1110_RADIO_GFSK_PREAMBLE_DETECTOR_MIN_8BITS )
#define DEMO_RADIO_GFSK_PBL_LENGTH_DEFAULT ( 16 )
#define DEMO_RADIO_GFSK_SW_LENGTH_DEFAULT ( 24 )

#define DEMO_PING_PONG_RX_TIMEOUT_DEFAULT ( 0xFFFFFFFF )
#define DEMO_PING_PONG_TX_TIMEOUT_DEFAULT ( 0xFFFFFFFF )

typedef struct
{
    uint32_t                       rf_frequency;
    lr1110_radio_pa_cfg_t          pa_configuration;
    int8_t                         tx_power;
    uint32_t                       nb_of_packets;
    uint8_t                        payload_length;
    lr1110_radio_ramp_time_t       pa_ramp_time;
    lr1110_radio_pkt_type_t        pkt_type;
    lr1110_radio_mod_params_gfsk_t modulation_gfsk;
    lr1110_radio_mod_params_lora_t modulation_lora;
    lr1110_radio_pkt_params_gfsk_t packet_gfsk;
    lr1110_radio_pkt_params_lora_t packet_lora;
} demo_radio_settings_t;

typedef struct
{
    demo_wifi_settings_t              wifi_settings;
    demo_wifi_country_code_settings_t wifi_country_code_settings;
    demo_gnss_settings_t              gnss_autonomous_settings;
    demo_gnss_settings_t              gnss_assisted_settings;
    demo_radio_settings_t             radio_settings;
} demo_all_settings_t;
#endif
