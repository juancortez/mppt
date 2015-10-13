/* seeed_can_spi.h
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
#ifndef _SEEED_CAN_SPI_H_
#define _SEEED_CAN_SPI_H_

#include "seeed_can_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

    /** CAN driver typedefs
     */
 /// Type definition to hold a Seeed Studios CAN-BUS Shield connections and resources structure
   struct Seeed_MCP_CAN_Shield {
        SPI             spi;
        DigitalOut      ncs;
        InterruptIn     irq;
        Seeed_MCP_CAN_Shield(SPI _spi_, DigitalOut _ncs_, InterruptIn _irq_) :
            spi(_spi_),
            ncs(_ncs_),
            irq(_irq_)
        {}
    };
    typedef struct Seeed_MCP_CAN_Shield mcp_can_t;

    /** mcp2515 spi instructions
     */
    void mcpReset(mcp_can_t *obj);                                      // reset the MCP2515 CAN controller chip

    uint8_t mcpRead(mcp_can_t *obj,                                     // read from a single MCP2512 register
                    const uint8_t address);
    void mcpReadMultiple(mcp_can_t *obj,                                // read multiple, sequential, registers into an array
                         const uint8_t address,
                         uint8_t values[],
                         const uint8_t n);
    void mcpReadBuffer(mcp_can_t *obj,                                  // read the specified receive buffer into an array
                       const uint8_t command,
                       uint8_t values[],
                       const uint8_t n);
    void mcpWrite(mcp_can_t *obj,                                       // write to a single MCP2512 register
                  const uint8_t address,
                  const uint8_t value);
    void mcpWriteMultiple(mcp_can_t *obj,                               // write an array into consecutive MCP2515 registers
                          const uint8_t address,
                          const uint8_t values[],
                          const uint8_t n);
    void mcpWriteBuffer(mcp_can_t *obj,                                 // write an array into the specified transmit buffer
                        const uint8_t command,
                        uint8_t values[],
                        const uint8_t n);
    void mcpBufferRTS(mcp_can_t *obj, const uint8_t command);           // initiate transmission of the specified MCP2515 transmit buffer
    uint8_t mcpStatus(mcp_can_t *obj);                                  // read the MCP2515's status register
    uint8_t mcpReceiveStatus(mcp_can_t *obj);                           // read mcp2515's receive status register
    void mcpBitModify(mcp_can_t *obj,                                   // modify bits of a register specified by a mask
                      const uint8_t address,
                      const uint8_t mask,
                      const uint8_t data);

#ifdef __cplusplus
};
#endif

#endif    // SEEED_CAN_SPI_H