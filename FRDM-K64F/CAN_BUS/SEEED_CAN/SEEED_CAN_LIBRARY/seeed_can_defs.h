/* seeed_can_defs.h
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
#ifndef SEEED_CAN_DEFS_H
#define SEEED_CAN_DEFS_H
 
#include "mbed.h"
 
#ifdef __cplusplus
extern "C" {
#endif
 
    /** FRDM-KL25Z port pins used by Seeed Studios CAN-BUS Shield
     * 
     * Changed CAN_IRQ and CAN_IO9 pins to match FRDM-K64F pins
     * - Juan Cortez on October 11, 2015
     */
#define SEEED_CAN_CS        PTD0
#define SEEED_CAN_CLK       PTD1
#define SEEED_CAN_MOSI      PTD2
#define SEEED_CAN_MISO      PTD3
#define SEEED_CAN_IRQ       PTB9
#define SEEED_CAN_IO9       PTC4
    /** Define MCP2515 register addresses
     */
#define MCP_RXF0SIDH        0x00
#define MCP_RXF0SIDL        0x01
#define MCP_RXF0EID8        0x02
#define MCP_RXF0EID0        0x03
#define MCP_RXF1SIDH        0x04
#define MCP_RXF1SIDL        0x05
#define MCP_RXF1EID8        0x06
#define MCP_RXF1EID0        0x07
#define MCP_RXF2SIDH        0x08
#define MCP_RXF2SIDL        0x09
#define MCP_RXF2EID8        0x0A
#define MCP_RXF2EID0        0x0B
#define MCP_CANSTAT         0x0E
#define MCP_CANCTRL         0x0F
#define MCP_RXF3SIDH        0x10
#define MCP_RXF3SIDL        0x11
#define MCP_RXF3EID8        0x12
#define MCP_RXF3EID0        0x13
#define MCP_RXF4SIDH        0x14
#define MCP_RXF4SIDL        0x15
#define MCP_RXF4EID8        0x16
#define MCP_RXF4EID0        0x17
#define MCP_RXF5SIDH        0x18
#define MCP_RXF5SIDL        0x19
#define MCP_RXF5EID8        0x1A
#define MCP_RXF5EID0        0x1B
#define MCP_TEC             0x1C
#define MCP_REC             0x1D
#define MCP_RXM0SIDH        0x20
#define MCP_RXM0SIDL        0x21
#define MCP_RXM0EID8        0x22
#define MCP_RXM0EID0        0x23
#define MCP_RXM1SIDH        0x24
#define MCP_RXM1SIDL        0x25
#define MCP_RXM1EID8        0x26
#define MCP_RXM1EID0        0x27
#define MCP_CNF3            0x28
#define MCP_CNF2            0x29
#define MCP_CNF1            0x2A
#define MCP_CANINTE         0x2B
#define MCP_CANINTF         0x2C
#define MCP_EFLG            0x2D
#define MCP_TXB0CTRL        0x30
#define MCP_TXB1CTRL        0x40
#define MCP_TXB2CTRL        0x50
#define MCP_RXB0CTRL        0x60
#define MCP_RXB0SIDH        0x61
#define MCP_RXB1CTRL        0x70
#define MCP_RXB1SIDH        0x71
    /** Define MCP2515 SPI Instructions
     */
#define MCP_WRITE           0x02
 
#define MCP_READ            0x03
 
#define MCP_BITMOD          0x05
 
#define MCP_WRITE_TX0       0x40
#define MCP_WRITE_TX1       0x42
#define MCP_WRITE_TX2       0x44
 
#define MCP_RTS_TX0         0x81
#define MCP_RTS_TX1         0x82
#define MCP_RTS_TX2         0x84
#define MCP_RTS_ALL         0x87
 
#define MCP_READ_RX0        0x90
#define MCP_READ_RX1        0x94
 
#define MCP_READ_STATUS     0xA0
 
#define MCP_RX_STATUS       0xB0
 
#define MCP_RESET           0xC0
 
//#define TIMEOUTVALUE        50
//#define MCP_SIDH            0
//#define MCP_SIDL            1
//#define MCP_EID8            2
//#define MCP_EID0            3
 
#define MCP_TXB_EXIDE_M     0x08                                        /* In TXBnSIDL                  */
#define MCP_DLC_MASK        0x0F                                        /* 4 LSBits                     */
#define MCP_RTR_MASK        0x40                                        /* (1<<6) Bit 6                 */
 
#define MCP_RXB_RX_ANY      0x60
#define MCP_RXB_RX_EXT      0x40
#define MCP_RXB_RX_STD      0x20
#define MCP_RXB_RX_STDEXT   0x00
#define MCP_RXB_RX_MASK     0x60
#define MCP_RXB_BUKT_MASK   (1<<2)
    /** Bits in the TXBnCTRL registers.
     */
#define MCP_TXB_TXBUFE_M    0x80
#define MCP_TXB_ABTF_M      0x40
#define MCP_TXB_MLOA_M      0x20
#define MCP_TXB_TXERR_M     0x10
#define MCP_TXB_TXREQ_M     0x08
#define MCP_TXB_TXIE_M      0x04
#define MCP_TXB_TXP10_M     0x03
 
#define MCP_TXB_RTR_M       0x40                                        /* In TXBnDLC                   */
#define MCP_RXB_IDE_M       0x08                                        /* In RXBnSIDL                  */
#define MCP_RXB_RTR_M       0x40                                        /* In RXBnDLC                   */
 
    /** STATUS Command Values
     */
#define MCP_STAT_RXIF_MASK  (0x03)
#define MCP_STAT_RX0IF      (1<<0)
#define MCP_STAT_RX1IF      (1<<1)
#define MCP_STAT_TX0REQ     (1<<2)
#define MCP_STAT_TX0IF      (1<<3)
#define MCP_STAT_TX1REQ     (1<<4)
#define MCP_STAT_TX1IF      (1<<5)
#define MCP_STAT_TX2REQ     (1<<6)
#define MCP_STAT_TX2IF      (1<<7)
 
    /** RX STATUS Command Values
     */
#define MCP_RXSTAT_RXF_MASK (7<<0)
#define MCP_RXSTAT_RXF0     (0<<0)
#define MCP_RXSTAT_RXF1     (1<<0)
#define MCP_RXSTAT_RXF2     (2<<0)
#define MCP_RXSTAT_RXF3     (3<<0)
#define MCP_RXSTAT_RXF4     (4<<0)
#define MCP_RXSTAT_RXF5     (5<<0)
#define MCP_RXSTAT_RXROF0   (6<<0)                                      // RXF0 rollover to RXB1
#define MCP_RXSTAT_RXROF1   (7<<0)                                      // RXF1 rollover to RXB1
#define MCP_RXSTAT_RTR      (1<<3)
#define MCP_RXSTAT_IDE      (1<<4)
#define MCP_RXSTAT_RXB_MASK (3<<6)
#define MCP_RXSTAT_NONE     (0<<6)
#define MCP_RXSTAT_RXB0     (1<<6)
#define MCP_RXSTAT_RXB1     (2<<6)
#define MCP_RXSTAT_BOTH     (3<<6)
 
    /** EFLG Register Values
     */
#define MCP_EFLG_ALLMASK    (0xFF)                                      // All Bits
#define MCP_EFLG_ERRORMASK  (0xF8)                                      // 5 MS-Bits
#define MCP_EFLG_WARNMASK   (0x07)                                      // 3 LS-Bits
#define MCP_EFLG_EWARN      (1<<0)
#define MCP_EFLG_RXWAR      (1<<1)
#define MCP_EFLG_TXWAR      (1<<2)
#define MCP_EFLG_RXEP       (1<<3)
#define MCP_EFLG_TXEP       (1<<4)
#define MCP_EFLG_TXBO       (1<<5)
#define MCP_EFLG_RX0OVR     (1<<6)
#define MCP_EFLG_RX1OVR     (1<<7)
 
    /** CANCTRL Register Values
     */
#define CLKOUT_PS1          (0<<0)
#define CLKOUT_PS2          (1<<0)
#define CLKOUT_PS4          (2<<0)
#define CLKOUT_PS8          (3<<0)
#define CLKOUT_ENABLE       (1<<2)
#define CLKOUT_DISABLE      (0<<2)
#define MODE_ONESHOT        (1<<3)
#define ABORT_TX            (1<<4)
#define MODE_NORMAL         (0<<5)
#define MODE_SLEEP          (1<<5)
#define MODE_LOOPBACK       (2<<5)
#define MODE_LISTENONLY     (3<<5)
#define MODE_CONFIG         (4<<5)
#define MODE_POWERUP        (7<<5)
#define MODE_MASK           (7<<5)
 
    /** Bit Rate timing
     */
#define MCP_CLOCK_FREQ          16000000                                // 16 MHz Crystal frequency
#define CAN_SYNCSEG             1                                       // CAN-BUS Sync segment is always 1 Time Quantum
#define CAN_MAX_RATE            MCP_CLOCK_FREQ/(2 * MCP_MIN_TIME_QUANTA)
#define CAN_MIN_RATE            MCP_CLOCK_FREQ/(2 * MCP_MAX_PRESCALER * MCP_MAX_TIME_QUANTA)
#define MCP_MAX_TIME_QUANTA     25
#define MCP_MIN_TIME_QUANTA     8
#define MCP_MAX_PRESCALER       64
#define MCP_MIN_PRESCALER       1
    /** CNF1 Register Values
     */
#define SJW1                (0<<6)
#define SJW2                (1<<6)
#define SJW3                (2<<6)
#define SJW4                (3<<6)
    /** CNF2 Register Values
     */
#define BTLMODE             (1<<7)
#define SAMPLE_1X           (0<<4)
#define SAMPLE_3X           (1<<4)
    /** CNF3 Register Values
     */
#define WAKFIL_ENABLE       (1<<4)
#define WAKFIL_DISABLE      (0<<4)
#define SOF_ENABLE          (1<<7)
#define SOF_DISABLE         (0<<7)
    /** CANINTF Register Bits
     */
#define MCP_NO_INTS         (0x00)                                      // Disable all interrupts
#define MCP_ALL_INTS        (0xFF)                                      // All Bits
#define MCP_RX_INTS         (MCP_RX1IF | MCP_RX0IF)                     // Enable all receive interrupts
#define MCP_TX_INTS         (MCP_TX2IF | MCP_TX1IF | MCP_TX0IF)         // Enable all transmit interrupts
#define MCP_RX0IF           (1<<0)
#define MCP_RX1IF           (1<<1)
#define MCP_TX0IF           (1<<2)
#define MCP_TX1IF           (1<<3)
#define MCP_TX2IF           (1<<4)
#define MCP_ERRIF           (1<<5)
#define MCP_WAKIF           (1<<6)
#define MCP_MERRF           (1<<7)
 
//#define MCP_RXBUF_0         (MCP_RXB0SIDH)
//#define MCP_RXBUF_1         (MCP_RXB1SIDH)
 
#ifdef __cplusplus
};
#endif
 
#endif    // SEEED_CAN_DEFS_H
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
 
            