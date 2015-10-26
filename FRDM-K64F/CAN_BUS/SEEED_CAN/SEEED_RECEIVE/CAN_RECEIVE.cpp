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
* Revised on: October 23, 2015 at 16:1
* 
* Serial Communication on MAC
* 
* $ cd /dev && screen `ls | grep tty.usbmodem`
*
* Example found in: // http://langster1980.blogspot.com/2014/10/mbed-can-bus-tutorial.html
****************************************************************************************/


#include "mbed.h"
#include "seeed_can.h"

// function that gets called on a receiving interrupt witht he specified ID
void CAN_Interrupt_Received(void);

// prints out the status of the CAN Bus initialization
void printStatus(int);

/*
* This function converts an 8 char array variable into a float variable
* and returns it.
*
* Expected Result: [0, 2, 3, ., 2, 3, 4] -> 23.234
*/
float convertToVariable(char*);

SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);
SEEED_CANMessage msg; // create empty CAN message
Serial pc(USBTX, USBRX);                                  

DigitalOut led1(LED1);
DigitalOut led2(LED2);


//TODO: which data are we going to transmit in the transmitter side?
int main() {
    printf("SEEED_RECEIVE Program Starting...\r\n");
    int filterID = 0x07;
    int can_open_status = can.open(500000, SEEED_CAN::Normal);  // initialize CAN-BUS Shield
    printStatus(can_open_status);
    
    //TODO: figure out which unique ID we want to use on the receiving side
    can.mask(0, 0x1FFFFFFF); // Configure Mask 0 to check all bits of a Standard CAN message Id
    can.mask(1, 0x1FFFFFFF, CANStandard); // Configure Mask 1 to check all bits of a Standard CAN message Id
    can.filter(0, filterID);  // ONLY ACCEPTS ID listed in filterID variable
    printf("CAN-BUS filtering messages with ID: %d\r\n", filterID);
    
    //TODO: figure out which IRQType to use. Available IRQTypes are found in seeed_can.h on line 251!
    can.attach(CAN_Interrupt_Received, SEEED_CAN::RxAny); // when an interrupt is triggered, it will call CAN_Interrupt_Received
    
  while(1) {
    led1 = !led1; // RED heartbeat to make sure that the program is running
    wait(1);
  }
}

/* Prints the status of CAN_BUS initialization */
void printStatus(int status){
     if(status == 1){
        printf("CAN Bus Shield successfully initialized!\r\n"); 
    } else{
        printf("CAN BUS Shield initialization failed...\r\n");    
    }
}

/*
* This function is called when the receiver receives a message from a transmitting CAN-BUS. Since the filter is set, it will
* only accept messages with the ID as specified in the can.filter parameter. In this program, it is 7. 
*/
void CAN_Interrupt_Received(void){
    int counter = 0;
    if(can.read(msg)) {  // if message is available, read into msg
      printf("{ID: %d | Data: ", msg.id);
      for(counter = 0; counter < msg.len; counter++){
        //TODO: what are we going to do with this data?
        printf("%c", msg.data[counter]);
      }
      printf("}\r\n");
      led2 = !led2; // Yellow toggle receive status LED
    } else{
        printf("No message data...\r\n");
        return;
    } 
}

/*
* This function converts an 8 char array variable into a float variable
* and returns it.
*
* Expected Result: [0, 2, 3, ., 2, 3, 4] -> 23.234
*/
float convertToVariable(char *ptr){
  float hundreds = ((float)*ptr++ - 48) * 100;
  hundreds = hundreds < 0 ? 0 : hundreds;
  float tens = ((float)*ptr++ - 48) * 10;
  tens = tens < 0 ? 0 : tens;
  float ones = ((float)*ptr++ - 48) * 1;
  ones = ones < 0 ? 0 : ones;
  ptr++; // skip over decimal value
  float tenths = ((float)*ptr++ - 48) / 10;
  tenths = tenths < 0 ? 0 : tenths;
  float hundredths = ((float)*ptr++ - 48) / 100;
  hundredths = hundredths < 0 ? 0 : hundredths;
  float thousandths = ((float)*ptr++ - 48) / 1000;
  thousandths = thousandths < 0 ? 0 : thousandths;
  return hundreds+tens+ones+tenths+hundredths+thousandths;
}