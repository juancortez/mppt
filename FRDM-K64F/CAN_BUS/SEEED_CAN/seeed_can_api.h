/* seeed_can_api.h
 * Copyright (c) 2013 Sophie Dexter
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _SEEED_CAN_API_H_
#define _SEEED_CAN_API_H_

#include "k64f_can_helper.h"
#include "seeed_can_spi.h"

// print debug information...
//#define DEBUG

#ifdef __cplusplus
extern "C" {
#endif

/** CAN driver typedefs
 */

// The 'MCP_xyz' structs mimic MCP register organisations
// A union of the struct and a simple array can be used to:
//  Process and manipulate the struct in an ordered way
//  Copy the struct to/from the MCP2512 as an array

/// Type definition to hold an MCP2515 Timing Register set structure
    struct MCP_CANtiming {
        uint8_t phseg2    : 3;      // PS2 length bits 2..0 (PHSEG2 + 1) Tq (minimum valid setting is 2 Tq)
        uint8_t reserved1 : 3;      // Unused bits (read as '0')
        uint8_t wakfil    : 1;      // Wake-up filter enable bit (1 = enabled, 0 = disabled)
        uint8_t sof       : 1;      // Start of frame bit (1 = CLKOUT pin is SOF, 0 = CLKOUT is clock)
        uint8_t prseg     : 3;      // Propagation Segment length bits 2..0 (PRSEG + 1) Tq (minimum valid setting is 1 Tq)
        uint8_t phseg1    : 3;      // PS2 length bits 2..0 (PRSEG + 1) Tq (minimum valid setting is 1 Tq)
        uint8_t sam       : 1;      // Sample Point Configuration bit (1 = Bus line is sampled 3 times, 0 = sampled once)
        uint8_t btlmode   : 1;      // PS2 Bit Time Length (1 = determined by phseg2, 0 = greater of PS1 an IPT (2 Tq))
        uint8_t brp       : 6;      // Baud Rate Prescaler bits 5..0 (Tq = 2 x (BRP + 1) / Fosc)
        uint8_t sjw       : 2;      // Synchronisation Jump Width Length bits 1..0 ((SJW + 1) Tq)
    };
    typedef struct MCP_CANtiming CANtiming;

/// Type definition to hold an MCP2515 CAN id structure
    struct MCP_CANid {
        uint8_t sid10_3   : 8;      // Bits 10..3 of a standard identifier
        uint8_t eid17_16  : 2;      // Bits 17..16 of an extended identifier
        uint8_t reserved1 : 1;      // Unused bit in RXBbSIDL
        uint8_t ide       : 1;      // Extended Identifier Flag
        // CANformat 'type'
        uint8_t srtr      : 1;      // Standard Frame Remote Transmit Request Bit (valid when IDE = 0)
        // CANtype 'type'
        // Unimplimented  in TXBnSIDL, filters and acceptance masks
        uint8_t sid2_0    : 3;      // Bits 2..0 of a standard identifier
        uint8_t eid15_8   : 8;      // Bits 15..8 of an extended identifier
        uint8_t eid7_0    : 8;      // Bits 7..0 of an extended identifier
    };
    typedef struct MCP_CANid CANid;

/// Type definition to hold an MCP2515 CAN id structure
    struct MCP_CANMsg {
        CANid id;
        uint8_t dlc       : 4;      // Bits 3..0: DLC - Data Length Counter
        uint8_t reserved1 : 2;      // unused bits in RXBnDLC
        uint8_t ertr      : 1;      // Extended Frame Remote Transmit Request Bit (valid when IDE = 1)
        // CANtype 'type'
        uint8_t reserved2 : 1;      // unused bit in RXBnDLC
        uint8_t data[8];            // CAN Message Data Bytes 0-7
    };
    typedef struct MCP_CANMsg CANMsg;

/* #ifndef MBED_CAN_HELPER_H             // These are already defined for some mbed family members */
/* #ifdef DEVICE_CAN                     // needs both can_helper and DEVICE_CAN */
/*     enum CANFormat { */
/*         CANStandard = 0, */
/*         CANExtended = 1 */
/*     }; */
/*     typedef enum CANFormat CANFormat; */

/*     enum CANType { */
/*         CANData   = 0, */
/*         CANRemote = 1 */
/*     }; */
/*     typedef enum CANType CANType; */

/*     struct CAN_Message { */
/*         unsigned int   id;          // 11 or 29 bit identifier */
/*         unsigned char  data[8];     // Data field */
/*         unsigned char  len;         // Length of data field in bytes */
/*         CANFormat      format;      // 0 - STANDARD, 1- EXTENDED IDENTIFIER */
/*         CANType        type;        // 0 - DATA FRAME, 1 - REMOTE FRAME */
/*     }; */
/*     typedef struct CAN_Message CAN_Message; */
/* #endif */
/* #endif */
    
    enum MCP_Mode {
        _M_NORMAL,
        _M_SLEEP,
        _M_LOOPBACK,
        _M_MONITOR,
        _M_CONFIG,
        _M_RESET,
    };
    typedef MCP_Mode CANMode;

    enum MCP_Error_Flags {
        _E_ALL,
        _E_ERRORS,
        _E_WARNINGS,
        _E_RX1OVR,
        _E_RX0OVR,
        _E_TXBO,
        _E_TXEP,
        _E_RXEP,
        _E_TXWAR,
        _E_RXWAR,
        _E_EWARN
    };
    typedef MCP_Error_Flags CANFlags;

    enum MCP_Interrupt_Flags {
        _I_NONE,
        _I_ANY,
        _I_RX,
        _I_TX,
        _I_RX0,
        _I_RX1,
        _I_TX0,
        _I_TX1,
        _I_TX2,
        _I_ERROR,
        _I_WAKE,
        _I_M_ERR
    };
    typedef MCP_Interrupt_Flags CANIrqs;

/** CAN driver functions
 */
    uint8_t mcpInit(mcp_can_t *obj,                                     // Initialise the MCP2515 and set the bit rate
                    const uint32_t bitRate,
                    const CANMode mode);
    uint8_t mcpSetMode(mcp_can_t *obj, const uint8_t newmode);          // set the MCP2515's operation mode
    uint8_t mcpSetBitRate(mcp_can_t *obj, const uint32_t bitRate);      // set bitrate
    
    void mcpWriteId(mcp_can_t *obj,                                     // write a CAN id
                    const uint8_t mcp_addr,
                    const uint8_t ext,
                    const uint32_t id );
    uint8_t mcpCanRead(mcp_can_t *obj, CAN_Message *msg);               // read a CAN message
    uint8_t mcpCanWrite(mcp_can_t *obj, CAN_Message msg);               // write a CAN message
    
    uint8_t mcpInitMask(mcp_can_t *obj,                                 // initialise an Acceptance Mask
                        uint8_t num,
                        uint32_t ulData,
                        bool ext);
    uint8_t mcpInitFilter(mcp_can_t *obj,                               // initialise an Acceptance Filter
                          uint8_t num,
                          uint32_t ulData,
                          bool ext);

    uint8_t mcpErrorType(mcp_can_t *obj, const CANFlags type);          // Report on the specified errors and warnings
    uint8_t mcpErrorFlags(mcp_can_t *obj);                              // Return contents of the error and warning flags register
    uint8_t mcpReceptionErrorCount(mcp_can_t *obj);                     // number of message reception errors
    uint8_t mcpTransmissionErrorCount(mcp_can_t *obj);                  // number of message transmission errors

    void mcpMonitor(mcp_can_t *obj, const bool silent);                 // Select between monitor (silent = 1) and normal (silent = 0) modes
    uint8_t mcpMode(mcp_can_t *obj, const CANMode mode);                // Change CAN operation to the specified mode

    void mcpSetInterrupts(mcp_can_t *obj, const CANIrqs irqSet);        // Configure interrupt sources
    uint8_t mcpInterruptType(mcp_can_t *obj, const CANIrqs irqFlag);    // Report on the specified interrupt causes
    uint8_t mcpInterruptFlags(mcp_can_t *obj);                          // Return contents of the interrupt flags register
    
#ifdef __cplusplus
};
#endif

#endif    // SEEED_CAN_API_H
