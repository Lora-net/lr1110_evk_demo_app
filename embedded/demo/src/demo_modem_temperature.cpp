#include "demo_modem_temperature.h"
#include "lis2de12.h"

#define DEMO_MODEM_TEMPERATURE_ACC_BUFFER_LENGTH ( 11 )
#define DEMO_MODEM_TEMPERATURE_LORAWAN_PORT ( 102 )

DemoModemTemperature::DemoModemTemperature( DeviceModem* device, SignalingInterface* signaling,
                                            CommunicationInterface*       communication_interface,
                                            EnvironmentInterface*         environment,
                                            ConnectivityManagerInterface* connectivity )
    : DemoModemInterface( device, signaling, communication_interface ),
      state( DEMO_MODEM_TEMPERATURE_STATE_INIT ),
      environment( environment ),
      connectivity( connectivity ),
      result( { 0, false } ),
      has_intermediate_results( false )
{
}

DemoModemTemperature::~DemoModemTemperature( ) {}

void DemoModemTemperature::Reset( ) { this->DemoInterface::Reset( ); }

void DemoModemTemperature::SpecificRuntime( )
{
    switch( this->state )
    {
    case DEMO_MODEM_TEMPERATURE_STATE_INIT:
    {
        this->has_intermediate_results = false;
        this->result.sent              = false;
        this->result.temperature       = 0.00;
        this->state                    = DEMO_MODEM_TEMPERATURE_STATE_GET_AND_SEND_TEMPERATURE;
        break;
    }

    case DEMO_MODEM_TEMPERATURE_STATE_GET_AND_SEND_TEMPERATURE:
    {
        ACC_ReadRawData( );
        const float   raw_temperature = this->fetch_temperature( );
        const uint8_t temperature =
            ( uint8_t ) raw_temperature;  // WARNING: Here the temperature that is a uint16 is converted to a uint8...
        this->connectivity->RequestTxUnconfirmed( DEMO_MODEM_TEMPERATURE_LORAWAN_PORT, &temperature, 1 );

        // Store the raw temperature
        this->result.temperature       = raw_temperature;
        this->has_intermediate_results = true;

        this->state = DEMO_MODEM_TEMPERATURE_STATE_WAIT_EVENT_DONE;
        break;
    }

    case DEMO_MODEM_TEMPERATURE_STATE_WAIT_EVENT_DONE:
    {
        if( this->InterruptHasRaised( ) == true )
        {
            if( this->last_received_event.event_type == LR1110_MODEM_LORAWAN_EVENT_TX_DONE )
            {
                this->communication_interface->Log( "Received Tx Done\n" );
                this->result.sent = true;
                this->state       = DEMO_MODEM_TEMPERATURE_STATE_TERMINATED;
            }
        }
        break;
    }

    case DEMO_MODEM_TEMPERATURE_STATE_TERMINATED:
    {
        this->state = DEMO_MODEM_TEMPERATURE_STATE_INIT;
        this->Terminate( );
        break;
    }
    }
}

void DemoModemTemperature::SpecificStop( ) { this->state = DEMO_MODEM_TEMPERATURE_STATE_INIT; }

demo_modem_temperature_results_t* DemoModemTemperature::GetResultsAndResetIntermediateFlag( )
{
    this->has_intermediate_results = false;
    return &this->result;
}

void DemoModemTemperature::Configure( ) {}

bool DemoModemTemperature::HasIntermediateResults( ) const { return this->has_intermediate_results; }

float DemoModemTemperature::fetch_temperature( ) const
{
    return lis2de12_from_lsb_to_celsius( acc_get_temperature( ) );
}