/**
 * @file      demo_gnss_base.h
 *
 * @brief     Definition of the components shared by GNSS demonstrations.
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

#ifndef __DEMO_GNSS_BASE_H__
#define __DEMO_GNSS_BASE_H__

#include "demo_base.h"
#include "environment_interface.h"
#include "antenna_selector_interface.h"
#include "timer_interface.h"
#include "log.h"

#define GNSS_DEMO_MAX_RESULT_TOTAL ( 32 )
#define GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH ( 259 )

typedef enum
{
    DEMO_GNSS_CONSTELLATION_BEIDOU,
    DEMO_GNSS_CONSTELLATION_GPS,
} demo_gnss_constellation_t;

typedef struct
{
    demo_gnss_constellation_t constellation;
    uint8_t                   satellite_id;
    int16_t                   snr;
} demo_gnss_single_result_t;

typedef struct
{
    uint8_t  message[GNSS_DEMO_NAV_MESSAGE_MAX_LENGTH];
    uint16_t size;
} demo_gnss_nav_result_t;

typedef enum
{
    DEMO_GNSS_BASE_NO_ERROR,
    DEMO_GNSS_BASE_ERROR_NO_DATE,
    DEMO_GNSS_BASE_ERROR_NO_LOCATION,
    DEMO_GNSS_BASE_ERROR_NO_SATELLITE,
    DEMO_GNSS_BASE_ERROR_ALMANAC_TOO_OLD,
    DEMO_GNSS_BASE_NAV_MESSAGE_TOO_LONG,
    DEMO_GNSS_BASE_ERROR_UNKNOWN,
} demo_gnss_error_t;

typedef struct
{
    demo_gnss_error_t         error;
    uint8_t                   nb_result;
    uint32_t                  consumption_uas;
    lr1110_gnss_timings_t     timings;
    demo_gnss_single_result_t result[GNSS_DEMO_MAX_RESULT_TOTAL];
    demo_gnss_nav_result_t    nav_message;
    uint32_t                  local_instant_measurement;
    uint32_t                  local_instant_measurement_second_capture;
    uint16_t                  almanac_age_days;
    bool                      almanac_too_old;
} demo_gnss_all_results_t;

typedef enum
{
    DEMO_GNSS_BASE_INIT,
    DEMO_GNSS_BASE_SCAN,
    DEMO_GNSS_BASE_WAIT_FOR_SCAN,
    DEMO_GNSS_BASE_WAIT_AND_EXECUTE_SECOND_SCAN,
    DEMO_GNSS_BASE_WAIT_FOR_SECOND_SCAN,
    DEMO_GNSS_BASE_GET_RESULTS,
    DEMO_GNSS_BASE_TERMINATED,
    DEMO_GNSS_BASE_ERROR,
} demo_gnss_state_t;

class DemoGnssBase : public DemoBase
{
   public:
    DemoGnssBase( radio_t* radio, SignalingInterface* signaling, EnvironmentInterface* environment,
                  AntennaSelectorInterface* antenna_selector, TimerInterface* timer );
    virtual ~DemoGnssBase( );

    virtual void Reset( );
    virtual void SpecificRuntime( );
    virtual void SpecificStop( );
    virtual void SpecificInterruptHandler( );

    void                           Configure( demo_gnss_settings_t& config );
    const demo_gnss_all_results_t* GetResult( ) const;

    static const char* state2str( const demo_gnss_state_t state );

   protected:
    virtual void ClearRegisteredIrqs( ) const;

    void                     JumpToErrorState( const demo_gnss_error_t error_code );
    demo_gnss_settings_t     GetSettings( ) const;
    EnvironmentInterface*    GetEnvironment( );
    void                     SetAntennaSwitch( const demo_gnss_antenna_selection_t antenna_selection );
    static bool              IsResultToSolver( const demo_gnss_nav_result_t& nav_result );
    static bool              IsResultToHost( const demo_gnss_nav_result_t& nav_result );
    static demo_gnss_error_t GetHostErrorFromResult( const demo_gnss_nav_result_t& nav_result );
    static bool              CanFetchResults( demo_gnss_nav_result_t& nav_message );

    static uint16_t GetAlmanacAgeFromGpsEpochInDays( const uint16_t almanac_age );

    /*!
     * \brief Check wether the almanac is too old or not
     *
     * \param [in] now_days Actual date in number of days elapsed since GPS
     * epoch (January 6th 1980)
     *
     * \param [in] almanac_age_days Age of the almanac expressed in number of
     * days since GPS epoch
     *
     * \param [in] days_limit The maximal number of days before considering the
     * almanac too old
     *
     * \retval false The almanac is too old
     *
     * \retval true The almanac is not too old
     */
    static bool IsAlmanacTooOld( uint16_t almanac_age_days, const uint16_t days_limit );

    uint16_t GetAlmanacAgeDaysSinceToday( ) const;

    /*!
     * \brief Check wether the almanac is too old or not
     *
     * Internally calls IsAlmanacTooOld and GetAlmanacAgeDaysSinceToday
     *
     * \param [in] days_limit The maximal number of days before considering the
     * almanac too old
     *
     * \retval false The almanac is too old
     *
     * \retval true The almanac is not too old
     */
    bool CheckAndStoreAlmanacAge( const uint16_t days_limit );

    /*!
     * \brief Execute scan call
     *
     * This method must be implemented in base class. It just needs to call
     * the call scan driver function.
     */
    virtual void CallScan( ) = 0;

    static lr1110_gnss_date_t GnssTimeFromEnvironment( environment_date_time_t date_time );
    void                      AskAndStoreDate( void );
    Logging                   log;
    TimerInterface*           timer;

   private:
    uint32_t                  gnss_irq;
    demo_gnss_state_t         state;
    demo_gnss_all_results_t   result;
    EnvironmentInterface*     environment;
    demo_gnss_settings_t      settings;
    uint8_t                   inter_capture_delay_s;
    uint16_t                  instant_start_capture_ms;
    uint16_t                  instant_second_capture_ms;
    AntennaSelectorInterface* antenna_selector;
};

#endif  // __DEMO_GNSS_BASE_H__
