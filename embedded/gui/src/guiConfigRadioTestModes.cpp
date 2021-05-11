/**
 * @file      guiConfigRadioTestModes.cpp
 *
 * @brief     Implementation of the gui configuration radio test modes page.
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

#include "guiConfigRadioTestModes.h"
#include "stdlib.h"

#define DDLIST_WIDTH 95
#define TMP_BUFFER_CONFIG_PARAM_GENERIC_LENGTH ( 10 )
#define TMP_BUFFER_CONFIG_PARAM_GFSK_LENGTH ( 7 )
#define TMP_BUFFER_CALLBACK_LENGTH ( 10 )

GuiConfigRadioTestModes::GuiConfigRadioTestModes( GuiRadioSetting_t*       settings_current,
                                                  const GuiRadioSetting_t* settings_default,
                                                  version_handler_t*       version_handler )
    : GuiCommon( GUI_PAGE_RADIO_TEST_MODES_CONFIG ),
      settings_current( settings_current ),
      settings_default( settings_default ),
      device_type( version_handler->device_type )
{
    this->settings_temp = *( this->settings_current );

    this->createHeaderWithIcons( "RADIO CONFIGURATION" );

    /* Create a Tab view object and configure it */
    this->tabview = lv_tabview_create( this->screen, NULL );
    lv_tabview_set_style( this->tabview, LV_TABVIEW_STYLE_BG, &( GuiCommon::screen_style ) );
    lv_tabview_set_style( this->tabview, LV_TABVIEW_STYLE_BTN_REL, &( GuiCommon::tab ) );
    lv_tabview_set_anim_time( this->tabview, 0 );
    lv_tabview_set_sliding( this->tabview, false );

    this->tab_generic = lv_tabview_add_tab( this->tabview, "Generic" );
    this->tab_lora    = lv_tabview_add_tab( this->tabview, "LoRa" );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        this->tab_gfsk = lv_tabview_add_tab( this->tabview, "GFSK" );
    }

    this->create_ta( &( this->ta_freq ), this->tab_generic, 10, "Frequency (Hz)", 9, "868000000",
                     GuiConfigRadioTestModes::callback_ta );

    this->create_ta( &( this->ta_pl_len ), this->tab_generic, 45, "Payload length", 3, "5",
                     GuiConfigRadioTestModes::callback_ta );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        this->create_ta( &( this->ta_pkt ), this->tab_generic, 80, "Nb of packets", 5, "100",
                         GuiConfigRadioTestModes::callback_ta );
    }

    this->createDropDownList( &( this->ddlist_pwr ), this->tab_generic, 115, "Power (dBm)", lp_pa_options,
                              GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

    this->createChoiceSwitch( &( this->sw_pkt_type ), this->tab_generic, "LoRa", "GFSK",
                              GuiConfigRadioTestModes::callback_sw, 30, true );

    this->createChoiceSwitch( &( this->sw_pa ), this->tab_generic, "LP PA", "HP PA",
                              GuiConfigRadioTestModes::callback_sw, 65,
                              ( this->device_type == VERSION_DEVICE_TRANSCEIVER ) ? true : false );

    switch( device_type )
    {
    case VERSION_DEVICE_TRANSCEIVER:
        this->createDropDownList( &( this->lora_ddlist_sf ), this->tab_lora, 20, "Spreading Factor",
                                  "SF5\n"
                                  "SF6\n"
                                  "SF7\n"
                                  "SF8\n"
                                  "SF9\n"
                                  "SF10\n"
                                  "SF11\n"
                                  "SF12",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );
        break;
    case VERSION_DEVICE_MODEM:
        this->createDropDownList( &( this->lora_ddlist_sf ), this->tab_lora, 20, "Spreading Factor",
                                  "SF7\n"
                                  "SF8\n"
                                  "SF9\n"
                                  "SF10\n"
                                  "SF11\n"
                                  "SF12",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );
        break;
    }

    this->createDropDownList( &( this->lora_ddlist_bw ), this->tab_lora, 50, "Bandwidth",
                              "125kHz\n"
                              "250kHz\n"
                              "500kHz",
                              GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

    this->createDropDownList( &( this->lora_ddlist_cr ), this->tab_lora, 80, "Coding rate",
                              "4/5\n"
                              "4/6\n"
                              "4/7\n"
                              "4/8",
                              GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        this->createDropDownList( &( this->lora_ddlist_iq ), this->tab_lora, 110, "IQ",
                                  "Standard\n"
                                  "Inverted",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

        this->createDropDownList( &( this->lora_ddlist_crc ), this->tab_lora, 140, "CRC",
                                  "OFF\n"
                                  "ON",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

        this->createDropDownList( &( this->lora_ddlist_hdr ), this->tab_lora, 170, "Header type",
                                  "Implicit\n"
                                  "Explicit",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );
    }

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        this->create_ta( &( this->gfsk_ta_br ), this->tab_gfsk, 10, "Bitrate (bps)", 6, "150000",
                         GuiConfigRadioTestModes::callback_ta );

        this->create_ta( &( this->gfsk_ta_fdev ), this->tab_gfsk, 45, "Freq. dev. (Hz)", 6, "50000",
                         GuiConfigRadioTestModes::callback_ta );

        this->createDropDownList( &( this->gfsk_ddlist_crc ), this->tab_gfsk, 80, "CRC",
                                  "OFF\n"
                                  "1B\n"
                                  "2B\n"
                                  "1B inv.\n"
                                  "2B inv.",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

        this->createDropDownList( &( this->gfsk_ddlist_hdr ), this->tab_gfsk, 110, "Header type",
                                  "Implicit\n"
                                  "Explicit",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );

        this->createDropDownList( &( this->gfsk_ddlist_dcfree ), this->tab_gfsk, 140, "DC free",
                                  "OFF\n"
                                  "ON",
                                  GuiConfigRadioTestModes::callback_ddlist, DDLIST_WIDTH, 0 );
    }

    this->createActionButton( &( this->btn_cancel ), "CANCEL", GuiConfigRadioTestModes::callback, GUI_BUTTON_POS_LEFT,
                              -5, true );

    this->createActionButton( &( this->btn_default ), "DEFAULT", GuiConfigRadioTestModes::callback,
                              GUI_BUTTON_POS_CENTER, -5, true );

    this->createActionButton( &( this->btn_save ), "SAVE", GuiConfigRadioTestModes::callback, GUI_BUTTON_POS_RIGHT, -5,
                              false );

    /*Create a keyboard and apply the styles*/
    this->kb_num = lv_kb_create( this->screen, NULL );
    lv_kb_set_mode( this->kb_num, LV_KB_MODE_NUM );
    lv_kb_set_cursor_manage( this->kb_num, true );
    lv_kb_set_style( this->kb_num, LV_KB_STYLE_BG, &lv_style_transp_tight );
    lv_obj_set_event_cb( this->kb_num, GuiConfigRadioTestModes::callback_kb );
    lv_obj_set_user_data( this->kb_num, this );
    lv_obj_move_foreground( this->kb_num );
    lv_obj_set_hidden( this->kb_num, true );

    this->ConfigActionButton( );

    this->ConfigParamGeneric( );
    this->ConfigParamLora( );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        this->ConfigParamGfsk( );
    }

    lv_scr_load( this->screen );
}

GuiConfigRadioTestModes::~GuiConfigRadioTestModes( ) {}

void GuiConfigRadioTestModes::ConfigParamGeneric( )
{
    char str[TMP_BUFFER_CONFIG_PARAM_GENERIC_LENGTH];

    snprintf( str, TMP_BUFFER_CONFIG_PARAM_GENERIC_LENGTH, "%d", this->settings_temp.rf_freq_in_hz );
    lv_ta_set_text( this->ta_freq, str );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        snprintf( str, TMP_BUFFER_CONFIG_PARAM_GENERIC_LENGTH, "%d", this->settings_temp.nb_of_packets );
        lv_ta_set_text( this->ta_pkt, str );
    }

    snprintf( str, TMP_BUFFER_CONFIG_PARAM_GENERIC_LENGTH, "%d", this->settings_temp.payload_length );
    lv_ta_set_text( this->ta_pl_len, str );

    if( this->settings_temp.is_lora == true )
    {
        lv_sw_off( this->sw_pkt_type, LV_ANIM_OFF );
    }
    else
    {
        lv_sw_on( this->sw_pkt_type, LV_ANIM_OFF );
    }

    if( this->settings_temp.is_hp_pa_enabled == true )
    {
        lv_sw_on( this->sw_pa, LV_ANIM_OFF );
        lv_ddlist_set_options( this->ddlist_pwr, hp_pa_options );
        switch( this->settings_temp.pwr_in_dbm )
        {
        case 14:
            lv_ddlist_set_selected( this->ddlist_pwr, 0 );
            break;
        case 17:
            lv_ddlist_set_selected( this->ddlist_pwr, 1 );
            break;
        case 20:
            lv_ddlist_set_selected( this->ddlist_pwr, 2 );
            break;
        case 22:
            lv_ddlist_set_selected( this->ddlist_pwr, 3 );
            break;
        default:
            break;
        }
    }
    else
    {
        lv_sw_off( this->sw_pa, LV_ANIM_OFF );
        lv_ddlist_set_options( this->ddlist_pwr, lp_pa_options );
        switch( this->settings_temp.pwr_in_dbm )
        {
        case 10:
            lv_ddlist_set_selected( this->ddlist_pwr, 0 );
            break;
        case 14:
            lv_ddlist_set_selected( this->ddlist_pwr, 1 );
            break;
        case 15:
            lv_ddlist_set_selected( this->ddlist_pwr, 2 );
            break;
        default:
            break;
        }
    }
}

void GuiConfigRadioTestModes::ConfigParamLora( )
{
    lv_ddlist_set_selected( this->lora_ddlist_sf, ( this->device_type == VERSION_DEVICE_TRANSCEIVER )
                                                      ? this->settings_temp.lora.sf
                                                      : this->settings_temp.lora.sf - 2 );

    lv_ddlist_set_selected( this->lora_ddlist_bw, this->settings_temp.lora.bw );

    lv_ddlist_set_selected( this->lora_ddlist_cr, this->settings_temp.lora.cr );

    if( this->device_type == VERSION_DEVICE_TRANSCEIVER )
    {
        lv_ddlist_set_selected( this->lora_ddlist_iq, ( this->settings_temp.lora.is_iq_inverted == false ) ? 0 : 1 );

        lv_ddlist_set_selected( this->lora_ddlist_crc, ( this->settings_temp.lora.is_crc_activated == false ) ? 0 : 1 );

        lv_ddlist_set_selected( this->lora_ddlist_hdr, ( this->settings_temp.lora.is_hdr_implicit == true ) ? 0 : 1 );
    }
}

void GuiConfigRadioTestModes::ConfigParamGfsk( )
{
    char str[7];

    snprintf( str, TMP_BUFFER_CONFIG_PARAM_GFSK_LENGTH, "%d", this->settings_temp.gfsk.br_in_bps );
    lv_ta_set_text( this->gfsk_ta_br, str );

    snprintf( str, TMP_BUFFER_CONFIG_PARAM_GFSK_LENGTH, "%d", this->settings_temp.gfsk.fdev_in_hz );
    lv_ta_set_text( this->gfsk_ta_fdev, str );

    lv_ddlist_set_selected( this->gfsk_ddlist_crc, this->settings_temp.gfsk.crc );

    lv_ddlist_set_selected( this->gfsk_ddlist_hdr, ( this->settings_temp.gfsk.is_hdr_implicit == true ) ? 0 : 1 );

    lv_ddlist_set_selected( this->gfsk_ddlist_dcfree, ( this->settings_temp.gfsk.is_dcfree_enabled == false ) ? 0 : 1 );
}

void GuiConfigRadioTestModes::ConfigActionButton( )
{
    lv_btn_set_state( this->btn_default, ( ( this->IsConfigTempEqualTo( this->settings_default ) == true ) )
                                             ? LV_BTN_STATE_INA
                                             : LV_BTN_STATE_REL );

    lv_btn_set_state( this->btn_save, ( ( this->IsConfigTempEqualTo( this->settings_current ) == true ) )
                                          ? LV_BTN_STATE_INA
                                          : LV_BTN_STATE_REL );
}

bool GuiConfigRadioTestModes::IsConfigTempEqualTo( const GuiRadioSetting_t* settings_to_compare ) const
{
    if( this->settings_temp.rf_freq_in_hz != settings_to_compare->rf_freq_in_hz )
    {
        return false;
    }
    else if( this->settings_temp.pwr_in_dbm != settings_to_compare->pwr_in_dbm )
    {
        return false;
    }
    else if( this->settings_temp.nb_of_packets != settings_to_compare->nb_of_packets )
    {
        return false;
    }
    else if( this->settings_temp.payload_length != settings_to_compare->payload_length )
    {
        return false;
    }
    else if( this->settings_temp.is_lora != settings_to_compare->is_lora )
    {
        return false;
    }
    else if( this->settings_temp.is_hp_pa_enabled != settings_to_compare->is_hp_pa_enabled )
    {
        return false;
    }
    else if( this->settings_temp.lora.sf != settings_to_compare->lora.sf )
    {
        return false;
    }
    else if( this->settings_temp.lora.bw != settings_to_compare->lora.bw )
    {
        return false;
    }
    else if( this->settings_temp.lora.cr != settings_to_compare->lora.cr )
    {
        return false;
    }
    else if( this->settings_temp.lora.is_iq_inverted != settings_to_compare->lora.is_iq_inverted )
    {
        return false;
    }
    else if( this->settings_temp.lora.is_crc_activated != settings_to_compare->lora.is_crc_activated )
    {
        return false;
    }
    else if( this->settings_temp.lora.is_hdr_implicit != settings_to_compare->lora.is_hdr_implicit )
    {
        return false;
    }
    else if( this->settings_temp.gfsk.br_in_bps != settings_to_compare->gfsk.br_in_bps )
    {
        return false;
    }
    else if( this->settings_temp.gfsk.fdev_in_hz != settings_to_compare->gfsk.fdev_in_hz )
    {
        return false;
    }
    else if( this->settings_temp.gfsk.crc != settings_to_compare->gfsk.crc )
    {
        return false;
    }
    else if( this->settings_temp.gfsk.is_dcfree_enabled != settings_to_compare->gfsk.is_dcfree_enabled )
    {
        return false;
    }
    else if( this->settings_temp.gfsk.is_hdr_implicit != settings_to_compare->gfsk.is_hdr_implicit )
    {
        return false;
    }

    return true;
}

void GuiConfigRadioTestModes::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigRadioTestModes* self = ( GuiConfigRadioTestModes* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_cancel )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_default )
        {
            self->settings_temp = *( self->settings_default );
            self->ConfigParamGeneric( );
            self->ConfigParamLora( );
            if( self->device_type == VERSION_DEVICE_TRANSCEIVER )
            {
                self->ConfigParamGfsk( );
            }
            self->ConfigActionButton( );
        }
        else if( obj == self->btn_save )
        {
            *self->settings_current = self->settings_temp;
            GuiCommon::_event       = GUI_EVENT_SAVE;
        }
    }
}

void GuiConfigRadioTestModes::callback_sw( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigRadioTestModes* self = ( GuiConfigRadioTestModes* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_VALUE_CHANGED )
    {
        if( obj == self->sw_pkt_type )
        {
            self->settings_temp.is_lora = !self->settings_temp.is_lora;
            self->ConfigActionButton( );
        }
        else if( obj == self->sw_pa )
        {
            self->settings_temp.is_hp_pa_enabled = !self->settings_temp.is_hp_pa_enabled;
            if( self->settings_temp.is_hp_pa_enabled == true )
            {
                lv_ddlist_set_options( self->ddlist_pwr, self->hp_pa_options );
                self->settings_temp.pwr_in_dbm = 14;
            }
            else
            {
                lv_ddlist_set_options( self->ddlist_pwr, self->lp_pa_options );
                self->settings_temp.pwr_in_dbm = 10;
            }
            lv_ddlist_set_selected( self->ddlist_pwr, 0 );
            self->ConfigActionButton( );
        }
    }
}

void GuiConfigRadioTestModes::callback_ta( lv_obj_t* obj, lv_event_t event )
{
    GuiConfigRadioTestModes* self = ( GuiConfigRadioTestModes* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_PRESSED )
    {
        if( lv_kb_get_ta( self->kb_num ) == NULL )
        {
            /*Assign the text area to the keyboard*/
            lv_kb_set_ta( self->kb_num, obj );
            lv_obj_set_hidden( self->kb_num, false );

            const char* txt = lv_ta_get_text( obj );
            strcpy( self->ta_text, txt );

            if( obj != self->ta_freq )
            {
                lv_ta_set_cursor_pos( obj, LV_TA_CURSOR_LAST );
            }
        }
    }
}

void GuiConfigRadioTestModes::callback_kb( lv_obj_t* obj, lv_event_t event )
{
    lv_obj_t* ta = lv_kb_get_ta( obj );

    GuiConfigRadioTestModes* self = ( GuiConfigRadioTestModes* ) lv_obj_get_user_data( obj );

    lv_kb_def_event_cb( obj, event );

    if( event == LV_EVENT_CANCEL )
    {
        lv_kb_set_ta( obj, NULL ); /*De-assign the text area*/
        lv_obj_set_hidden( obj, true );
    }
    else if( event == LV_EVENT_APPLY )
    {
        char        str[TMP_BUFFER_CALLBACK_LENGTH];
        const char* txt = lv_ta_get_text( ta );

        int len = strlen( txt );

        if( len == 0 )
        {
            lv_ta_set_text( ta, self->ta_text );
        }

        if( ta == self->ta_freq )
        {
            self->settings_temp.rf_freq_in_hz = ( uint32_t ) atoi( txt );

            self->settings_temp.rf_freq_in_hz =
                GuiCommon::check_value_limits( self->settings_temp.rf_freq_in_hz, 150000000, 960000000 );

            snprintf( str, TMP_BUFFER_CALLBACK_LENGTH, "%d", self->settings_temp.rf_freq_in_hz );
            lv_ta_set_text( ta, str );
        }
        else if( ta == self->ta_pkt )
        {
            self->settings_temp.nb_of_packets = ( uint32_t ) atoi( txt );

            snprintf( str, TMP_BUFFER_CALLBACK_LENGTH, "%d", self->settings_temp.nb_of_packets );
            lv_ta_set_text( ta, str );
        }
        else if( ta == self->ta_pl_len )
        {
            self->settings_temp.payload_length = ( uint32_t ) atoi( txt );

            self->settings_temp.payload_length =
                GuiCommon::check_value_limits( self->settings_temp.payload_length, 1, 255 );

            snprintf( str, TMP_BUFFER_CALLBACK_LENGTH, "%d", self->settings_temp.payload_length );
            lv_ta_set_text( ta, str );
        }
        else if( ta == self->gfsk_ta_br )
        {
            self->settings_temp.gfsk.br_in_bps = ( uint32_t ) atoi( txt );

            self->settings_temp.gfsk.br_in_bps =
                GuiCommon::check_value_limits( self->settings_temp.gfsk.br_in_bps, 600, 300000 );

            snprintf( str, TMP_BUFFER_CALLBACK_LENGTH, "%d", self->settings_temp.gfsk.br_in_bps );
            lv_ta_set_text( ta, str );
        }
        else if( ta == self->gfsk_ta_fdev )
        {
            self->settings_temp.gfsk.fdev_in_hz = ( uint32_t ) atoi( txt );

            self->settings_temp.gfsk.fdev_in_hz =
                GuiCommon::check_value_limits( self->settings_temp.gfsk.fdev_in_hz, 600, 200000 );

            snprintf( str, TMP_BUFFER_CALLBACK_LENGTH, "%d", self->settings_temp.gfsk.fdev_in_hz );
            lv_ta_set_text( ta, str );
        }

        self->ConfigActionButton( );

        lv_kb_set_ta( obj, NULL ); /*De-assign the text area*/
        lv_obj_set_hidden( obj, true );
    }
}

void GuiConfigRadioTestModes::callback_ddlist( lv_obj_t* obj, lv_event_t event )
{
    uint8_t                  id;
    GuiConfigRadioTestModes* self = ( GuiConfigRadioTestModes* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_VALUE_CHANGED )
    {
        id = lv_ddlist_get_selected( obj );

        if( obj == self->ddlist_pwr )
        {
            if( self->settings_temp.is_hp_pa_enabled == true )
            {
                switch( id )
                {
                case 0:
                    self->settings_temp.pwr_in_dbm = 14;
                    break;
                case 1:
                    self->settings_temp.pwr_in_dbm = 17;
                    break;
                case 2:
                    self->settings_temp.pwr_in_dbm = 20;
                    break;
                case 3:
                    self->settings_temp.pwr_in_dbm = 22;
                    break;
                }
            }
            else
            {
                switch( id )
                {
                case 0:
                    self->settings_temp.pwr_in_dbm = 10;
                    break;
                case 1:
                    self->settings_temp.pwr_in_dbm = 14;
                    break;
                case 2:
                    self->settings_temp.pwr_in_dbm = 15;
                    break;
                }
            }
        }

        else if( obj == self->lora_ddlist_sf )
        {
            // The "+ 2" trick is done because modem and transceiver does not behave the same
            self->settings_temp.lora.sf = ( self->device_type == VERSION_DEVICE_TRANSCEIVER ) ? id : id + 2;
        }

        else if( obj == self->lora_ddlist_bw )
        {
            self->settings_temp.lora.bw = id;
        }

        else if( obj == self->lora_ddlist_cr )
        {
            self->settings_temp.lora.cr = id;
        }

        else if( obj == self->lora_ddlist_iq )
        {
            self->settings_temp.lora.is_iq_inverted = ( id == 0 ) ? false : true;
        }

        else if( obj == self->lora_ddlist_crc )
        {
            self->settings_temp.lora.is_crc_activated = ( id == 0 ) ? false : true;
        }

        else if( obj == self->lora_ddlist_hdr )
        {
            self->settings_temp.lora.is_hdr_implicit = ( id == 0 ) ? false : true;
        }

        else if( obj == self->gfsk_ddlist_crc )
        {
            self->settings_temp.gfsk.crc = id;
        }

        else if( obj == self->gfsk_ddlist_hdr )
        {
            self->settings_temp.gfsk.is_hdr_implicit = ( id == 0 ) ? false : true;
        }

        else if( obj == self->gfsk_ddlist_dcfree )
        {
            self->settings_temp.gfsk.is_dcfree_enabled = ( id == 0 ) ? false : true;
        }

        self->ConfigActionButton( );
    }
}

void GuiConfigRadioTestModes::create_ta( lv_obj_t** ta, lv_obj_t* screen, int16_t off_y, const char* lbl_name,
                                         uint16_t max_length, const char* init_text, lv_event_cb_t event_cb )
{
    lv_obj_t* lbl;

    lbl = lv_label_create( screen, NULL );
    lv_obj_set_style( lbl, &( GuiCommon::screen_style ) );
    lv_label_set_text( lbl, lbl_name );
    lv_obj_align( lbl, NULL, LV_ALIGN_IN_TOP_LEFT, 5, off_y );

    *ta = lv_ta_create( screen, NULL );
    lv_obj_set_click( *ta, true );
    lv_obj_set_event_cb( *ta, event_cb );
    lv_obj_set_width( *ta, 100 );
    lv_obj_align( *ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, off_y );
    lv_ta_set_accepted_chars( *ta, "0123456789" );
    lv_ta_set_cursor_type( *ta, LV_CURSOR_LINE | LV_CURSOR_HIDDEN );
    lv_ta_set_one_line( *ta, true );
    lv_ta_set_max_length( *ta, max_length );
    lv_ta_set_text( *ta, init_text );
    lv_ta_set_text_align( *ta, LV_LABEL_ALIGN_RIGHT );
    lv_obj_set_user_data( *ta, this );
}
