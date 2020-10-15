#ifndef __DEMO_GNSS_TYPES_H__
#define __DEMO_GNSS_TYPES_H__

#include <stdint.h>

#define GNSS_DEMO_MAX_RESULT_TOTAL ( 32 )
#define GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH ( 259 )

typedef enum
{
    DEMO_GNSS_CONSTELLATION_BEIDOU,
    DEMO_GNSS_CONSTELLATION_GPS,
} demo_gnss_constellation_t;

typedef struct
{
    demo_gnss_constellation_t constellation;
    uint8_t                   satellite_id;
    int16_t                   snr;
} demo_gnss_single_result_t;

typedef struct
{
    uint32_t radio_ms;
    uint32_t computation_ms;
} demo_gnss_timings_t;

typedef struct
{
    uint8_t  message[GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH];
    uint16_t size;
} demo_gnss_nav_result_t;

typedef enum
{
    DEMO_GNSS_BASE_NO_ERROR,
    DEMO_GNSS_BASE_ERROR_NO_DATE,
    DEMO_GNSS_BASE_ERROR_NO_LOCATION,
    DEMO_GNSS_BASE_ERROR_NO_SATELLITE,
    DEMO_GNSS_BASE_ERROR_ALMANAC_TOO_OLD,
    DEMO_GNSS_BASE_NAV_MESSAGE_TOO_LONG,
    DEMO_GNSS_BASE_ERROR_UNKNOWN,
} demo_gnss_error_t;

typedef struct
{
    demo_gnss_error_t         error;
    uint8_t                   nb_result;
    uint32_t                  consumption_uas;
    demo_gnss_timings_t       timings;
    demo_gnss_single_result_t result[GNSS_DEMO_MAX_RESULT_TOTAL];
    demo_gnss_nav_result_t    nav_message;
    uint32_t                  local_instant_measurement;
    uint32_t                  local_instant_measurement_second_capture;
    uint16_t                  almanac_age_days;
    bool                      almanac_too_old;
} demo_gnss_all_results_t;

typedef enum
{
    DEMO_GNSS_NO_ANTENNA_SELECTION = 0,
    DEMO_GNSS_ANTENNA_SELECTION_1  = 1,
    DEMO_GNSS_ANTENNA_SELECTION_2  = 2,
} demo_gnss_antenna_selection_t;

typedef enum
{
    DEMO_GNSS_GPS_MASK    = 0x01,
    DEMO_GNSS_BEIDOU_MASK = 0x02,
} demo_gnss_constellation_mask_t;

typedef enum
{
    DEMO_GNSS_OPTION_DEFAULT,
    DEMO_GNSS_OPTION_BEST_EFFORT,
} demo_gnss_search_mode_t;

typedef enum
{
    DEMO_GNSS_SINGLE_SCAN_MODE,
    DEMO_GNSS_DOUBLE_SCAN_MODE,
} demo_gnss_scan_mode_t;

typedef struct
{
    demo_gnss_search_mode_t       option;
    demo_gnss_scan_mode_t         capture_mode;
    uint8_t                       nb_satellites;
    demo_gnss_antenna_selection_t antenna_selection;
    uint8_t                       constellation_mask;
} demo_gnss_settings_t;

#endif  // __DEMO_GNSS_TYPES_H__