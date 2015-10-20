/*************************** CAN_RECEIVE.cpp ***************************************************
* Maximum Power Point Tracker Project for EE 464R
* 
* CAN_TRANSMIT Program - This program is loaded onto one of the FRDM boards with the CAN-BUS Shield
* attached. This program transmits data via CAN-BUS. In order to see outputs of this program, another
* FRDM with a CAN-BUS Shield must be connected. The results of the program running is located
* in the SEEED_TRANSMIT.png file.
* 
* Author: Juan Cortez
* Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
* Created on: October 20, 2015 at 16:10
* Revised on: October 20, 2015 at 16:10
* 
* Serial Communication on MAC
* 
* $ cd /dev && screen `ls | grep tty.usbmodem`
*
* Example found in: // http://langster1980.blogspot.com/2014/10/mbed-can-bus-tutorial.html
****************************************************************************************/

#include "mbed.h"
#include "seeed_can.h"

SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);
Serial pc(USBTX, USBRX);                                  

DigitalOut led1(LED1);
DigitalOut led2(LED2);

//SEEED_CANMessage(int _id, const char *_data, char _len = 8, CANType _type = CANData, CANFormat _format = CANStandard)
int pc_ID = 1;
char candata[8]; 
int pc_length = 8;


int main()
{
    candata[0] = 'a'; // insert dummy data for the const char *_data portion of the CANMessage constructor 
    candata[1] = 'b'; 
    SEEED_CANMessage canMsg(pc_ID,candata,(char)pc_length,CANData,CANStandard);  // initialize constructor
    
    printf("SEEED_TRANSMIT Program Starting...\r\n"); // initialize CAN-BUS Shield
    
    int can_open_status = can.open(500000, SEEED_CAN::Normal); 
    if(can_open_status){
        printf("CAN Bus Shield successfully initialized!\r\n");    
    } else{
        printf("CAN BUS Shield initialization failed...\r\n");    
    }

    printf("sending a message via CANbus...\r\n");
    
    while (1) {       
        // send value to CAN bus and monitor return value to check if CAN
        // message was sent successfully. If so display, increment and toggle
        if (can.write(canMsg)) {  
            printf("CANBus Message sent: %d %d\r\n", candata[0], candata[1]); // display message being sent
            led1 = !led1; // heartbeat
        }else{
            int reset_status = can.mode(SEEED_CAN::Reset); // reset canbus if there is a problem, returns 1 if successful, 0 otherwise
         }
        wait(1);                                                  
    }
}