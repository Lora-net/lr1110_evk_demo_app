/**
 * @file      connectivity_conversions.cpp
 *
 * @brief     Implementation of the ConnectivityConversions class.
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

#include <string.h>
#include "connectivity_conversions.h"

#define CONNECTIVITY_CONVERSIONS_WIFI_SINGLE_VALUE_LENGTH ( 7 )
#define CONNECTIVITY_CONVERSIONS_TLV_LENGTH_TAG_FIELD ( 1 )
#define CONNECTIVITY_CONVERSIONS_TLV_LENGTH_LEN_FIELD ( 1 )
#define CONNECTIVITY_CONVERSIONS_TLV_TAG_WIFI ( 0x08 )
#define CONNECTIVITY_CONVERSIONS_TLV_TAG_GNSS ( 0x07 )

ConnectivityConversionStatus_t ConnectivityConversions::copy_demo_result_to_tlv_payload_buffer(
    const demo_wifi_scan_all_results_t& wifi_results, uint8_t* buffer, uint16_t* buffer_size, uint16_t max_buffer_size )
{
    const uint16_t length_value_field = ConnectivityConversions::tlv_value_field_length_from_results( wifi_results );
    const uint16_t local_buffer_size  = length_value_field + CONNECTIVITY_CONVERSIONS_TLV_LENGTH_TAG_FIELD +
                                       CONNECTIVITY_CONVERSIONS_TLV_LENGTH_LEN_FIELD;
    // 0. Check the buffer is long enough to contain the TLV
    if( max_buffer_size < local_buffer_size )
    {
        return CONNECTIVITY_CONVERSION_ERROR_BUFFER_TOO_SHORT;
    }

    *buffer_size = local_buffer_size;

    // 1. Write the tag and length fields
    buffer[0] = CONNECTIVITY_CONVERSIONS_TLV_TAG_WIFI;
    buffer[1] = length_value_field;

    // 2. Write the value field
    for( uint8_t index_wifi_result = 0; index_wifi_result < wifi_results.nbrResults; index_wifi_result++ )
    {
        const uint16_t local_buffer_offset = 2 + index_wifi_result * CONNECTIVITY_CONVERSIONS_WIFI_SINGLE_VALUE_LENGTH;
        const demo_wifi_scan_single_result_t local_wifi_result = wifi_results.results[index_wifi_result];
        buffer[local_buffer_offset]                            = local_wifi_result.rssi;
        memcpy( &buffer[local_buffer_offset + 1], local_wifi_result.mac_address, DEMO_TYPE_WIFI_MAC_ADDRESS_LENGTH );
    }
    return CONNECTIVITY_CONVERSION_SUCCESS;
}

ConnectivityConversionStatus_t ConnectivityConversions::copy_demo_result_to_tlv_payload_buffer(
    const demo_gnss_all_results_t& gnss_result, uint8_t* buffer, uint16_t* buffer_size, uint16_t max_buffer_size )
{
    const uint16_t length_value_field = ConnectivityConversions::tlv_value_field_length_from_results( gnss_result );
    const uint16_t local_buffer_size  = length_value_field + CONNECTIVITY_CONVERSIONS_TLV_LENGTH_TAG_FIELD +
                                       CONNECTIVITY_CONVERSIONS_TLV_LENGTH_LEN_FIELD;
    // 0. Check the buffer is long enough to contain the TLV
    if( max_buffer_size < local_buffer_size )
    {
        return CONNECTIVITY_CONVERSION_ERROR_BUFFER_TOO_SHORT;
    }

    *buffer_size = local_buffer_size;
    // 1. Write the tag and length fields
    buffer[0] = CONNECTIVITY_CONVERSIONS_TLV_TAG_GNSS;
    buffer[1] = length_value_field;

    // 2. Write the value field. WARNING: do not copy the first byte of the nav message in the buffer
    memcpy( &buffer[2], &gnss_result.nav_message.message[1], gnss_result.nav_message.size - 1 );
    return CONNECTIVITY_CONVERSION_SUCCESS;
}

uint16_t ConnectivityConversions::tlv_value_field_length_from_results(
    const demo_wifi_scan_all_results_t& wifi_results )
{
    return wifi_results.nbrResults * CONNECTIVITY_CONVERSIONS_WIFI_SINGLE_VALUE_LENGTH;
}

uint16_t ConnectivityConversions::tlv_value_field_length_from_results( const demo_gnss_all_results_t& gnss_results )
{
    return gnss_results.nav_message.size - 1;
}
