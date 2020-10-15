/**
 * @file      demo_modem_radio_converters.cpp
 *
 * @brief     Implementation of the modem radio convertion methods.
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

#include "demo_modem_radio_converters.h"

bool DemoModemRadioConverters::convert_radio_setting_sf( lr1110_radio_lora_sf_t      sf_setting,
                                                         lr1110_modem_tst_mode_sf_t* sf_test )
{
    bool success = false;
    switch( sf_setting )
    {
    case LR1110_RADIO_LORA_SF7:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF7;
        break;
    }
    case LR1110_RADIO_LORA_SF8:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF8;
        break;
    }
    case LR1110_RADIO_LORA_SF9:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF9;
        break;
    }
    case LR1110_RADIO_LORA_SF10:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF10;
        break;
    }
    case LR1110_RADIO_LORA_SF11:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF11;
        break;
    }
    case LR1110_RADIO_LORA_SF12:
    {
        success      = true;
        ( *sf_test ) = LR1110_MODEM_TST_MODE_SF12;
        break;
    }
    default:
    {
        success = false;
    }
    }
    return success;
}

bool DemoModemRadioConverters::convert_radio_setting_bw( lr1110_radio_lora_bw_t      bw_setting,
                                                         lr1110_modem_tst_mode_bw_t* bw_test )
{
    bool success = false;
    switch( bw_setting )
    {
    case LR1110_RADIO_LORA_BW_125:
    {
        success      = true;
        ( *bw_test ) = LR1110_MODEM_TST_MODE_125_KHZ;
        break;
    }
    case LR1110_RADIO_LORA_BW_250:
    {
        success      = true;
        ( *bw_test ) = LR1110_MODEM_TST_MODE_250_KHZ;
        break;
    }
    case LR1110_RADIO_LORA_BW_500:
    {
        success      = true;
        ( *bw_test ) = LR1110_MODEM_TST_MODE_500_KHZ;
        break;
    }
    default:
    {
        success = false;
    }
    }
    return success;
}

bool DemoModemRadioConverters::convert_radio_setting_cr( lr1110_radio_lora_cr_t      cr_setting,
                                                         lr1110_modem_tst_mode_cr_t* cr_test )
{
    bool success = false;
    switch( cr_setting )
    {
    case LR1110_RADIO_LORA_CR_4_5:
    {
        success      = true;
        ( *cr_test ) = LR1110_MODEM_TST_MODE_4_5;
        break;
    }
    case LR1110_RADIO_LORA_CR_4_6:
    {
        success      = true;
        ( *cr_test ) = LR1110_MODEM_TST_MODE_4_5;
        break;
    }
    case LR1110_RADIO_LORA_CR_4_7:
    {
        success      = true;
        ( *cr_test ) = LR1110_MODEM_TST_MODE_4_5;
        break;
    }
    case LR1110_RADIO_LORA_CR_4_8:
    {
        success      = true;
        ( *cr_test ) = LR1110_MODEM_TST_MODE_4_5;
        break;
    }
    default:
    {
        success = false;
    }
    }
    return success;
}
