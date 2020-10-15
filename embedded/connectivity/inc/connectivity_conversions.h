/**
 * @file      connectivity_conversions.h
 *
 * @brief     Definition of the ConnectivityConversions class.
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

#ifndef __CONNECTIVITY_CONVERSIONS_H__
#define __CONNECTIVITY_CONVERSIONS_H__

#include "demo_wifi_types.h"
#include "demo_gnss_types.h"

typedef enum
{
    CONNECTIVITY_CONVERSION_SUCCESS,
    CONNECTIVITY_CONVERSION_ERROR_BUFFER_TOO_SHORT,
} ConnectivityConversionStatus_t;

class ConnectivityConversions
{
   public:
    static ConnectivityConversionStatus_t copy_demo_result_to_tlv_payload_buffer(
        const demo_wifi_scan_all_results_t& wifi_results, uint8_t* buffer, uint16_t* buffer_size,
        uint16_t max_buffer_size );

    static ConnectivityConversionStatus_t copy_demo_result_to_tlv_payload_buffer(
        const demo_gnss_all_results_t& gnss_result, uint8_t* buffer, uint16_t* buffer_size, uint16_t max_buffer_size );

   protected:
    static uint16_t tlv_paylod_size_from_results( const demo_wifi_scan_all_results_t& wifi_results );
    static uint16_t tlv_paylod_size_from_results( const demo_gnss_all_results_t& gnss_results );
    static uint16_t tlv_value_field_length_from_results( const demo_wifi_scan_all_results_t& wifi_results );
    static uint16_t tlv_value_field_length_from_results( const demo_gnss_all_results_t& gnss_results );
};

#endif  // __CONNECTIVITY_CONVERSIONS_H__