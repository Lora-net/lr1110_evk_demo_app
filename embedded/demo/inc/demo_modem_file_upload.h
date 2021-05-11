/**
 * @file      demo_modem_file upload.h
 *
 * @brief     Definition of the demo modem with file upload.
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

#ifndef DEMO_MODEM_FILE_UPLOAD_H
#define DEMO_MODEM_FILE_UPLOAD_H

#include "demo_modem_interface.h"
#include "environment_interface.h"
#include "connectivity_manager_interface.h"

typedef enum
{
    DEMO_MODEM_FILE_UPLOAD_NOT_TERMINATED,
    DEMO_MODEM_FILE_UPLOAD_TERMINATED_SUCCESS,
    DEMO_MODEM_FILE_UPLOAD_TERMINATED_TIMEOUT,
} demo_modem_file_upload_termination_status_t;

typedef struct
{
    demo_modem_file_upload_termination_status_t termination_status;
} demo_modem_file_upload_results_t;

typedef enum
{
    DEMO_MODEM_FILE_UPLOAD_STATE_INIT,
    DEMO_MODEM_FILE_UPLOAD_STATE_WAIT_TERMINATION,
    DEMO_MODEM_FILE_UPLOAD_STATE_TERMINATED,
} demo_modem_file_upload_state_t;

class DemoModemFileUpload : public DemoModemInterface
{
   public:
    DemoModemFileUpload( DeviceModem* device, SignalingInterface* signaling,
                         CommunicationInterface* communication_interface, EnvironmentInterface* environment,
                         ConnectivityManagerInterface* connectivity );
    virtual ~DemoModemFileUpload( );

    void                              Reset( ) override;
    void                              SpecificRuntime( ) override;
    void                              SpecificStop( ) override;
    demo_modem_file_upload_results_t* GetResultsAndResetIntermediateFlag( );
    void                              Configure( );
    bool                              HasIntermediateResults( ) const override;

   protected:
    static uint32_t compute_crc( uint32_t crc, const char* buf, uint16_t len );

   private:
    demo_modem_file_upload_state_t   state;
    EnvironmentInterface*            environment;
    ConnectivityManagerInterface*    connectivity;
    demo_modem_file_upload_results_t result;
    bool                             has_intermediate_results;
};

#endif  // DEMO_MODEM_FILE_UPLOAD_H
