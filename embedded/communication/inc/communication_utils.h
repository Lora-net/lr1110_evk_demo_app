/**
 * @file      communication_utils.h
 *
 * @brief     Definition of the communication utils.
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

#ifndef __COMMUNICATION_UTILS_H__
#define __COMMUNICATION_UTILS_H__

#include <stdint.h>
#include "system_time.h"
#include "system_uart.h"

typedef enum
{
    COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OK,
    COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_TIMEOUT,
    COMMUNICATION_UTILS_RECEPTION_UTILS_STATUS_OVERFLOW,
} CommunicationUtilsReceptionStatus_t;

CommunicationUtilsReceptionStatus_t CommunicationUtilsReceiveData( const uint16_t buffer_length_max, char* buffer,
                                                                   uint16_t&      buffer_length_received,
                                                                   const uint16_t timeout );

#endif  // __COMMUNICATION_UTILS_H__