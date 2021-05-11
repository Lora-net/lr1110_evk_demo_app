/**
 * @file      version.h
 *
 * @brief     Placeholder for LR1110 EVK version number.
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

#ifndef __VERSION_H__
#define __VERSION_H__

#include "version_file.h"

#define DEMO_VERSION VERSION

#define VERSION_STRING_LENGTH 10
#define EUI_LENGTH 8
#define PIN_LENGTH 4

typedef enum
{
    VERSION_DEVICE_TRANSCEIVER,
    VERSION_DEVICE_MODEM,
} version_device_type_t;

typedef struct
{
    version_device_type_t device_type;
    char                  version_sw[VERSION_STRING_LENGTH];
    char                  version_driver[VERSION_STRING_LENGTH];
    uint8_t               chip_uid[EUI_LENGTH];
    uint8_t               dev_eui[EUI_LENGTH];
    uint8_t               join_eui[EUI_LENGTH];
    uint8_t               pin[PIN_LENGTH];
    uint16_t              almanac_date;
    uint32_t              almanac_crc;
    union
    {
        struct
        {
            uint8_t  version_chip_type;
            uint8_t  version_chip_hw;
            uint16_t version_chip_fw;
        } transceiver;
        struct
        {
            uint8_t  version_chip_type;
            uint32_t version_chip_bootloader;
            uint32_t version_chip_fw;
        } modem;
    };
} version_handler_t;

#endif
