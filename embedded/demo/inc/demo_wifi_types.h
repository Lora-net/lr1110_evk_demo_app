#ifndef __DEMO_WIFI_TYPES_H__
#define __DEMO_WIFI_TYPES_H__

#include "lr1110_wifi_types.h"

#define DEMO_WIFI_MAX_RESULT_TOTAL 32
#define DEMO_TYPE_WIFI_MAC_ADDRESS_LENGTH ( 6 )

typedef struct DemoWifiTimings
{
    DemoWifiTimings( ) : rx_detection_us( 0 ), rx_correlation_us( 0 ), rx_capture_us( 0 ), demodulation_us( 0 ) {}
    DemoWifiTimings( const lr1110_wifi_cumulative_timings_t& transceiver_timings )
        : rx_detection_us( transceiver_timings.rx_detection_us ),
          rx_correlation_us( transceiver_timings.rx_correlation_us ),
          rx_capture_us( transceiver_timings.rx_capture_us ),
          demodulation_us( transceiver_timings.demodulation_us )
    {
    }

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

demo_wifi_signal_type_t demo_wifi_types_from_transceiver( lr1110_wifi_signal_type_result_t wifi_type_result );

typedef struct
{
    demo_wifi_mac_address_t mac_address;
    demo_wifi_channel_t     channel;
    demo_wifi_signal_type_t type;
    int8_t                  rssi;
    uint8_t                 country_code[LR1110_WIFI_STR_COUNTRY_CODE_SIZE];
} demo_wifi_scan_single_result_t;

typedef struct
{
    uint8_t                        nbrResults;
    demo_wifi_scan_single_result_t results[DEMO_WIFI_MAX_RESULT_TOTAL];
    demo_wifi_timings_t            timings;
    uint32_t                       global_consumption_uas;
    bool                           error;
} demo_wifi_scan_all_results_t;

#endif  // __DEMO_WIFI_TYPES_H__