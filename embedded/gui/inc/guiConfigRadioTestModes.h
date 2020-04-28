/**
 * @file      guiConfigRadioTestModes.h
 *
 * @brief     Definition of the gui radio test mode configuration page.
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

#ifndef __GUI_CONFIG_RADIO_TEST_MODES_H__
#define __GUI_CONFIG_RADIO_TEST_MODES_H__

#include "guiCommon.h"

class GuiConfigRadioTestModes : public GuiCommon
{
   public:
    GuiConfigRadioTestModes( GuiRadioSetting_t* settings_current, const GuiRadioSetting_t* settings_default );
    virtual ~GuiConfigRadioTestModes( );

    virtual void init( );
    virtual void draw( );

    void ConfigParamGeneric( );
    void ConfigParamLora( );
    void ConfigParamGfsk( );

    void ConfigActionButton( );
    bool IsConfigTempEqualTo( const GuiRadioSetting_t* settings_to_compare ) const;

    static void callback( lv_obj_t* obj, lv_event_t event );
    static void callback_sw( lv_obj_t* obj, lv_event_t event );
    static void callback_ta( lv_obj_t* obj, lv_event_t event );
    static void callback_kb( lv_obj_t* obj, lv_event_t event );
    static void callback_ddlist( lv_obj_t* obj, lv_event_t event );

   private:
    void create_ddlist( lv_obj_t** ddlist, lv_obj_t* screen, int16_t off_y, const char* lbl_name, const char* options,
                        lv_event_cb_t event_cb );
    void create_ta( lv_obj_t** ta, lv_obj_t* screen, int16_t off_y, const char* lbl_name, uint16_t max_length,
                    const char* init_text, lv_event_cb_t event_cb );

    lv_obj_t* tabview;
    lv_obj_t* tab_generic;
    lv_obj_t* tab_lora;
    lv_obj_t* tab_gfsk;
    lv_obj_t* sw_pkt_type;
    lv_obj_t* sw_pa;
    lv_obj_t* ta_freq;
    lv_obj_t* ta_pl_len;
    lv_obj_t* ta_pkt;
    lv_obj_t* ddlist_pwr;
    lv_obj_t* lora_ddlist_sf;
    lv_obj_t* lora_ddlist_bw;
    lv_obj_t* lora_ddlist_cr;
    lv_obj_t* lora_ddlist_iq;
    lv_obj_t* lora_ddlist_crc;
    lv_obj_t* lora_ddlist_hdr;
    lv_obj_t* gfsk_ta_br;
    lv_obj_t* gfsk_ta_fdev;
    lv_obj_t* gfsk_ddlist_hdr;
    lv_obj_t* gfsk_ddlist_crc;
    lv_obj_t* gfsk_ddlist_dcfree;
    lv_obj_t* kb_num;
    lv_obj_t* btn_cancel;
    lv_obj_t* btn_default;
    lv_obj_t* btn_save;

    char ta_text[10];

    const char lp_pa_options[9]  = "10\n14\n15";
    const char hp_pa_options[12] = "14\n17\n20\n22";

    GuiRadioSetting_t        settings_temp;
    GuiRadioSetting_t*       settings_current;
    const GuiRadioSetting_t* settings_default;
};

#endif  // __GUI_CONFIG_RADIO_TEST_MODES_H__
