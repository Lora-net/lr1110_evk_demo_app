/**
 * @file      guiEui.cpp
 *
 * @brief     Implementation of the GUI EUI page.
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

#include "guiEui.h"

#define GUI_ABOUT_BUFFER_LENGTH ( 40 )

GuiEui::GuiEui( version_handler_t* version_handler ) : GuiCommon( GUI_PAGE_EUI )
{
    char buffer[GUI_ABOUT_BUFFER_LENGTH];

    this->createHeader( "EUI" );

    this->createNetworkConnectivityIcon( &( this->_label_connectivity_icon ) );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "CHIP EUI\n%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
              version_handler->chip_uid[0], version_handler->chip_uid[1], version_handler->chip_uid[2],
              version_handler->chip_uid[3], version_handler->chip_uid[4], version_handler->chip_uid[5],
              version_handler->chip_uid[6], version_handler->chip_uid[7] );
    this->createSection( buffer, -80 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "DEV EUI\n%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
              version_handler->dev_eui[0], version_handler->dev_eui[1], version_handler->dev_eui[2],
              version_handler->dev_eui[3], version_handler->dev_eui[4], version_handler->dev_eui[5],
              version_handler->dev_eui[6], version_handler->dev_eui[7] );
    this->createSection( buffer, -30 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "JOIN EUI\n%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
              version_handler->join_eui[0], version_handler->join_eui[1], version_handler->join_eui[2],
              version_handler->join_eui[3], version_handler->join_eui[4], version_handler->join_eui[5],
              version_handler->join_eui[6], version_handler->join_eui[7] );
    this->createSection( buffer, 20 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "PIN: %02X-%02X-%02X-%02X", version_handler->pin[0],
              version_handler->pin[1], version_handler->pin[2], version_handler->pin[3] );
    this->createSection( buffer, 70 );

    this->createActionButton( &( this->btn_back ), "BACK", GuiEui::callback, GUI_BUTTON_POS_CENTER, -5, true );

    lv_scr_load( this->screen );
}

GuiEui::~GuiEui( ) {}

void GuiEui::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiEui* self = ( GuiEui* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}
