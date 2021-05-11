/*!
 * @file      lr1110_modem_helper.h
 *
 * @brief     helper functions definition for LR1110 modem
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

#ifndef LR1110_MODEM_HELPER_H
#define LR1110_MODEM_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdbool.h>
#include <stdint.h>
#include "lr1110_modem_common.h"
#include "lr1110_modem_gnss.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*!
 * @brief Initial GNSS date in second, 6 January 1980
 */
#define LR1110_MODEM_HELPER_INITAL_GNSS_DATE_SECONDS 315964800

/*!
 * @brief GNSS leap seconds offset in second
 */
#define LR1110_MODEM_HELPER_GNSS_LEAP_SECONDS_OFFSET 18

/*!
 * @brief GPS week number rollover
 */
#define LR1110_MODEM_HELPER_NB_WEEK_PER_WRAP 1024

/*!
 * @brief Number of seconds in per hour
 */
#define LR1110_MODEM_HELPER_NB_SEC_PER_HOUR 3600

/*!
 * @brief Number of hours per day
 */
#define LR1110_MODEM_HELPER_NB_HOUR_PER_DAY 24

/*!
 * @brief Number of days per week
 */
#define LR1110_MODEM_HELPER_NB_DAY_PER_WEEK 7

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/**
 * @brief LR1110 Modem helper status
 */
typedef enum
{
    LR1110_MODEM_HELPER_STATUS_OK    = 0,
    LR1110_MODEM_HELPER_STATUS_ERROR = 3,
} lr1110_modem_helper_status_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/*!
 * @brief Query the current UTC time. \note the conversion is based on the Modem-E GPS date, The application
 * layer clock synchronization shall be received before call this function.
 *
 * @param [in] context Chip implementation context
 * @param [out] time UTC Time in seconds
 *
 * @returns Operation status
 */
lr1110_modem_response_code_t lr1110_modem_helper_get_utc_time( const void* context, uint32_t* time );

/*!
 * @brief Compute the Gnss Week Number Rollover \note the computation is based on the Modem-E GPS date, The application
 * layer clock synchronization shall be received before call this function.
 *
 * @param [in] context Chip implementation context
 * @param [out] gnss_week_number_rollover Number of GPS Week Number Rollover which is a phenomenon that happens every
 * 1024 weeks
 *
 * @returns Operation status
 */
lr1110_modem_response_code_t lr1110_modem_helper_gnss_get_gnss_week_number_rollover(
    const void* context, uint8_t* gnss_week_number_rollover );

/*!
 * @brief Read the almanac date by SV index.
 *
 * @param [in] context Chip implementation context
 * @param [in] sv_id Satellite ID
 * @param [out] date Almanac date UTC format in second \note if the returned date is 0 it means that the SV is not
 * activated
 * @param [in] gnss_week_number_rollover Number of GPS Week Number Rollover which is a phenomenon that happens every
 * 1024 weeks
 *
 * @returns Operation status
 */
lr1110_modem_response_code_t lr1110_modem_helper_gnss_get_almanac_date_by_index( const void* context, uint8_t sv_id,
                                                                                 uint32_t* date,
                                                                                 uint8_t   gnss_week_number_rollover );

/**
 * @brief Extract the destination from the result returned by a GNSS scan
 *
 * @param [in]  result_buffer       Pointer to the buffer holding the result
 * @param [in]  result_buffer_size  Size of the result in byte
 * @param [out] destination         Destination of the result \see lr1110_modem_gnss_destination_t
 *
 * @returns  Operation status
 */
lr1110_modem_helper_status_t lr1110_modem_helper_gnss_get_result_destination(
    const uint8_t* result_buffer, const uint16_t result_buffer_size, lr1110_modem_gnss_destination_t* destination );

/**
 * @brief Extract the event type from the result returned by a GNSS scan, \note a event type exists only if the
 * destination is LR1110_MODEM_GNSS_DESTINATION_HOST
 *
 * @param [in]  result_buffer       Pointer to the buffer holding the result
 * @param [in]  result_buffer_size  Size of the result in byte
 * @param [out] event_type          Event type of the result \see lr1110_modem_gnss_scan_done_event_t
 *
 * @returns  Operation status
 */
lr1110_modem_helper_status_t lr1110_modem_helper_gnss_get_event_type( const uint8_t* result_buffer,
                                                                      const uint16_t result_buffer_size,
                                                                      lr1110_modem_gnss_scan_done_event_t* event_type );

#ifdef __cplusplus
}
#endif

#endif  // LR1110_MODEM_HELPER_H

/* --- EOF ------------------------------------------------------------------ */
