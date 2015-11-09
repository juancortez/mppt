/**
* This program flashes the red/green LEDs and resets the Seeed Studio CAN-BUS shield
* and reads some of the registers. The output of this program can be found in 
* /mppt/FRDM-K64F/CAN_BUS/MCP2515/MCP2515_Result.png
* 
* Author: Juan Cortez
* Date Created: October 19, 2015 at 12:15pm
* Date Modified: October 19, 2015 at 13:01pm
*
* SPI Signals Can be found in: mppt/FRDM-K64F/Documentation/MCP2515_Data_Sheet.pdf
* Program Acquired from: https://developer.mbed.org/users/Just4pLeisure/notebook/frdm-kl25z-library-for-seeed-studio-can-bus-shield/
* 
* Terminal Emulator on Mac: $ cd /dev && screen `ls | grep tty.usbmodem`
*/

#include "mbed.h"
 
SPI spi(PTD2, PTD3, PTD1); // mosi, miso, sclk
DigitalOut cs(PTD0);
DigitalOut myled(LED2);
DigitalOut resetled(LED3);
 
 
int main()
{
    myled = 1;
    resetled = 1;
    // Chip must be deselected
    cs = 1;
 
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spi.format(8,3);
    spi.frequency(1000000);
 
    while (1) {
 
        // Select the device by seting chip select low
        resetled = 0;
        cs = 0;

        /* Juan Cortez (10/19/2015) - MCP2515_Data_Sheet pg. 66
        *  Instruction Name: RESET
        *  Description: Resets internal registers to default state, set
        *               Configuration Mode.
        *  Send 0xC0, the command to reset the MCP2515 chip
        */
        spi.write(0xC0);

        printf("Resetting the MCP2515 chip.\r\n");
        // Deselect the device
        cs = 1;
        wait_ms(10);
        resetled = 1;
 
        // Select the device by seting chip select low
        cs = 0;

        /* Juan Cortez (10/19/2015) - MCP2515_Data_Sheet pg. 66
        *  Instruction Name: READ STATUS
        *  Description: Quick polling command that reads several status bits for
        *               transmit and receive functions.
        *  Send 0xA0, the command to read the MCP2515 Status Register
        */
        spi.write(0xA0);

        // Send a dummy byte to receive the contents of the status register
        int status = spi.write(0x00);
        printf("status register = 0x%02X.\r\n", status);
        // Deselect the device
        cs = 1;
 
        // Select the device by seting chip select low
        cs = 0;

        /* Juan Cortez (10/19/2015) - MCP2515_Data_Sheet pg. 66
        *  Instruction Name: WRITE
        *  Description: Write data to register beginning at selected address
        *  Send 0x03, the command to read an MCP2515 register
        */
        spi.write(0x03);

        /* Juan Cortez (10/19/2015) - MCP2515_Data_Sheet pg. 63
        *  CANTRL Register is 0xF
        *  Send 0x0F, the address of the MCP2515 CANTRL register
        */
        spi.write(0x0F);

        // Send a dummy byte to receive the contents of the status register
        int CANCTRL = spi.write(0x00);
        printf("CANCTRL register = 0x%02X.\r\n", CANCTRL);
        // Deselect the device
        cs = 1;
 
        wait(1);
        myled = !myled;
    }
}
 