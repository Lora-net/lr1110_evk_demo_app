#include "demo_modem_file_upload.h"
#include "lr1110_modem_lorawan.h"
#include "demo_file_upload_assets.h"

#define DEMO_MODEM_FILE_UPLOAD_INDEX ( 25 )
#define DEMO_MODEM_FILE_UPLOAD_MAX_BYTES_PER_CHUNK ( 255 )

DemoModemFileUpload::DemoModemFileUpload( DeviceModem* device, SignalingInterface* signaling,
                                          CommunicationInterface*       communication_interface,
                                          EnvironmentInterface*         environment,
                                          ConnectivityManagerInterface* connectivity )
    : DemoModemInterface( device, signaling, communication_interface ),
      state( DEMO_MODEM_FILE_UPLOAD_STATE_INIT ),
      environment( environment ),
      connectivity( connectivity ),
      result( { DEMO_MODEM_FILE_UPLOAD_NOT_TERMINATED } ),
      has_intermediate_results( false )
{
}

DemoModemFileUpload::~DemoModemFileUpload( ) {}

void DemoModemFileUpload::Reset( ) { this->DemoInterface::Reset( ); }

void DemoModemFileUpload::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_MODEM_FILE_UPLOAD_STATE_INIT:
    {
        this->communication_interface->Log( "File Upload Init\n" );

        this->has_intermediate_results  = false;
        this->result.termination_status = DEMO_MODEM_FILE_UPLOAD_NOT_TERMINATED;
        lr1110_modem_upload_init( this->device->GetRadio( ), DEMO_MODEM_FILE_UPLOAD_INDEX,
                                  LR1110_MODEM_SERVICES_ENCRYPTION_DISABLE, DEMO_FILE_UPLOAD_ASSETS_FILE0_SIZE, 1 );

        const uint32_t crc =
            this->compute_crc( 0, ( char* ) demo_file_upload_asset_file_0, DEMO_FILE_UPLOAD_ASSETS_FILE0_SIZE );
        uint16_t remaining_file_data_to_send = DEMO_FILE_UPLOAD_ASSETS_FILE0_SIZE;
        uint8_t* p_data_to_send              = demo_file_upload_asset_file_0;
        while( remaining_file_data_to_send > DEMO_MODEM_FILE_UPLOAD_MAX_BYTES_PER_CHUNK )
        {
            lr1110_modem_upload_data( this->device->GetRadio( ), p_data_to_send,
                                      DEMO_MODEM_FILE_UPLOAD_MAX_BYTES_PER_CHUNK );
            p_data_to_send += DEMO_MODEM_FILE_UPLOAD_MAX_BYTES_PER_CHUNK;
            remaining_file_data_to_send -= DEMO_MODEM_FILE_UPLOAD_MAX_BYTES_PER_CHUNK;
        }
        if( remaining_file_data_to_send > 0 )
        {
            lr1110_modem_upload_data( this->device->GetRadio( ), p_data_to_send, remaining_file_data_to_send );
        }
        lr1110_modem_upload_start( this->device->GetRadio( ), crc );

        this->SetWaitingForInterrupt( );
        this->state = DEMO_MODEM_FILE_UPLOAD_STATE_WAIT_TERMINATION;
        break;
    }

    case DEMO_MODEM_FILE_UPLOAD_STATE_WAIT_TERMINATION:
    {
        if( this->InterruptHasRaised( ) )
        {
            this->communication_interface->Log( "File Upload received interrupt\n" );
            if( this->last_received_event.event_type == LR1110_MODEM_LORAWAN_EVENT_UPLOAD_DONE )
            {
                this->result.termination_status = ( this->last_received_event.buffer[0] == 0x01 )
                                                      ? DEMO_MODEM_FILE_UPLOAD_TERMINATED_SUCCESS
                                                      : DEMO_MODEM_FILE_UPLOAD_TERMINATED_TIMEOUT;
                this->state                     = DEMO_MODEM_FILE_UPLOAD_STATE_TERMINATED;
            }
            else
            {
                this->SetWaitingForInterrupt( );
                this->state = DEMO_MODEM_FILE_UPLOAD_STATE_WAIT_TERMINATION;
            }
        }
        else
        {
            this->SetWaitingForInterrupt( );
            this->state = DEMO_MODEM_FILE_UPLOAD_STATE_WAIT_TERMINATION;
        }
        break;
    }

    case DEMO_MODEM_FILE_UPLOAD_STATE_TERMINATED:
    {
        this->state = DEMO_MODEM_FILE_UPLOAD_STATE_INIT;
        this->Terminate( );
        break;
    }
    }
}

void DemoModemFileUpload::SpecificStop( ) { this->state = DEMO_MODEM_FILE_UPLOAD_STATE_INIT; }

demo_modem_file_upload_results_t* DemoModemFileUpload::GetResultsAndResetIntermediateFlag( )
{
    this->has_intermediate_results = false;
    return &this->result;
}

void DemoModemFileUpload::Configure( ) {}

bool DemoModemFileUpload::HasIntermediateResults( ) const { return this->has_intermediate_results; }

uint32_t DemoModemFileUpload::compute_crc( uint32_t crc, const char* buf, uint16_t len )
{
    static uint32_t table[256];
    uint32_t        rem;
    uint8_t         octet;
    int             i, j;
    const char *    p, *q;

    for( i = 0; i < 256; i++ )
    {
        rem = i; /* remainder from polynomial division */
        for( j = 0; j < 8; j++ )
        {
            if( rem & 1 )
            {
                rem >>= 1;
                rem ^= 0xedb88320;
            }
            else
                rem >>= 1;
        }
        table[i] = rem;
    }
    crc = ~crc;
    q   = buf + len;
    for( p = buf; p < q; p++ )
    {
        octet = *p; /* Cast to unsigned octet. */
        crc   = ( crc >> 8 ) ^ table[( crc & 0xff ) ^ octet];
    }
    return ~crc;
}
