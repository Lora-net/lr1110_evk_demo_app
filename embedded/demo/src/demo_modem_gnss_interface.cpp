/**
 * @file      demo_modem_gnss_interface.cpp
 *
 * @brief     Implementation of the components shared by GNSS demonstrations.
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

#include <string.h>
#include "demo_modem_gnss_interface.h"
#include "system_lptim.h"
#include "lr1110_modem_lorawan.h"

#define DEMO_GNSS_CONSUMPTION_DCDC_ACQUISITION_MA ( 11 )
#define DEMO_GNSS_CONSUMPTION_DCDC_COMPUTATION_MA ( 6 )

DemoModemGnssInterface::DemoModemGnssInterface( DeviceModem* device, SignalingInterface* signaling,
                                                EnvironmentInterface*     environment,
                                                AntennaSelectorInterface* antenna_selector, TimerInterface* timer,
                                                CommunicationInterface* communication_interface )
    : DemoModemInterface( device, signaling, communication_interface ),
      timer( timer ),
      state( DEMO_MODEM_GNSS_BASE_INIT ),
      environment( environment ),
      instant_start_capture_ms( 0 ),
      antenna_selector( antenna_selector )
{
}

DemoModemGnssInterface::~DemoModemGnssInterface( ) {}

void DemoModemGnssInterface::Reset( )
{
    this->DemoInterface::Reset( );
    this->state                            = DEMO_MODEM_GNSS_BASE_INIT;
    this->result.nb_result                 = 0;
    this->result.nav_message.size          = 0;
    this->result.consumption_uas           = 0;
    this->result.error                     = DEMO_GNSS_BASE_NO_ERROR;
    this->result.timings                   = { 0 };
    this->result.local_instant_measurement = 0;
    this->result.almanac_too_old           = false;
    this->result.almanac_age_days          = 0;
    this->instant_start_capture_ms         = 0;
}

void DemoModemGnssInterface::JumpToErrorState( const demo_gnss_error_t error_code )
{
    this->result.error = error_code;
    this->state        = DEMO_MODEM_GNSS_BASE_ERROR;
}

demo_gnss_settings_t DemoModemGnssInterface::GetSettings( ) const { return this->settings; }

EnvironmentInterface* DemoModemGnssInterface::GetEnvironment( ) { return this->environment; }

void DemoModemGnssInterface::SpecificRuntime( )
{
    static uint32_t measurement_instant_s;
    switch( this->state )
    {
    case DEMO_MODEM_GNSS_BASE_INIT:
    {
        lr1110_modem_gnss_set_constellations_to_use( this->device->GetRadio( ), this->settings.constellation_mask );

        this->SetAntennaSwitch( this->settings.antenna_selection );
        this->ConfigureEnvironmentTimeFromDevice( );

        if( ( !this->GetEnvironment( )->HasLocation( ) ) )
        {
            this->AskAndStoreLocation( );
        }
        if( !this->GetEnvironment( )->HasLocation( ) )
        {
            this->JumpToErrorState( DEMO_GNSS_BASE_ERROR_NO_LOCATION );
            this->communication_interface->Log( "No location available\n" );
            break;
        }
        this->state = DEMO_MODEM_GNSS_BASE_SCAN;
        break;
    }

    case DEMO_MODEM_GNSS_BASE_SCAN:
    {
        this->SetWaitingForInterrupt( );
        measurement_instant_s = this->environment->GetLocalTimeSeconds( );

        const lr1110_modem_response_code_t scan_response_code = this->CallScan( );

        if( scan_response_code != LR1110_MODEM_RESPONSE_CODE_OK )
        {
            this->communication_interface->Log( "Error code when calling scan: 0x%x\r\n", scan_response_code );
            this->JumpToErrorState( DemoModemGnssInterface::ErrorCodeFromScanResponseCode( scan_response_code ) );
        }
        else
        {
            this->instant_start_capture_ms = this->environment->GetLocalTimeMilliseconds( );

            this->state = DEMO_MODEM_GNSS_BASE_WAIT_FOR_SCAN;
            this->signaling->StartCapture( );
        }

        break;
    }

    case DEMO_MODEM_GNSS_BASE_WAIT_FOR_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( this->last_received_event.event_type == LR1110_MODEM_LORAWAN_EVENT_GNSS_SCAN_DONE )
            {
                this->result.local_instant_measurement = measurement_instant_s;
                this->FetchAndSaveResults( this->last_received_event.buffer, this->last_received_event.buffer_len );
                this->state = DEMO_MODEM_GNSS_BASE_TERMINATED;
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_MODEM_GNSS_BASE_WAIT_FOR_SCAN;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_MODEM_GNSS_BASE_WAIT_FOR_SCAN;
        }
        break;
    }

    case DEMO_MODEM_GNSS_BASE_TERMINATED:
    {
        this->state = DEMO_MODEM_GNSS_BASE_INIT;
        this->Terminate( );
        break;
    }

    case DEMO_MODEM_GNSS_BASE_ERROR:
    {
        this->state = DEMO_MODEM_GNSS_BASE_TERMINATED;
        break;
    }
    }
}

void DemoModemGnssInterface::SpecificStop( ) { signaling->StopCapture( ); }

bool DemoModemGnssInterface::CanFetchResults( demo_gnss_nav_result_t& nav_message )
{
    if( DemoModemGnssInterface::IsResultToSolver( nav_message ) )
    {
        return true;
    }
    else
    {
        const demo_gnss_error_t error_code = DemoModemGnssInterface::GetHostErrorFromResult( nav_message );
        if( error_code == DEMO_GNSS_BASE_ERROR_NO_SATELLITE )
        {
            // In the case where no satellite has been detected, it is still ok
            // to fetch the results
            return true;
        }
        else
        {
            return false;
        }
    }
}

void DemoModemGnssInterface::FetchAndSaveResults( const uint8_t* buffer, uint16_t buffer_length )
{
    this->result.nav_message.size = buffer_length;
    if( this->result.nav_message.size <= GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH )
    {
        memcpy( this->result.nav_message.message, buffer, buffer_length );
    }
    else
    {
        this->JumpToErrorState( DEMO_GNSS_BASE_NAV_MESSAGE_TOO_LONG );
        this->communication_interface->Log(
            "Error when fetching NAV message: size too long (max is %u, "
            "actual size is %u)\n",
            GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH, this->result.nav_message.size );
    }

    if( DemoModemGnssInterface::CanFetchResults( this->result.nav_message ) )
    {
        uint8_t                                nb_sv_detected = 0;
        lr1110_modem_gnss_detected_satellite_t list_of_sv[30] = { 0 };
        lr1110_modem_gnss_get_nb_detected_satellites( this->device->GetRadio( ), &nb_sv_detected );
        lr1110_modem_gnss_get_detected_satellites( this->device->GetRadio( ), nb_sv_detected, list_of_sv );
        lr1110_modem_gnss_timings_t modem_timings = {};
        lr1110_modem_gnss_get_timings( this->device->GetRadio( ), &modem_timings );
        this->result.timings = DemoModemGnssInterface::DemoTimingFromModem( modem_timings );

        this->result.consumption_uas =
            ( this->result.timings.radio_ms * DEMO_GNSS_CONSUMPTION_DCDC_ACQUISITION_MA +
              this->result.timings.computation_ms * DEMO_GNSS_CONSUMPTION_DCDC_COMPUTATION_MA );

        this->result.nb_result = nb_sv_detected;

        for( uint8_t index = 0; ( index < nb_sv_detected ) && ( index < GNSS_DEMO_MAX_RESULT_TOTAL ); index++ )
        {
            if( list_of_sv[index].satellite_id < 64 )
            {
                this->result.result[index].constellation = DEMO_GNSS_CONSTELLATION_GPS;
                this->result.result[index].satellite_id  = list_of_sv[index].satellite_id + 1;
            }
            else
            {
                this->result.result[index].constellation = DEMO_GNSS_CONSTELLATION_BEIDOU;
                this->result.result[index].satellite_id  = ( list_of_sv[index].satellite_id % 64 ) + 1;
            }
            this->result.result[index].snr = list_of_sv[index].cnr;
        }
    }
}

void DemoModemGnssInterface::SpecificInterruptHandler( ) {}

void DemoModemGnssInterface::Configure( demo_gnss_settings_t& config ) { this->settings = config; }

const demo_gnss_all_results_t* DemoModemGnssInterface::GetResult( ) const { return &this->result; }

void DemoModemGnssInterface::SetAntennaSwitch( const demo_gnss_antenna_selection_t antenna_selection )
{
    switch( antenna_selection )
    {
    case DEMO_GNSS_NO_ANTENNA_SELECTION:
    {
        break;
    }

    case DEMO_GNSS_ANTENNA_SELECTION_1:
    {
        this->antenna_selector->SelectAntenna1( );
        break;
    }

    case DEMO_GNSS_ANTENNA_SELECTION_2:
    {
        this->antenna_selector->SelectAntenna2( );
        break;
    }
    }
}

void DemoModemGnssInterface::AskAndStoreLocation( )
{
    environment_location_t initial_position;
    uint32_t               actual_time_gps_seconds;

    const bool success = this->communication_interface->GetDateAndApproximateLocation(
        actual_time_gps_seconds, initial_position.latitude, initial_position.longitude, initial_position.altitude );

    if( success == true )
    {
        this->GetEnvironment( )->SetTimeFromGpsEpoch( actual_time_gps_seconds );
        this->GetEnvironment( )->SetLocation( initial_position );
    }
}

bool DemoModemGnssInterface::IsResultToSolver( const demo_gnss_nav_result_t& nav_result )
{
    if( nav_result.size >= 2 )
    {
        return nav_result.message[0] == LR1110_MODEM_GNSS_DESTINATION_SOLVER;
    }
    else
    {
        return false;
    }
}

bool DemoModemGnssInterface::IsResultToHost( const demo_gnss_nav_result_t& nav_result )
{
    if( nav_result.size >= 2 )
    {
        return nav_result.message[0] == LR1110_MODEM_GNSS_DESTINATION_HOST;
    }
    else
    {
        return false;
    }
}

demo_gnss_error_t DemoModemGnssInterface::GetHostErrorFromResult( const demo_gnss_nav_result_t& nav_result )
{
    demo_gnss_error_t error = DEMO_GNSS_BASE_ERROR_UNKNOWN;
    if( DemoModemGnssInterface::IsResultToHost( nav_result ) )
    {
        switch( nav_result.message[1] )
        {
        case 0x06:
        {
            error = DEMO_GNSS_BASE_ERROR_NO_DATE;
            break;
        }
        case 0x07:
        {
            error = DEMO_GNSS_BASE_ERROR_NO_SATELLITE;
            break;
        }
        case 0x08:
        {
            error = DEMO_GNSS_BASE_ERROR_ALMANAC_TOO_OLD;
            break;
        }
        default:
        {
            error = DEMO_GNSS_BASE_ERROR_UNKNOWN;
        }
        }
    }
    else
    {
        error = DEMO_GNSS_BASE_NO_ERROR;
    }
    return error;
}

uint16_t DemoModemGnssInterface::GetAlmanacAgeFromGpsEpochInDays( const uint16_t almanac_age )
{
    // The almanac_age is the number of days since last GPS week number rollover
    // As we are in year 2020, our last rollover was on April 7th 2019. The next
    // one will be around 2038. So if you are reading this code because you
    // suspect mistake in the almanac age handling, and the current year is >
    // 2038, consider revisiting this piece of code...
    // (and now I'm a 50 year old guy... Greetings from the past ;) )

    // Let's convert in the number of days elapsed since GPS epoch (January
    // 6th 1970)
    const uint16_t days_between_2019_rollover_to_epoch = 14336;
    const uint16_t almanac_age_days_since_gps_epoch    = almanac_age + days_between_2019_rollover_to_epoch;
    return almanac_age_days_since_gps_epoch;
}

lr1110_modem_gnss_search_mode_t DemoModemGnssInterface::ModemSearchModeFromDemo(
    const demo_gnss_search_mode_t& demo_search_mode )
{
    lr1110_modem_gnss_search_mode_t transceiver_search_mode = LR1110_MODEM_GNSS_OPTION_DEFAULT;
    switch( demo_search_mode )
    {
    case DEMO_GNSS_OPTION_DEFAULT:
    {
        transceiver_search_mode = LR1110_MODEM_GNSS_OPTION_DEFAULT;
        break;
    }

    case DEMO_GNSS_OPTION_BEST_EFFORT:
    {
        transceiver_search_mode = LR1110_MODEM_GNSS_OPTION_BEST_EFFORT;
        break;
    }
    }
    return transceiver_search_mode;
}

demo_gnss_timings_t DemoModemGnssInterface::DemoTimingFromModem(
    const lr1110_modem_gnss_timings_t& transceiver_timings )
{
    demo_gnss_timings_t demo_timings = {};
    demo_timings.computation_ms      = transceiver_timings.computation_ms;
    demo_timings.radio_ms            = transceiver_timings.radio_ms;
    return demo_timings;
}

uint16_t DemoModemGnssInterface::GetAlmanacAgeDaysSinceToday( ) const
{
    // First, we get the age of the almanac of a satellite (here sv_id 5)
    uint16_t almanac_age = 0;
    this->device->GetAlmanacAgesForSatelliteId( 5, &almanac_age );

    // Then we convert the age from almanac into number of days elapsed since
    // GPS epoch
    const uint16_t almanac_age_since_epoch = DemoModemGnssInterface::GetAlmanacAgeFromGpsEpochInDays( almanac_age );

    // We get the actual time in days
    const time_t   now_seconds = this->environment->GetDateTime( );
    const uint16_t now_days    = now_seconds / 3600 / 24;

    const uint16_t almanac_age_since_update = now_days - almanac_age_since_epoch;
    return almanac_age_since_update;
}

bool DemoModemGnssInterface::IsAlmanacTooOld( uint16_t almanac_age_days, const uint16_t days_limit )
{
    return almanac_age_days > days_limit;
}

bool DemoModemGnssInterface::CheckAndStoreAlmanacAge( const uint16_t days_limit )
{
    const uint16_t almanac_age_days = this->GetAlmanacAgeDaysSinceToday( );

    const bool almanac_too_old = DemoModemGnssInterface::IsAlmanacTooOld( almanac_age_days, days_limit );

    this->result.almanac_age_days = almanac_age_days;
    this->result.almanac_too_old  = almanac_too_old;

    return !almanac_too_old;
}

demo_gnss_error_t DemoModemGnssInterface::ErrorCodeFromScanResponseCode(
    lr1110_modem_response_code_t scan_response_code )
{
    demo_gnss_error_t error_code = DEMO_GNSS_BASE_ERROR_UNKNOWN;
    switch( scan_response_code )
    {
    case LR1110_MODEM_RESPONSE_CODE_NO_TIME:
    {
        error_code = DEMO_GNSS_BASE_ERROR_NO_DATE;
        break;
    }
    default:
    {
        error_code = DEMO_GNSS_BASE_ERROR_UNKNOWN;
    }
    }
    return error_code;
}

void DemoModemGnssInterface::ConfigureEnvironmentTimeFromDevice( )
{
    uint32_t gps_time = 0;
    lr1110_modem_get_gps_time( this->device->GetRadio( ), &gps_time );
    this->environment->SetTimeFromGpsEpoch( gps_time );
}