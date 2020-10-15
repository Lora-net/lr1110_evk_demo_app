/**
 * @file      demo_transceiver_gnss_interface.cpp
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
#include "demo_transceiver_gnss_interface.h"
#include "lr1110_gnss.h"
#include "system_lptim.h"

#define DEMO_GNSS_CONSUMPTION_DCDC_ACQUISITION_MA ( 11 )
#define DEMO_GNSS_CONSUMPTION_DCDC_COMPUTATION_MA ( 6 )

DemoTransceiverGnssInterface::DemoTransceiverGnssInterface( DeviceTransceiver* device, SignalingInterface* signaling,
                                                            EnvironmentInterface*     environment,
                                                            AntennaSelectorInterface* antenna_selector,
                                                            TimerInterface*           timer,
                                                            CommunicationInterface*   communication_interface )
    : DemoTransceiverInterface( device, signaling, communication_interface, environment ),
      timer( timer ),
      gnss_irq( LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE ),
      state( DEMO_GNSS_BASE_INIT ),
      inter_capture_delay_s( 0 ),
      instant_start_capture_ms( 0 ),
      instant_second_capture_ms( 0 ),
      antenna_selector( antenna_selector )
{
}

DemoTransceiverGnssInterface::~DemoTransceiverGnssInterface( ) {}

void DemoTransceiverGnssInterface::Reset( )
{
    this->DemoInterface::Reset( );
    this->state                                           = DEMO_GNSS_BASE_INIT;
    this->result.nb_result                                = 0;
    this->result.nav_message.size                         = 0;
    this->result.consumption_uas                          = 0;
    this->result.error                                    = DEMO_GNSS_BASE_NO_ERROR;
    this->result.timings                                  = { 0 };
    this->result.local_instant_measurement                = 0;
    this->result.local_instant_measurement_second_capture = 0;
    this->result.almanac_too_old                          = false;
    this->result.almanac_age_days                         = 0;
    this->instant_start_capture_ms                        = 0;
    this->instant_second_capture_ms                       = 0;
}

void DemoTransceiverGnssInterface::JumpToErrorState( const demo_gnss_error_t error_code )
{
    this->result.error = error_code;
    this->state        = DEMO_GNSS_BASE_ERROR;
}

demo_gnss_settings_t DemoTransceiverGnssInterface::GetSettings( ) const { return this->settings; }

EnvironmentInterface* DemoTransceiverGnssInterface::GetEnvironment( ) { return this->environment; }

void DemoTransceiverGnssInterface::SpecificRuntime( )
{
    static uint32_t measurement_instant_s;
    switch( this->state )
    {
    case DEMO_GNSS_BASE_INIT:
    {
        uint32_t irq_to_en_dio1 = LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE;
        uint32_t irq_to_en_dio2 = 0x00;
        lr1110_system_set_dio_irq_params( this->device->GetRadio( ), irq_to_en_dio1, irq_to_en_dio2 );

        lr1110_gnss_set_constellations_to_use( this->device->GetRadio( ), this->settings.constellation_mask );

        lr1110_gnss_set_scan_mode(
            this->device->GetRadio( ),
            DemoTransceiverGnssInterface::TransceiverScanModeFromDemo( this->settings.capture_mode ),
            &this->inter_capture_delay_s );

        this->SetAntennaSwitch( this->settings.antenna_selection );

        if( ( !this->GetEnvironment( )->HasDate( ) ) || ( !this->GetEnvironment( )->HasLocation( ) ) )
        {
            this->AskAndStoreDate( );
        }
        if( !this->GetEnvironment( )->HasDate( ) )
        {
            this->JumpToErrorState( DEMO_GNSS_BASE_ERROR_NO_DATE );
            this->communication_interface->Log( "No date available\n" );
            break;
        }
        if( !this->GetEnvironment( )->HasLocation( ) )
        {
            this->JumpToErrorState( DEMO_GNSS_BASE_ERROR_NO_LOCATION );
            this->communication_interface->Log( "No location available\n" );
            break;
        }
        this->state = DEMO_GNSS_BASE_SCAN;
        break;
    }

    case DEMO_GNSS_BASE_SCAN:
    {
        this->SetWaitingForInterrupt( );
        measurement_instant_s = this->environment->GetLocalTimeSeconds( );

        this->CallScan( );

        this->instant_start_capture_ms = this->environment->GetLocalTimeMilliseconds( );

        this->state = DEMO_GNSS_BASE_WAIT_FOR_SCAN;
        this->signaling->StartCapture( );

        break;
    }

    case DEMO_GNSS_BASE_WAIT_AND_EXECUTE_SECOND_SCAN:
    {
        if( this->timer->is_timer_elapsed( ) == true )
        {
            this->timer->clear_timer( );
            lr1110_gnss_scan_continuous( this->device->GetRadio( ) );
            this->instant_second_capture_ms = this->environment->GetLocalTimeMilliseconds( );
            this->SetWaitingForInterrupt( );
            this->state = DEMO_GNSS_BASE_WAIT_FOR_SECOND_SCAN;
            this->signaling->StartCapture( );
        }
        break;
    }

    case DEMO_GNSS_BASE_WAIT_FOR_SECOND_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( ( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE ) != 0 )
            {
                this->state = DEMO_GNSS_BASE_GET_RESULTS;
                this->signaling->StopCapture( );
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_GNSS_BASE_WAIT_FOR_SECOND_SCAN;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_GNSS_BASE_WAIT_FOR_SECOND_SCAN;
        }
        break;
    }

    case DEMO_GNSS_BASE_WAIT_FOR_SCAN:
    {
        if( this->InterruptHasRaised( ) )
        {
            if( ( this->last_received_irq_mask & LR1110_SYSTEM_IRQ_GNSS_SCAN_DONE ) != 0 )
            {
                if( this->settings.capture_mode == DEMO_GNSS_SINGLE_SCAN_MODE )
                {
                    this->state = DEMO_GNSS_BASE_GET_RESULTS;
                }
                else
                {
                    this->state = DEMO_GNSS_BASE_WAIT_AND_EXECUTE_SECOND_SCAN;
                }

                this->signaling->StopCapture( );
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_GNSS_BASE_WAIT_FOR_SCAN;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_GNSS_BASE_WAIT_FOR_SCAN;
        }
        break;
    }

    case DEMO_GNSS_BASE_GET_RESULTS:
    {
        uint8_t                          nb_sv_detected;
        lr1110_gnss_detected_satellite_t list_of_sv[30] = { 0 };

        lr1110_gnss_get_result_size( this->device->GetRadio( ), &this->result.nav_message.size );
        if( this->result.nav_message.size <= GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH )
        {
            lr1110_gnss_read_results( this->device->GetRadio( ), this->result.nav_message.message,
                                      this->result.nav_message.size );
        }
        else
        {
            this->JumpToErrorState( DEMO_GNSS_BASE_NAV_MESSAGE_TOO_LONG );
            this->communication_interface->Log(
                "Error when fetching NAV message: size too long (max is %u, "
                "actual size is %u)\n",
                GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH, this->result.nav_message.size );
            break;
        }

        if( DemoTransceiverGnssInterface::CanFetchResults( this->result.nav_message ) )
        {
            lr1110_gnss_get_nb_detected_satellites( this->device->GetRadio( ), &nb_sv_detected );
            lr1110_gnss_get_detected_satellites( this->device->GetRadio( ), nb_sv_detected, list_of_sv );
            lr1110_gnss_timings_t tansceiver_timings = {};
            lr1110_gnss_get_timings( this->device->GetRadio( ), &tansceiver_timings );
            this->result.timings = DemoTransceiverGnssInterface::DemoTimingFromTransceiver( tansceiver_timings );

            this->result.consumption_uas =
                ( this->result.timings.radio_ms * DEMO_GNSS_CONSUMPTION_DCDC_ACQUISITION_MA +
                  this->result.timings.computation_ms * DEMO_GNSS_CONSUMPTION_DCDC_COMPUTATION_MA );

            this->result.local_instant_measurement                = measurement_instant_s;
            this->result.local_instant_measurement_second_capture = this->instant_second_capture_ms;

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

            this->state = DEMO_GNSS_BASE_TERMINATED;
        }
        else
        {
            const demo_gnss_error_t error_code =
                DemoTransceiverGnssInterface::GetHostErrorFromResult( this->result.nav_message );
            this->JumpToErrorState( error_code );
            this->communication_interface->Log( "Error during GNSS scan\n" );
        }
        break;
    }

    case DEMO_GNSS_BASE_TERMINATED:
    {
        this->state = DEMO_GNSS_BASE_INIT;
        this->Terminate( );
        break;
    }

    case DEMO_GNSS_BASE_ERROR:
    {
        this->state = DEMO_GNSS_BASE_TERMINATED;
        break;
    }
    }
}

void DemoTransceiverGnssInterface::SpecificStop( ) { signaling->StopCapture( ); }

bool DemoTransceiverGnssInterface::CanFetchResults( demo_gnss_nav_result_t& nav_message )
{
    if( DemoTransceiverGnssInterface::IsResultToSolver( nav_message ) )
    {
        return true;
    }
    else
    {
        const demo_gnss_error_t error_code = DemoTransceiverGnssInterface::GetHostErrorFromResult( nav_message );
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

void DemoTransceiverGnssInterface::Configure( demo_gnss_settings_t& config ) { this->settings = config; }

const demo_gnss_all_results_t* DemoTransceiverGnssInterface::GetResult( ) const { return &this->result; }

void DemoTransceiverGnssInterface::ClearRegisteredIrqs( ) const
{
    lr1110_system_clear_irq_status( this->device->GetRadio( ), this->gnss_irq );
}

void DemoTransceiverGnssInterface::SetAntennaSwitch( const demo_gnss_antenna_selection_t antenna_selection )
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

lr1110_gnss_date_t DemoTransceiverGnssInterface::GnssTimeFromEnvironment( environment_date_time_t date_time )
{
    lr1110_gnss_date_t gnss_time = date_time;
    return gnss_time;
}

void DemoTransceiverGnssInterface::AskAndStoreDate( )
{
    environment_location_t initial_position;
    lr1110_gnss_date_t     actual_time;

    const bool success = this->communication_interface->GetDateAndApproximateLocation(
        actual_time, initial_position.latitude, initial_position.longitude, initial_position.altitude );

    if( success == true )
    {
        this->GetEnvironment( )->SetTimeFromGpsEpoch( actual_time );
        this->GetEnvironment( )->SetLocation( initial_position );
    }
}

bool DemoTransceiverGnssInterface::IsResultToSolver( const demo_gnss_nav_result_t& nav_result )
{
    if( nav_result.size >= 2 )
    {
        return nav_result.message[0] == LR1110_GNSS_DESTINATION_SOLVER;
    }
    else
    {
        return false;
    }
}

bool DemoTransceiverGnssInterface::IsResultToHost( const demo_gnss_nav_result_t& nav_result )
{
    if( nav_result.size >= 2 )
    {
        return nav_result.message[0] == LR1110_GNSS_DESTINATION_HOST;
    }
    else
    {
        return false;
    }
}

demo_gnss_error_t DemoTransceiverGnssInterface::GetHostErrorFromResult( const demo_gnss_nav_result_t& nav_result )
{
    demo_gnss_error_t error = DEMO_GNSS_BASE_ERROR_UNKNOWN;
    if( DemoTransceiverGnssInterface::IsResultToHost( nav_result ) )
    {
        const lr1110_gnss_message_host_status_t status_code_raw =
            ( lr1110_gnss_message_host_status_t ) nav_result.message[1];
        switch( status_code_raw )
        {
        case LR1110_GNSS_HOST_NO_TIME:
        {
            error = DEMO_GNSS_BASE_ERROR_NO_DATE;
            break;
        }
        case LR1110_GNSS_HOST_NO_SATELLITE_DETECTED:
        {
            error = DEMO_GNSS_BASE_ERROR_NO_SATELLITE;
            break;
        }
        case LR1110_GNSS_HOST_ALMANAC_IN_FLASH_TOO_OLD:
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

uint16_t DemoTransceiverGnssInterface::GetAlmanacAgeFromGpsEpochInDays( const uint16_t almanac_age )
{
    // The almanac_age is the number of days since last GPS week number rollover
    // As we are in year 2020, our last rollover was on April 7th 2019. The next
    // one will be around 2038. So if you are reading this code because you
    // suspect mistake in the almanac age handling, and the current year is >
    // 2038, consider revisiting this piece of code...
    // (and now I'm a 50 year old guy... Greetings from the past ;) )

    // Let's convert in the number of days elapsed since GPS epoch (January
    // 6th 1980)
    const uint16_t days_between_2019_rollover_to_epoch = 14336;
    const uint16_t almanac_age_days_since_gps_epoch    = almanac_age + days_between_2019_rollover_to_epoch;
    return almanac_age_days_since_gps_epoch;
}

lr1110_gnss_search_mode_t DemoTransceiverGnssInterface::TransceiverSearchModeFromDemo(
    const demo_gnss_search_mode_t& demo_search_mode )
{
    lr1110_gnss_search_mode_t transceiver_search_mode = LR1110_GNSS_OPTION_DEFAULT;
    switch( demo_search_mode )
    {
    case DEMO_GNSS_OPTION_DEFAULT:
    {
        transceiver_search_mode = LR1110_GNSS_OPTION_DEFAULT;
        break;
    }

    case DEMO_GNSS_OPTION_BEST_EFFORT:
    {
        transceiver_search_mode = LR1110_GNSS_OPTION_BEST_EFFORT;
        break;
    }
    }
    return transceiver_search_mode;
}

lr1110_gnss_scan_mode_t DemoTransceiverGnssInterface::TransceiverScanModeFromDemo(
    const demo_gnss_scan_mode_t& demo_scan_mode )
{
    lr1110_gnss_scan_mode_t transceiver_scan_mode = LR1110_GNSS_SINGLE_SCAN_MODE;
    switch( demo_scan_mode )
    {
    case DEMO_GNSS_SINGLE_SCAN_MODE:
    {
        transceiver_scan_mode = LR1110_GNSS_SINGLE_SCAN_MODE;
        break;
    }

    case DEMO_GNSS_DOUBLE_SCAN_MODE:
    {
        transceiver_scan_mode = LR1110_GNSS_DOUBLE_SCAN_MODE;
        break;
    }
    }
    return transceiver_scan_mode;
}

demo_gnss_timings_t DemoTransceiverGnssInterface::DemoTimingFromTransceiver(
    const lr1110_gnss_timings_s& transceiver_timings )
{
    demo_gnss_timings_t demo_timings = {};
    demo_timings.computation_ms      = transceiver_timings.computation_ms;
    demo_timings.radio_ms            = transceiver_timings.radio_ms;
    return demo_timings;
}

uint16_t DemoTransceiverGnssInterface::GetAlmanacAgeDaysSinceToday( ) const
{
    // First, we get the age of the almanac of the satellite id 0
    uint16_t almanac_age = 0;
    lr1110_gnss_get_almanac_age_for_satellite( this->device->GetRadio( ), 0, &almanac_age );

    // Then we convert the age from almanac into number of days elapsed since
    // GPS epoch
    const uint16_t almanac_age_since_epoch =
        DemoTransceiverGnssInterface::GetAlmanacAgeFromGpsEpochInDays( almanac_age );

    // We get the actual time in days
    const time_t   now_seconds = this->environment->GetDateTime( );
    const uint16_t now_days    = now_seconds / 3600 / 24;

    const uint16_t almanac_age_since_update = now_days - almanac_age_since_epoch;
    return almanac_age_since_update;
}

bool DemoTransceiverGnssInterface::IsAlmanacTooOld( uint16_t almanac_age_days, const uint16_t days_limit )
{
    return almanac_age_days > days_limit;
}

bool DemoTransceiverGnssInterface::CheckAndStoreAlmanacAge( const uint16_t days_limit )
{
    const uint16_t almanac_age_days = this->GetAlmanacAgeDaysSinceToday( );

    const bool almanac_too_old = DemoTransceiverGnssInterface::IsAlmanacTooOld( almanac_age_days, days_limit );

    this->result.almanac_age_days = almanac_age_days;
    this->result.almanac_too_old  = almanac_too_old;

    return !almanac_too_old;
}
