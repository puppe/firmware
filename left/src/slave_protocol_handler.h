#ifndef __SLAVE_PROTOCOL_HANDLER_H__
#define __SLAVE_PROTOCOL_HANDLER_H__

// Includes:

    #include "fsl_port.h"

// Macros:

    #define SLAVE_RX_BUFFER_SIZE 100
    #define SLAVE_TX_BUFFER_SIZE 100

    #define PROTOCOL_RESPONSE_SUCCESS       0
    #define PROTOCOL_RESPONSE_GENERIC_ERROR 1

// Variables:

    uint8_t SlaveRxBuffer[SLAVE_RX_BUFFER_SIZE];
    uint8_t SlaveTxBuffer[SLAVE_TX_BUFFER_SIZE];
    uint8_t SlaveTxSize;

// Functions:

    extern void SlaveProtocolHandler(void);

#endif
