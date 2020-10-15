#ifndef __DEMO_WIFI_TYPES_H__
#define __DEMO_WIFI_TYPES_H__

#include <stdint.h>

#define DEMO_WIFI_STR_COUNTRY_CODE_SIZE ( 2 )
#define DEMO_WIFI_MAX_RESULT_TOTAL ( 32 )
#define DEMO_TYPE_WIFI_MAC_ADDRESS_LENGTH ( 6 )

typedef struct DemoWifiTimings
{
    uint32_t rx_detection_us;
    uint32_t rx_correlation_us;
    uint32_t rx_capture_us;
    uint32_t demodulation_us;
} demo_wifi_timings_t;

typedef uint8_t demo_wifi_mac_address_t[DEMO_TYPE_WIFI_MAC_ADDRESS_LENGTH];
typedef uint8_t demo_wifi_channel_t;
typedef enum
{
    DEMO_WIFI_TYPE_B,  //!< WiFi B
    DEMO_WIFI_TYPE_G,  //!< WiFi G
    DEMO_WIFI_TYPE_N,  //!< WiFi N
} demo_wifi_signal_type_t;

typedef struct
{
    demo_wifi_mac_address_t mac_address;
    demo_wifi_channel_t     channel;
    demo_wifi_signal_type_t type;
    int8_t                  rssi;
    uint8_t                 country_code[DEMO_WIFI_STR_COUNTRY_CODE_SIZE];
} demo_wifi_scan_single_result_t;

typedef struct
{
    uint8_t                        nbrResults;
    demo_wifi_scan_single_result_t results[DEMO_WIFI_MAX_RESULT_TOTAL];
    demo_wifi_timings_t            timings;
    uint32_t                       global_consumption_uas;
    bool                           error;
} demo_wifi_scan_all_results_t;

typedef enum
{
    DEMO_WIFI_RESULT_TYPE_BASIC_COMPLETE,
    DEMO_WIFI_RESULT_TYPE_BASIC_MAC_TYPE_CHANNEL,
} demo_wifi_result_type_t;

typedef uint16_t demo_wifi_channel_mask_t;

typedef enum
{
    DEMO_WIFI_SETTING_TYPE_B,
    DEMO_WIFI_SETTING_TYPE_G,
    DEMO_WIFI_SETTING_TYPE_N,
    DEMO_WIFI_SETTING_TYPE_B_G_N,
} demo_wifi_signal_type_scan_t;

typedef enum
{
    DEMO_WIFI_SCAN_MODE_BEACON,
    DEMO_WIFI_SCAN_MODE_BEACON_AND_PACKET,
} demo_wifi_mode_t;

typedef struct
{
    demo_wifi_channel_mask_t     channels;
    demo_wifi_signal_type_scan_t types;
    demo_wifi_mode_t             scan_mode;
    uint8_t                      nbr_retrials;
    uint8_t                      max_results;
    uint16_t                     timeout;
    demo_wifi_result_type_t      result_type;
} demo_wifi_settings_t;

typedef struct
{
    demo_wifi_channel_mask_t channels;
    uint8_t                  nbr_retrials;
    uint8_t                  max_results;
    uint32_t                 timeout;
    bool                     does_abort_on_timeout;
} demo_wifi_country_code_settings_t;

#endif  // __DEMO_WIFI_TYPES_H__