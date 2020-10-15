/**
 * @file      guiMenu.cpp
 *
 * @brief     Implementation of the gui menu page.
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

#include "guiMenu.h"

#define BUTTON_HEIGHT 40
#define BUTTON_WIDTH 60
#define LABEL_WIDTH 150
#define MARGIN 10

GuiMenu::GuiMenu( version_handler_t* version_handler ) : GuiMenuCommon( GUI_PAGE_MENU )
{
    uint8_t index = 0;

    this->createHeader( "MENU" );

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

    this->createTestEntry( index++, &( this->lbl_radio_test_modes ), &( this->btn_radio_test_modes ),
                           &( this->lbl_btn_radio_test_modes ), "RADIO TEST MODES", true, GuiMenu::callback );

    this->createTestEntry( index++, &( this->lbl_demos ), &( this->btn_demos ), &( this->lbl_btn_demos ),
                           "DEMONSTRATIONS", true, GuiMenu::callback );

    if( GuiCommon::_has_connectivity == true )
    {
        this->createTestEntry( index++, &( this->lbl_connectivity ), &( this->btn_connectivity ),
                               &( this->lbl_btn_connectivity ), "CONNECTIVITY", true, GuiMenu::callback );
    }

    this->createTestEntry( index++, &( this->lbl_eui ), &( this->btn_eui ), &( this->lbl_btn_eui ), "EUI", true,
                           GuiMenu::callback );

    lv_scr_load( this->screen );
}

GuiMenu::~GuiMenu( ) {}

void GuiMenu::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiMenu* self = ( GuiMenu* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_radio_test_modes )
        {
            GuiCommon::_event = GUI_EVENT_LAUNCH_RADIO_TEST_MODE;
        }
        else if( obj == self->btn_demos )
        {
            GuiCommon::_event = GUI_EVENT_LAUNCH_DEMO;
        }
        else if( obj == self->btn_connectivity )
        {
            GuiCommon::_event = GUI_EVENT_LAUNCH_CONNECTIVITY;
        }
        else if( obj == self->btn_eui )
        {
            GuiCommon::_event = GUI_EVENT_EUI;
        }
    }
}
