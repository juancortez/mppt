/* mbed FRDM-KL25Z Library for Seeed Studios CAN-BUS Shield
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

#include "seeed_can.h"

/** Seeed Studios CAN-BUS Shield Constructor - Create a SEEED_CAN interface connected to the specified pins.
 */
SEEED_CAN::SEEED_CAN(PinName ncs, PinName irq, PinName mosi, PinName miso, PinName clk, int spiBitrate) :
    _spi(mosi, miso, clk),
    _can(_spi, ncs, irq),
    _irqpin(irq)
{
    // Make sure CS is high
    _can.ncs = 1;
    // Set up the spi interface
    _can.spi.format(8, 3);
    _can.spi.frequency(spiBitrate);
    _irqpin.fall(this, &SEEED_CAN::call_irq);
}

/** Open initialises the Seeed Studios CAN-BUS Shield.
 */
int SEEED_CAN::open(int canBitrate, Mode mode)
{
    return mcpInit(&_can, (uint32_t) canBitrate, (CANMode)mode);
}

/** Puts or removes the Seeed Studios CAN-BUS shield into or from silent monitoring mode
 */
void SEEED_CAN::monitor(bool silent)
{
    mcpMonitor(&_can, silent);
}

/** Change the Seeed Studios CAN-BUS shield CAN operation mode
 */
int SEEED_CAN::mode(Mode mode)
{
    return mcpMode(&_can, (CANMode)mode);
}

/** Set the CAN bus frequency (Bit Rate)
*/
int SEEED_CAN::frequency(int canBitRate){
    return mcpSetBitRate(&_can, (uint32_t) canBitRate);
  //  return mcpInit(&_can, (uint32_t) canBitRate, (CANMode)Normal);
}

/** Read a CAN bus message from the MCP2515 (if one has been received)
 */
int SEEED_CAN::read(SEEED_CANMessage &msg)
{
    return mcpCanRead(&_can, &msg);
}

/**  Write a CAN bus message to the MCP2515 (if there is a free message buffer)
 */
int SEEED_CAN::write(SEEED_CANMessage msg)
{
    return mcpCanWrite(&_can, msg);
}

/** Configure one of the Accpetance Masks (0 or 1)
 */
int SEEED_CAN::mask(int maskNum, int canId, CANFormat format)
{
    return mcpInitMask(&_can, maskNum, canId, format);
}

/** Configure one of the Acceptance Filters (0 through 5)
 */
int SEEED_CAN::filter(int filterNum, int canId, CANFormat format)
{
    return mcpInitFilter(&_can, filterNum, canId, format);
}

/** Returns number of message reception (read) errors to detect read overflow errors.
 */
unsigned char SEEED_CAN::rderror(void)
{
    return mcpReceptionErrorCount(&_can);
}

/** Returns number of message transmission (write) errors to detect write overflow errors.
 */
unsigned char SEEED_CAN::tderror(void)
{
    return mcpTransmissionErrorCount(&_can);
}

/** Check if any type of error has been detected on the CAN bus
 */
int SEEED_CAN::errors(ErrorType type)
{
    return mcpErrorType(&_can, (CANFlags)type);
}

/** Returns the contents of the MCP2515's Error Flag register
 */
 unsigned char SEEED_CAN::errorFlags(void)
 {
    return mcpErrorFlags(&_can);
 }

/** Attach a function to call whenever a CAN frame received interrupt is generated.
 */
void SEEED_CAN::attach(void (*fptr)(void), IrqType event)
{
    if (fptr) {
        _callback_irq.attach(fptr);
        mcpSetInterrupts(&_can, (CANIrqs)event);
//        _irq[(CanIrqType)type].attach(fptr);
//        can_irq_set(&_can, (CanIrqType)type, 1);
    } else {
        mcpSetInterrupts(&_can, (CANIrqs)SEEED_CAN::None);
//        can_irq_set(&_can, (CanIrqType)type, 0);
    }
}


void SEEED_CAN::call_irq(void)
{
    _callback_irq.call();
}

/** Check if the specified interrupt event has occurred
 */
int SEEED_CAN::interrupts(IrqType type)
{
    return mcpInterruptType(&_can, (CANIrqs)type);
}

/** Returns the contents of the MCP2515's Interrupt Flag register
 */
 unsigned char SEEED_CAN::interruptFlags(void)
 {
    return mcpInterruptFlags(&_can);
 }

/** Sets MCP2515's Interrupt Enable register  //jcw
 */
void SEEED_CAN::setinterrupts(IrqType type)
{
       mcpSetInterrupts(&_can, (CANIrqs)type);
}


