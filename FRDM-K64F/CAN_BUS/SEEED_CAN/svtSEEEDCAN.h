/***************************************************************
   -
   - file:  svtSEEEDCAN.h
   -
   - purpose: handles connection to solar car main CAN bus
   -
   - author: J. C. Wiley, Sept. 2014
   -         uses SEEED_CAN by Sophie Dexter, mbed.org (with modifications)
   -         for thread within class see: https://mbed.org/forum/mbed/topic/4388/
   -
***************************************************************/
#ifndef _SVTSEEEDCAN_H_
#define _SVTSEEEDCAN_H_

#include "mbed.h"
#include "rtos.h"
#include "svtCAN.h"
#define MBED_CAN_HELPER_H 1
#include "seeed_can.h"

// F64K spi uses same pins as Arduino
#define Arduino_ncs  PTD0
#define Arduino_mosi PTD2
#define Arduino_miso PTD3
#define Arduino_sck PTD1
#define Arduino_int PTB9

// Set low for testing, SPI can exceed logic analyzer no error speed
#define SPISPEED 500000

extern RawSerial udebug;  // crutch for testing

/** svtSEEEDCAN class
 *   Uses modified Seeed_CAN library 
 */
class svtSEEEDCAN: public svtCAN { 
   public:
    /** svtSEEEDCAN - constructor
     */
 svtSEEEDCAN():
        svtCAN(),
        _can(Arduino_ncs, Arduino_int, Arduino_mosi, Arduino_miso, Arduino_sck, SPISPEED),
	_interrupt(Arduino_int) {
	    _interrupt.fall(this, &svtSEEEDCAN::isr);  // new CAN message on falling edge
            _canmsg.len = 0;
	};
 
    /** svtSEEEDCAN - destructor
     */
    ~svtSEEEDCAN();

    /** isr - handles all interrupts from MPC2515
     */
    void isr();

    /** init - initialize can bus
     */
    int init(int canspeed, CANFormat format = CANStandard, uint32_t addr=0x0234);



/** Filter - sets global mask and up to six input id filters, which when combined with
 *   mask, determine which messages are read from bus.
 *  @param mask  32 bit mask, where a 1 indicates bit will be checked between
 *     incoming-id and filter-id, 0 is pass all. The underlying hardware may support
 *     more complex masking, but here only single global mask used
 *   @param idfilters - pointer to an array of filters
 *   @param n - number of filters supplied
 *   @return 0 - success, 1 failure
 */
    int Filter(int32_t mask, int32_t* idfilters, int32_t n);

    /** Set the frequency of the CAN interface
     *
     *  @param hz The bus frequency in hertz
     *
     *  @returns
     *    1 if successful,
     *    0 otherwise
     */
    int frequency(int hz);

    /** handleInMsg - retrieve msg from MPC2515 and put in queue
     */
    int handleInMsg();

    /** handleOutMsg - retrieve msg from OUTqueue and send CAN message.
     */
    int handleOutMsg();
      
private:          
    SEEED_CAN     _can;
    InterruptIn   _interrupt; 
    SEEED_CANMessage   _canmsg; // single out can message  
};
#endif

