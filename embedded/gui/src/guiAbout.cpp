/**
 * @file      guiAbout.cpp
 *
 * @brief     Implementation of the gui about page.
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

#include "guiAbout.h"
#include "version.h"
#include "semtech_logo.h"

#define GUI_ABOUT_BUFFER_LENGTH ( 100 )

GuiAbout::GuiAbout( version_handler_t* version_handler )
    : GuiCommon( GUI_PAGE_ABOUT ), version_handler( version_handler )
{
    char buffer[GUI_ABOUT_BUFFER_LENGTH];

    this->createHeader( "ABOUT" );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "EVK DEMO APP -  %s", DEMO_VERSION );
    this->createSection( buffer, -100 );

    switch( this->version_handler->device_type )
    {
    case VERSION_DEVICE_TRANSCEIVER:
    {
        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 TYPE - 0x%02x",
                  this->version_handler->transceiver.version_chip_type );
        this->createSection( buffer, -80 );

        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 HARDWARE - 0x%02x",
                  this->version_handler->transceiver.version_chip_hw );
        this->createSection( buffer, -60 );

        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LR1110 FIRMWARE - 0x%04x",
                  this->version_handler->transceiver.version_chip_fw );
        this->createSection( buffer, -40 );
        break;
    }

    case VERSION_DEVICE_MODEM:
    {
        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "MODEM TYPE - 0x%02x",
                  this->version_handler->modem.version_chip_type );
        this->createSection( buffer, -80 );

        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "MODEM BLOADER - 0x%08x",
                  this->version_handler->modem.version_chip_bootloader );
        this->createSection( buffer, -60 );

        snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "MODEM FIRMWARE - 0x%06x",
                  this->version_handler->modem.version_chip_fw );
        this->createSection( buffer, -40 );
        break;
    }

    default:
        break;
    }

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "LVGL\nCopyright (c) 2016\nGabor Kiss-Vamosi" );
    this->createSection( buffer, 10 );

    snprintf( buffer, GUI_ABOUT_BUFFER_LENGTH, "Data (c) OpenStreetMap\ncontributors, ODbL 1.0." );
    this->createSection( buffer, 80 );

    this->createActionButton( &( this->btn_back ), "BACK", GuiAbout::callback, GUI_BUTTON_POS_CENTER, -5, true );

    lv_scr_load( this->screen );
}

GuiAbout::~GuiAbout( ) {}

void GuiAbout::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiAbout* self = ( GuiAbout* ) lv_obj_get_user_data( obj );

    if( event == LV_EVENT_RELEASED )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
    }
}
