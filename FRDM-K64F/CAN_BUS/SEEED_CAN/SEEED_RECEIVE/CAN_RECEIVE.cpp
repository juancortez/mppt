/*************************** CAN_RECEIVE.cpp ***************************************************
* Maximum Power Point Tracker Project for EE 464R
* 
* CAN_RECEIVE Program - This program is loaded onto one of the FRDM boards with the CAN-BUS Shield
* attached. This program receives data via CAN-BUS. The results of the program running is located
* in the SEEED_RECEIVE.png file.
* 
* Author: Juan Cortez
* Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
* Created on: October 20, 2015 at 16:05
* Revised on: October 20, 2015 at 16:05
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

DigitalOut led2(LED2);

int main() {
  SEEED_CANMessage msg; // create empty CAN message

    printf("SEEED_RECEIVE Program Starting...\r\n");
    
    int can_open_status = can.open(500000, SEEED_CAN::Normal);  // initialize CAN-BUS Shield
    if(can_open_status){
        printf("CAN Bus Shield successfully initialized!\r\n");    
    } else{
        printf("CAN BUS Shield initialization failed...\r\n");    
    }
    
  while(1) {
    if(can.read(msg)) {  // if message is available, read into msg
      printf("Message received: %d %d\r\n", msg.data[0], msg.data[1]); // display message data
      led2 = !led2; // toggle receive status LED
    } 
    wait(1);
  }
}