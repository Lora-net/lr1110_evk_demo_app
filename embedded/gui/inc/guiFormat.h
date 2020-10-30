/**
 * @file      guiFormat.h
 *
 * @brief     Definition of the gui format page.
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

#ifndef __GUI_FORMAT_H__
#define __GUI_FORMAT_H__

#include <string.h>
#include <stdint.h>

/*!
 * \brief C-string size to represent a WiFi MAC address
 *
 * 6 fields of two chars plus 5 ':' separator and \00 end character
 */
#define GUI_WIFI_STRING_LENGTH 18

#define GUI_WIFI_STRING_COUNTRY_CODE_LENGTH 3

#define GUI_MAX_WIFI_MAC_ADDRESSES 5
#define GUI_MAX_GNSS_SATELLITES 32

#define GUI_WIFI_CHANNELS 14

#define GUI_RESULT_GEO_LOC_STREET_LENGTH 32
#define GUI_RESULT_GEO_LOC_CITY_LENGTH 32
#define GUI_RESULT_GEO_LOC_COUNTRY_LENGTH 32
#define GUI_RESULT_GEO_LOC_LATITUDE_LENGTH 16
#define GUI_RESULT_GEO_LOC_LONGITUDE_LENGTH 16

typedef char GuiWifiMacAddress_t[GUI_WIFI_STRING_LENGTH];
typedef char GuiWifiCountryCode_t[GUI_WIFI_STRING_COUNTRY_CODE_LENGTH];

struct GuiResultGeoLoc_t
{
    char street[GUI_RESULT_GEO_LOC_STREET_LENGTH];
    char city[GUI_RESULT_GEO_LOC_CITY_LENGTH];
    char country[GUI_RESULT_GEO_LOC_COUNTRY_LENGTH];
    char latitude[GUI_RESULT_GEO_LOC_LATITUDE_LENGTH];
    char longitude[GUI_RESULT_GEO_LOC_LONGITUDE_LENGTH];
    GuiResultGeoLoc_t( )
    {
        memset( this->street, '\0', GUI_RESULT_GEO_LOC_STREET_LENGTH );
        memset( this->city, '\0', GUI_RESULT_GEO_LOC_CITY_LENGTH );
        memset( this->country, '\0', GUI_RESULT_GEO_LOC_COUNTRY_LENGTH );
        memset( this->latitude, '\0', GUI_RESULT_GEO_LOC_LATITUDE_LENGTH );
        memset( this->longitude, '\0', GUI_RESULT_GEO_LOC_LONGITUDE_LENGTH );
    }
};

typedef enum
{
    GUI_DEMO_STATUS_OK = 0,
    GUI_DEMO_STATUS_KO_HOST_NOT_CONNECTED,
    GUI_DEMO_STATUS_KO_ALMANAC_TOO_OLD,
    GUI_DEMO_STATUS_KO_NAV_MESSAGE_TOO_LONG,
    GUI_DEMO_STATUS_KO_UNKNOWN,
} GuiDemoStatus_t;

typedef enum
{
    GUI_CONNECTIVITY_STATUS_NOT_CONNECTED,
    GUI_CONNECTIVITY_STATUS_JOINING,
    GUI_CONNECTIVITY_STATUS_CONNECTED,
} GuiConnectivityState_t;

typedef struct
{
    GuiWifiMacAddress_t  macAddr;
    GuiWifiCountryCode_t countryCode;
    int8_t               rssi;
} GuiWifiResultData_t;

typedef struct
{
    GuiWifiResultData_t data[GUI_MAX_WIFI_MAC_ADDRESSES];
    uint8_t             nbMacAddr;
} GuiWifiResultChannel_t;

typedef struct
{
    GuiWifiResultChannel_t channel[GUI_WIFI_CHANNELS];
} GuiWifiResultType_t;

typedef struct
{
    GuiDemoStatus_t     status;
    GuiWifiResultType_t typeB;
    GuiWifiResultType_t typeG;
    uint8_t             nbMacAddrTotal;
    uint32_t            powerConsTotal;
    uint32_t            timingTotal;
    GuiResultGeoLoc_t   reverse_geo_loc;
    bool                error;
} GuiWifiResult_t;

typedef enum
{
    GUI_GNSS_CONSTELLATION_BEIDOU,
    GUI_GNSS_CONSTELLATION_GPS,
} gui_gnss_constellation_t;
typedef struct
{
    uint8_t                  satellite_id;
    int16_t                  snr;
    gui_gnss_constellation_t constellation;
} GuiGnssSatelliteResult_t;
typedef struct
{
    GuiDemoStatus_t          status;
    uint8_t                  nb_satellites;
    GuiGnssSatelliteResult_t satellite_ids[GUI_MAX_GNSS_SATELLITES];
    GuiResultGeoLoc_t        reverse_geo_loc;
    uint32_t                 powerConsTotal;
    uint32_t                 timingTotal;
    uint16_t                 almanac_age;
    bool                     almanac_too_old;
} GuiGnssResult_t;

typedef struct
{
    uint32_t count_rx_correct_packet;
    uint32_t count_rx_wrong_packet;
    uint32_t count_tx;
    uint32_t count_rx_timeout;
} GuiRadioPingPongResult_t;

typedef struct
{
    uint32_t count_rx_correct_packet;
    uint32_t count_rx_wrong_packet;
    uint32_t count_tx;
    uint32_t count_rx_timeout;
} GuiRadioPerResult_t;

typedef struct
{
    GuiRadioPerResult_t      radio_per_result;
    GuiRadioPingPongResult_t radio_pingpong_result;
    GuiWifiResult_t          wifi_result;
    GuiGnssResult_t          gnss_result;
} GuiDemoResult_t;

typedef struct
{
    uint16_t channel_mask;
    bool     is_type_b;
    bool     is_type_g;
    bool     is_type_n;
    bool     is_type_all;
} GuiWifiDemoSetting_t;

typedef struct
{
    bool is_gps_enabled;
    bool is_beidou_enabled;
    bool is_dual_scan_activated;
    bool is_best_effort_activated;
} GuiGnssDemoSetting_t;

typedef struct
{
    float latitude;
    float longitude;
    bool  set_by_network;
} GuiGnssDemoAssistancePosition_t;

typedef struct
{
    uint8_t  sf;
    uint16_t bw;
    uint8_t  cr;
    bool     is_iq_inverted;
    bool     is_crc_activated;
    bool     is_hdr_implicit;
} GuiRadioSettingLora_t;

typedef struct
{
    uint32_t br_in_bps;
    uint32_t fdev_in_hz;
    uint8_t  crc;
    bool     is_dcfree_enabled;
    bool     is_hdr_implicit;
} GuiRadioSettingGfsk_t;

typedef struct
{
    uint32_t              rf_freq_in_hz;
    int16_t               pwr_in_dbm;
    uint16_t              nb_of_packets;
    uint16_t              payload_length;
    bool                  is_hp_pa_enabled;
    bool                  is_lora;
    GuiRadioSettingLora_t lora;
    GuiRadioSettingGfsk_t gfsk;
} GuiRadioSetting_t;

typedef struct
{
    GuiRadioSetting_t    radio_settings;
    GuiWifiDemoSetting_t wifi_settings;
    GuiGnssDemoSetting_t gnss_autonomous_settings;
    GuiGnssDemoSetting_t gnss_assisted_settings;
} GuiDemoSettings_t;

typedef enum
{
    GUI_NETWORK_CONNECTIVITY_REGION_EU868,
    GUI_NETWORK_CONNECTIVITY_REGION_US915,
} GuiNetworkConnectivityLorawanRegion_t;

typedef enum
{
    GUI_NETWORK_CONNECTIVITY_ADR_NETWORK_SERVER_CONTROLLED,
    GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LONG_RANGE,
    GUI_NETWORK_CONNECTIVITY_ADR_MOBILE_LOW_POWER,
} GuiNetworkConnectivityAdrProfile_t;

typedef struct
{
    GuiNetworkConnectivityLorawanRegion_t region;
    GuiNetworkConnectivityAdrProfile_t    adr_profile;
} GuiNetworkConnectivitySettings_t;

typedef struct
{
    GuiConnectivityState_t connectivity_state;
    bool                   is_time_sync;
} GuiNetworkConnectivityStatus_t;

#endif
