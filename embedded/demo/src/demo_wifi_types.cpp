#include "demo_wifi_types.h"

#define WIFI_TYPE_FROM_TRANSCEIVER_CASE( var, type ) \
    case LR1110_WIFI_TYPE_RESULT_##type:             \
    {                                                \
        var = DEMO_WIFI_TYPE_##type;                 \
        break;                                       \
    }
demo_wifi_signal_type_t demo_wifi_types_from_transceiver( lr1110_wifi_signal_type_result_t wifi_type_result )
{
    demo_wifi_signal_type_t demo_wifi_type = DEMO_WIFI_TYPE_B;
    switch( wifi_type_result )
    {
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, B )
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, G )
        WIFI_TYPE_FROM_TRANSCEIVER_CASE( demo_wifi_type, N )
    }
    return demo_wifi_type;
}
#undef WIFI_TYPE_FROM_TRANSCEIVER_CASE