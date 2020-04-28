/**
 * @file      com_code.h
 *
 * @brief     Definitions of the command codes.
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

#ifndef __COM_CODE_H__
#define __COM_CODE_H__

#define COM_CODE_STATUS ( 0 )
#define COM_CODE_START ( 1 )
#define COM_CODE_CONFIGURE ( 2 )
#define COM_CODE_FETCH_RESULT ( 3 )
#define COM_CODE_RESET ( 4 )
#define COM_CODE_SET_DATE_LOC ( 5 )
#define COM_CODE_GET_VERSION ( 6 )
#define COM_CODE_GET_ALMANAC_DATES ( 7 )
#define COM_CODE_UPDATE_ALMANAC ( 8 )
#define COM_CODE_CHECK_ALMANAC_UPDATE ( 9 )

#define RESP_CODE_EVENT ( 0x80 )
#define RESP_CODE_WIFI_RESULT ( 0x81 )
#define RESP_CODE_GNSS_AUTONOMOUS_RESULT ( 0x82 )
#define RESP_CODE_GNSS_ASSISTED_RESULT ( 0x83 )
#define LOG_RESPONSE_CODE ( 0x84 )
#define ERROR_CODE_EVENT ( 0x90 )

#endif  // __COM_CODE_H__
