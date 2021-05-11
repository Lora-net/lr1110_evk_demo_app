/**
 * @file      guiFileUpload.h
 *
 * @brief     Definition of the gui file upload page.
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

#include "guiFileUpload.h"

GuiFileUpload::GuiFileUpload( const GuiFileUploadResult_t* results )
    : GuiCommon( GUI_PAGE_FILE_UPLOAD_DEMO ), is_running( false ), results( results )
{
    this->createHeaderWithIcons( "FILE UPLOAD DEMO" );
    this->lbl_btn_start_stop = this->createActionButton( &( this->btn_start_stop ), "START", GuiFileUpload::callback,
                                                         GUI_BUTTON_POS_CENTER, -60, true );
    this->createActionButton( &( this->btn_back ), "BACK", GuiFileUpload::callback, GUI_BUTTON_POS_CENTER, -5, true );
    this->createInfoFrame( &( this->info_frame ), &( this->lbl_info_frame_1 ), "PRESS START TO BEGIN",
                           &( this->lbl_info_frame_2 ), "", &( this->lbl_info_frame_3 ), "" );
    lv_scr_load( this->screen );
}

GuiFileUpload::~GuiFileUpload( ) {}

void GuiFileUpload::start( )
{
    printf( "File Upload gui start\n" );
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );

    lv_label_set_text( this->lbl_info_frame_1, "Sending file" );
    lv_label_set_text( this->lbl_info_frame_2, "See Application Server" );
    lv_label_set_text( this->lbl_info_frame_3, "" );

    lv_label_set_text( this->lbl_btn_start_stop, "STOP" );
    this->is_running = true;
}

void GuiFileUpload::stop( )
{
    lv_label_set_text( this->lbl_btn_start_stop, "START" );
    lv_label_set_text( this->lbl_info_frame_1, "PRESS START TO BEGIN" );
    lv_label_set_text( this->lbl_info_frame_2, "" );
    lv_label_set_text( this->lbl_info_frame_3, "" );
    lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_init ) );
    this->is_running = false;
}

void GuiFileUpload::refresh( )
{
    if( this->results->terminated == true )
    {
        if( this->results->success == true )
        {
            lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ok ) );
            this->is_running = false;
            lv_label_set_text( this->lbl_info_frame_2, "File Upload Success" );
            lv_label_set_text( this->lbl_btn_start_stop, "START" );
        }
        else
        {
            lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_error ) );
            this->is_running = false;
            lv_label_set_text( this->lbl_info_frame_2, "File Upload Timeout" );
            lv_label_set_text( this->lbl_btn_start_stop, "START" );
        }
    }
    else
    {
        lv_label_set_text( this->lbl_info_frame_2, "Sending..." );
        lv_cont_set_style( this->info_frame, LV_CONT_STYLE_MAIN, &( GuiCommon::info_frame_style_ongoing ) );
    }

    this->updateButtons( );
}

void GuiFileUpload::propagateHostConnectivityStateChange( ) { this->updateButtons( ); }

void GuiFileUpload::propagateNetworkConnectivityStateChange( ) { this->updateButtons( ); }

void GuiFileUpload::callback( lv_obj_t* obj, lv_event_t event )
{
    GuiFileUpload* self = ( GuiFileUpload* ) lv_obj_get_user_data( obj );

    if( ( event == LV_EVENT_RELEASED ) && ( lv_btn_get_state( obj ) != LV_BTN_STATE_INA ) )
    {
        if( obj == self->btn_back )
        {
            GuiCommon::_event = GUI_EVENT_BACK;
        }
        else if( obj == self->btn_start_stop )
        {
            if( self->is_running == true )
            {
                GuiCommon::_event = GUI_EVENT_STOP;
            }
            else
            {
                GuiCommon::_event = GUI_EVENT_START_DEMO_FILE_UPLOAD;
            }
        }
    }
}

void GuiFileUpload::updateButtons( )
{
    // if( ( ( GuiCommon::_network_connectivity_status.connectivity_state == GUI_CONNECTIVITY_STATUS_CONNECTED ) ||
    //       ( GuiCommon::_is_host_connected == true ) ) )
    // {
    //     lv_btn_set_state( this->btn_send, LV_BTN_STATE_REL );
    // }
    // else
    // {
    //     lv_btn_set_state( this->btn_send, LV_BTN_STATE_INA );
    // }
}
