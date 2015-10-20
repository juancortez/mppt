/* seeed_can_api.cpp
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
 
#include "seeed_can_api.h"
 
/** Initialise the MCP2515 and set the bit rate
 */
uint8_t mcpInit(mcp_can_t *obj, const uint32_t bitRate, const CANMode mode)
{
    union {                                                             // Access CANMsg as:
        CANMsg x;                                                       // the organised struct
        uint8_t y[];                                                    // or contiguous memory array
    };
    uint8_t maskFilt[8] = { MCP_RXM0SIDH, MCP_RXM1SIDH, MCP_RXF0SIDH, MCP_RXF1SIDH, MCP_RXF2SIDH, MCP_RXF3SIDH, MCP_RXF4SIDH, MCP_RXF5SIDH };
    uint8_t canBufCtrl[5] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL, MCP_RXB0CTRL, MCP_RXB1CTRL };
    uint8_t canBuffer[3] = { MCP_TXB0CTRL+1, MCP_TXB1CTRL+1, MCP_TXB2CTRL+1 };
 
#ifdef DEBUG
    printf("Reseting MCP2515\r\n");
#endif
    mcpReset(obj);
    for (uint32_t i = 0; i < 8; i++) {                                  // Clear all CAN id masks and filters
        mcpWriteId(obj, maskFilt[i], NULL, NULL);
    }
    for (uint32_t i = 0; i < 5; i++) {                                  // Clear all CAN buffer control registers
        mcpWrite(obj, canBufCtrl[i], NULL);
    }
    for (uint32_t i = 0; i < sizeof(x); i++) y[i] = NULL;               // Initialise empty CAN message buffer
    for (uint32_t i = 0; i < 3; i++) {                                  // Clear all CAN TX buffers
        mcpWriteMultiple(obj, canBuffer[i], y, sizeof(x) );             // using empty CAN message (as an array)
    }
    // enable both receive-buffers, using filters to receive messages with std. and ext. identifiers that meet the filter criteria and enable rollover from RXB0 to RXB1 if RXB0 is full
    mcpBitModify(obj, MCP_RXB0CTRL, MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK, MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );
    mcpBitModify(obj, MCP_RXB1CTRL, MCP_RXB_RX_MASK, MCP_RXB_RX_STDEXT);
#ifdef DEBUG
    printf("Setting bit rate\r\n");
#endif
    if (!mcpSetBitRate(obj, bitRate)) {                                 // set baudrate
        return 0;
    }
//    return mcpSetMode(obj, MODE_NORMAL) ? 1 : 0;                        // set Normal mode and return
    return mcpSetMode(obj, mode) ? 1 : 0;                        // set Normal mode and return
}
 
/**  set MCP2515 operation mode
 *
 * Configuration, Normal, Sleep, Listen-only or Loopback
 */
uint8_t mcpSetMode(mcp_can_t *obj, const uint8_t newmode)
{
    mcpBitModify(obj, MCP_CANCTRL, MODE_MASK, newmode);
    for (uint32_t i = 0; i<10; i++) {
        if ((mcpRead(obj, MCP_CANSTAT) & MODE_MASK) == newmode) {
#ifdef DEBUG
            printf("Successfully entered mode: %02x time: %dms\r\n", newmode, i);
            printf("CANCTRL:%02x CANSTAT:%02x TXB0:%02x TXB1:%02x TXB2:%02x\r\n", mcpRead(obj, MCP_CANCTRL),  mcpRead(obj, MCP_CANSTAT), mcpRead(obj, MCP_TXB0CTRL), mcpRead(obj, MCP_TXB1CTRL), mcpRead(obj, MCP_TXB2CTRL));
#endif
            return 1;
        }
        wait_ms(1);
    }
#ifdef DEBUG
    printf("Failed to enter mode: %02x\r\n", newmode);
    printf("CANCTRL:%02x CANSTAT:%02x TXB0:%02x TXB1:%02x TXB2:%02x\r\n", mcpRead(obj, MCP_CANCTRL),  mcpRead(obj, MCP_CANSTAT), mcpRead(obj, MCP_TXB0CTRL), mcpRead(obj, MCP_TXB1CTRL), mcpRead(obj, MCP_TXB2CTRL));
#endif
    return 0;
}
 
/** set the CAN bus bitrate
 *
 * Calculate suitable BTR register values.
 * The Bit Rate Pre-scaler (BRP) can be in the range of 1-64.
 * According to CANopen, Bit Time can be be between 25 and 8 Time Quanta (TQU).
 * Bit Time = SyncSeg(1 TQU) + PropSeg(1-8 TQU) + PhaseSeg1(1-8 TQU) + PhaseSeg2(2-8 TQU).
 * SyncSeg is always 1TQU, PhaseSeg2 must be at least 2TQU to be longer than the processing time.
 * Opinions vary on when to take a sample but a point roughly 2/3 of Bit Time seems OK.
 * Synchronisation Jump width can be 1-4 TQU, a value of 1 seems to be normal.
 *
 * All register values are -1, e.g. PropSeg can range from 1-8 TQU, so values are 0-7 (0-63 for BRP).
 *
 * This table has the sampling points as close to 2/3 (66.7%) as possible.
 * The first value is PropSeg, 2nd PhaseSeg1.
 * PhaseSeg2 will be the same as PhaseSeg1 when btlmode bit is initialised to 0.
 */
static const uint8_t timing_pts[18][2] = {
    {0x0, 0x2},    // 8,  62.5%
    {0x1, 0x2},    // 9,  66.7%
    {0x2, 0x2},    // 10, 70.0%
    {0x1, 0x3},    // 11, 63.6%
    {0x2, 0x3},    // 12, 66.7%
    {0x3, 0x3},    // 13, 69.2%
    {0x2, 0x4},    // 14, 64.3%
    {0x3, 0x4},    // 15, 66.7%
    {0x4, 0x4},    // 16, 68.75%
    {0x3, 0x5},    // 17, 64.7%
    {0x4, 0x5},    // 18, 66.7%
    {0x5, 0x5},    // 19, 63.2%
    {0x4, 0x6},    // 20, 65.0%
    {0x5, 0x6},    // 21, 66.7%
    {0x6, 0x6},    // 22, 68.2%
    {0x5, 0x7},    // 23, 65.2
    {0x6, 0x7},    // 24, 66.7%
    {0x7, 0x7},    // 25, 68.0%
};
 
uint8_t mcpSetBitRate(mcp_can_t *obj, const uint32_t bitRate)
{
    union {                                                             // Access CANtiming as:
        CANtiming x;                                                    // the organised struct
        uint8_t y[];                                                    // or contiguous memory array
    };
    uint32_t bestBRP = 0;
    uint32_t bestTQU = 0;
    uint32_t bestCanRate = 0;
    uint32_t minBRP = (MCP_CLOCK_FREQ / (2 * MCP_MAX_TIME_QUANTA * bitRate));
    uint32_t maxBRP = (MCP_CLOCK_FREQ / (2 * MCP_MIN_TIME_QUANTA * bitRate));
 
#ifdef DEBUG
    printf("Setting configuration mode\r\n");
#endif
    uint8_t initialMode = mcpRead(obj, MCP_CANCTRL) & MODE_MASK;        // Store the current operation mode
    if(!mcpSetMode(obj, MODE_CONFIG)) {                             // Go into configuration mode
        return 0;
    }
 
    for (uint32_t i = 0; i < sizeof(x); i++) y[i] = NULL;               // Initialise CANtiming (btlmode, sjw and sam all = 0)
    if ((bitRate < CAN_MIN_RATE) || (bitRate > CAN_MAX_RATE)) {
#ifdef DEBUG
        printf("FAILED!! The requested Bit Rate is too high or too low: %d\r\n", bitRate);
#endif
        return 0;                                                       // Cannot set the requested bit rate!
    }
    minBRP = (minBRP == 0) ? MCP_MIN_PRESCALER : minBRP;
    maxBRP = (maxBRP > MCP_MAX_PRESCALER) ? MCP_MAX_PRESCALER : maxBRP;
    for (uint32_t BRP = minBRP; BRP < (maxBRP + 1); BRP++) {
        uint32_t timeQuanta = (MCP_CLOCK_FREQ / (2 * BRP * bitRate));
        if ((timeQuanta >= MCP_MIN_TIME_QUANTA) && (timeQuanta <= MCP_MAX_TIME_QUANTA)) {
            for (uint32_t TQU = timeQuanta; TQU <= MCP_MAX_TIME_QUANTA; TQU++) {
                uint32_t thisCanRate = MCP_CLOCK_FREQ / (2 * BRP * TQU);
                if ( abs((int)bitRate - (int)thisCanRate) < abs((int)bitRate - (int)bestCanRate)) {
                    bestCanRate = thisCanRate;
                    bestBRP= BRP;
                    bestTQU= TQU;
                }
            }
        }
    }
    x.brp = (bestBRP - 1);
    x.prseg = (timing_pts[bestTQU - 8][0]);
    x.phseg1 = (timing_pts[bestTQU - 8][1]);
    mcpWriteMultiple(obj,  MCP_CNF3, y, sizeof(x) );                    // Copy CANtiming to the MCP2515 (as an array)
#ifdef DEBUG
    printf("minBRP %d maxBRP %d\r\n", minBRP, maxBRP);
    printf("Bitrate: %d\tactualBitRate: %d\t Error: %1.2f percent.\r\n", bitRate, bestCanRate, (100-(100*(float)bitRate/(float)bestCanRate)));
    printf("TimeQuanta: %d\tbitRatePrescaler: %d\tSamplePoint: %2.2f percent\r\n", bestTQU, bestBRP, 100*(float)(3 + x.prseg + x.phseg1)/(float)bestTQU ) ;
    printf("Syncseg: 1\tPropSeg: %d\tPhaseSeg1: %d\tPhaseSeg2: %d\r\n", (x.prseg+1), (x.phseg1+1), (x.phseg1+1));
    printf("Setting normal mode\r\n");
#endif
    return (mcpSetMode(obj, initialMode)) ? 1 : 0;                      // desired bit rate set enter normal mode and return
}
 
/** write a CAN id to a mask, filter or transmit buffer
 */
void mcpWriteId(mcp_can_t *obj, const uint8_t mcp_addr, const uint8_t ext, const uint32_t id )
{
    union {                                                             // Access CANid as:
        CANid x;                                                        // the organised struct
        uint8_t y[];                                                    // or contiguous memory array
    };
 
    for (uint32_t i = 0; i < sizeof(x); i++) y[i] = NULL;               // Initialise CANid structure
    x.ide = ext;                                                        // Extended Identifier Flag
    if (x.ide == CANExtended) {
        x.sid10_3  = (uint8_t) (id >> 21);                              // SID10..3
        x.sid2_0   = (uint8_t) (id >> 18) & 0x07;                       // SID2..0
        x.eid17_16 = (uint8_t) (id >> 16) & 0x03;                       // EID17..16
        x.eid15_8  = (uint8_t) (id >> 8);                               // EID15..8
        x.eid7_0   = (uint8_t) id;                                      // EID7..0
    } else {
        x.sid10_3  = (uint8_t) (id >> 3);                               // SID10..3
        x.sid2_0   = (uint8_t) (id & 0x07);                             // SID2..0
    }
#ifdef DEBUG
    printf("sizeof CanIdStruct: %d bytes\r\n", sizeof(x));
    printf("sid10_3: %x\r\n", x.sid10_3);
    printf("eid17_16: %x\r\n", x.eid17_16);
    printf("ide: %x\r\n", x.ide);
    printf("srtr: %x\r\n", x.srtr);
    printf("sid2_0: %x\r\n", x.sid2_0);
    printf("eid15_8: %x\r\n", x.eid15_8);
    printf("eid7_0: %x\r\n", x.eid7_0);
#endif
    mcpWriteMultiple(obj,  mcp_addr, y, sizeof(x) );                     // Copy CANid to the MCP2515 (as an array)
}
 
/**  write a CAN message to the MCP2515
 */
uint8_t mcpCanWrite(mcp_can_t *obj, CAN_Message msg)
{
    union {                                                             // Access CANMsg as:
        CANMsg x;                                                       // the organised struct
        uint8_t y[];                                                    // or contiguous memory array
    };
    uint8_t bufferCommand[] = {MCP_WRITE_TX0, MCP_WRITE_TX1, MCP_WRITE_TX2};
    uint8_t rtsCommand[] = {MCP_RTS_TX0, MCP_RTS_TX1, MCP_RTS_TX2};
    uint8_t status = mcpStatus(obj);
    uint32_t num = 0;
// Check if there is a free message buffer
    if (!(status & MCP_STAT_TX0REQ)) {                                  // TX Message Buffer 0 free?
        num = 0;
    } else if (!(status & MCP_STAT_TX1REQ)) {                           // TX Message Buffer 1 free?
        num = 1;
    } else if (!(status & MCP_STAT_TX2REQ)) {                           // TX Message Buffer 2 free?
        num = 2;
    } else {
        return 0;                                                       // No free transmit buffers in the MCP2515 CAN controller chip
    }
// populate CANMsg structure
    for (uint32_t i = 0; i < sizeof(x); i++) y[i] = NULL;               // Initialise CANMsg structure
    x.id.ide = msg.format;                                              // Extended Identifier Flag
    if (x.id.ide == CANExtended) {
        x.id.sid10_3  = (uint8_t) (msg.id >> 21);                       // SID10..3
        x.id.sid2_0   = (uint8_t) (msg.id >> 18) & 0x07;                // SID2..0
        x.id.eid17_16 = (uint8_t) (msg.id >> 16) & 0x03;                // EID17..16
        x.id.eid15_8  = (uint8_t) (msg.id >> 8);                        // EID15..8
        x.id.eid7_0   = (uint8_t) msg.id;                               // EID7..0
    } else {
        x.id.sid10_3  = (uint8_t) (msg.id >> 3);                        // SID10..3
        x.id.sid2_0   = (uint8_t) (msg.id & 0x07);                      // SID2..0
    }
    x.dlc = msg.len & 0x0f;                                             // Number of bytes in can message
    x.ertr = msg.type;                                                  // Data or remote message
    memcpy(x.data,msg.data,x.dlc);                                      // Get the Data bytes
// write CANmsg to the specified TX buffer 'num'
    mcpWriteBuffer(obj, bufferCommand[num], y, sizeof(x));              // Write the message ,CANMsg, to the MCP2515's Tx buffer 'num' (as an array)
    mcpBufferRTS(obj, rtsCommand[num]);
    return 1;                                                           // Indicate that message has been transmitted
}
 
/** read a CAN message from the MCP2515
 */
uint8_t mcpCanRead(mcp_can_t *obj, CAN_Message *msg)
{
    union {                                                             // Access CANMsg as:
        CANMsg x;                                                       // the organised struct
        uint8_t y[];                                                    // or contiguous memory array
    };
    uint8_t bufferCommand[] = {MCP_READ_RX0, MCP_READ_RX1};
    uint8_t status = mcpReceiveStatus(obj);
    bool num = 0;
// Check if there is a message the buffers
    if (status & MCP_RXSTAT_RXB0) {                                     // Msg in Buffer 0?
        num = 0;
    } else if (status & MCP_RXSTAT_RXB1) {                              // Msg in Buffer 1?
        num = 1;
    } else {
        return 0;                                                       // No messages waiting
    }
    mcpReadBuffer(obj, bufferCommand[0], y, sizeof(x));                 // Read the message into CANMsg (as an array)
    mcpBitModify(obj, MCP_CANINTF, (!num ? MCP_RX0IF : MCP_RX1IF), 0);  // Free the message buffer
#ifdef DEBUG
    printf("sizeof CanMsgStruct: %d bytes\r\n", sizeof(x));
    printf("sizeof CanMsgArray: %d bytes\r\n", sizeof(y));
    printf("sid10_3: %x\r\n", x.id.sid10_3);
    printf("eid17_16: %x\r\n", x.id.eid17_16);
    printf("ide: %x\r\n", x.id.ide);
    printf("srtr: %x\r\n", x.id.srtr);
    printf("sid2_0: %x\r\n", x.id.sid2_0);
    printf("eid15_8: %x\r\n", x.id.eid15_8);
    printf("eid7_0: %x\r\n", x.id.eid7_0);
    printf("dlc: %x\r\n", x.dlc);
    printf("ertr: %x\r\n", x.ertr);
    printf("data: ");
    for (char i=0; i<8; i++)
        printf("%02x,", x.data[i]);
    printf("\r\n");
#endif
    msg->format = (status & MCP_RXSTAT_IDE) ? CANExtended : CANStandard;// Extended CAN id Flag
    if (msg->format == CANExtended) {                                   // Assemble the Extended CAN id
        msg->id = (x.id.sid10_3 << 21)  |
                  (x.id.sid2_0 << 18)   |
                  (x.id.eid17_16 << 16) |
                  (x.id.eid15_8 << 8)   |
                  (x.id.eid7_0);
    } else {                                                            // Assemble the Standard CAN id
        msg->id = (x.id.sid10_3 << 3)   |
                  (x.id.sid2_0);
    }
    msg->len    = x.dlc;                                                // Number of bytes in CAN message
    msg->type = (status & MCP_RXSTAT_RTR) ? CANRemote : CANData;        // Determine if a Remote or Data message type
    memcpy(msg->data,x.data,x.dlc);                                     // Get the Data bytes
    return 1;                                                           // Indicate that message has been retrieved
}
 
/** initialise an Acceptance Mask
 */
uint8_t mcpInitMask(mcp_can_t *obj, uint8_t num, uint32_t ulData, bool ext)
{
    uint8_t mask[2] = { MCP_RXM0SIDH, MCP_RXM1SIDH };
 
    if (num > 1) {
#ifdef DEBUG
        printf("Trying to set an invalid Mask number: %d\r\n", num);
#endif
        return 0;
    }
#ifdef DEBUG
    printf("Begin to set Mask!!\r\n");
#endif
    uint8_t initialMode = mcpRead(obj, MCP_CANCTRL) & MODE_MASK;        // Store the current operation mode
    if(!mcpSetMode(obj, MODE_CONFIG)) {
        return 0;
    }
    mcpWriteId(obj, mask[num], ext, ulData);
    if(!mcpSetMode(obj, initialMode)) {
        return 0;
    }
#ifdef DEBUG
    printf("Successfully set Mask number: %d\r\n", num);
#endif
    return 1;
}
 
/** initialise an Acceptance Filter
 */
uint8_t mcpInitFilter(mcp_can_t *obj, uint8_t num, uint32_t ulData, bool ext)
{
    uint8_t filter[6] = { MCP_RXF0SIDH, MCP_RXF1SIDH, MCP_RXF2SIDH, MCP_RXF3SIDH, MCP_RXF4SIDH, MCP_RXF5SIDH };
 
    if (num > 5) {
#ifdef DEBUG
        printf("Trying to set an invalid Filter number: %d\r\n", num);
#endif
        return 0;
    }
#ifdef DEBUG
    printf("Begin to set Filter!!\r\n");
#endif
    uint8_t initialMode = mcpRead(obj, MCP_CANCTRL) & MODE_MASK;        // Store the current operation mode
    if(!mcpSetMode(obj, MODE_CONFIG)) {
        return 0;
    }
    mcpWriteId(obj, filter[num], ext, ulData);
    if(!mcpSetMode(obj, initialMode)) {
        return 0;
    }
#ifdef DEBUG
    printf("Successfully set Filter: %d\r\n", num);
#endif
    return 1;
}
 
/*  Report on the specified errors and warnings
 */
uint8_t mcpErrorType(mcp_can_t *obj, const CANFlags type)
{
    uint8_t which[] = { MCP_EFLG_ALLMASK,
                        MCP_EFLG_ERRORMASK,
                        MCP_EFLG_WARNMASK,
                        MCP_EFLG_RX1OVR,
                        MCP_EFLG_RX0OVR,
                        MCP_EFLG_TXBO,
                        MCP_EFLG_TXEP,
                        MCP_EFLG_RXEP,
                        MCP_EFLG_TXWAR,
                        MCP_EFLG_RXWAR,
                        MCP_EFLG_EWARN
                      };
 
    return (mcpRead(obj, MCP_EFLG) & which[type]) ? 1 : 0;
}
 
/*  Return contents of the error and warning flags register
 */
uint8_t mcpErrorFlags(mcp_can_t *obj)
{
    return (mcpRead(obj, MCP_EFLG));
}
 
/*  Number of message reception errors
 */
uint8_t mcpReceptionErrorCount(mcp_can_t *obj)
{
    return (mcpRead(obj, MCP_REC));
}
 
/*  Number of message transmission errors
 */
uint8_t mcpTransmissionErrorCount(mcp_can_t *obj)
{
    return (mcpRead(obj, MCP_TEC));
}
 
/* Select between monitor (silent = 1) and normal (silent = 0) modes
 */
void mcpMonitor(mcp_can_t *obj, const bool silent)
{
    silent ? mcpSetMode(obj, MODE_LISTENONLY) : mcpSetMode(obj, MODE_NORMAL);
}
 
/* Change CAN operation to the specified mode
 */
uint8_t mcpMode(mcp_can_t *obj, const CANMode mode)
{
    uint8_t which[] = { MODE_NORMAL,
                        MODE_SLEEP,
                        MODE_LOOPBACK,
                        MODE_LISTENONLY,
                        MODE_CONFIG,
                        MODE_CONFIG
                      };
 
    if (mode == _M_RESET) {
        mcpReset(obj);
    }
    if (mcpSetMode(obj, which[mode])) {
        return 1;
    }
    return 0;
}
 
/*  Configure interrupt sources
 */
void mcpSetInterrupts(mcp_can_t *obj, const CANIrqs irqSet)
{
    uint8_t which[] = { MCP_NO_INTS,
                        MCP_ALL_INTS,
                        MCP_RX_INTS,
                        MCP_TX_INTS,
                        MCP_RX0IF,
                        MCP_RX1IF,
                        MCP_TX0IF,
                        MCP_TX1IF,
                        MCP_TX2IF,
                        MCP_ERRIF,
                        MCP_WAKIF,
                        MCP_MERRF
                      };
 
    mcpWrite(obj, MCP_CANINTE, which[irqSet]);
}
 
/*  Report on the specified interrupt causes
 */
uint8_t mcpInterruptType(mcp_can_t *obj, const CANIrqs irqFlag)
{
    uint8_t which[] = { MCP_NO_INTS,
                        MCP_ALL_INTS,
                        MCP_RX_INTS,
                        MCP_TX_INTS,
                        MCP_RX0IF,
                        MCP_RX1IF,
                        MCP_TX0IF,
                        MCP_TX1IF,
                        MCP_TX2IF,
                        MCP_ERRIF,
                        MCP_WAKIF,
                        MCP_MERRF
                      };
 
    return (mcpRead(obj, MCP_EFLG) & which[irqFlag]) ? 1 : 0;
}
 
/*  Return contents of the interrupt flags register
 */
uint8_t mcpInterruptFlags(mcp_can_t *obj)
{
    return (mcpRead(obj, MCP_EFLG));
}
