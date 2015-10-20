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
* Revised on: October 20, 2015 at 18:01
* 
* Serial Communication on MAC
* 
* $ cd /dev && screen `ls | grep tty.usbmodem`
*
* Example found in: // http://langster1980.blogspot.com/2014/10/mbed-can-bus-tutorial.html
****************************************************************************************/

#include "mbed.h"
#include "seeed_can.h"
#define MESSAGE_LENGTH 8

void printData(char*); // this functions prints the contents that are inside the array

SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);
Serial pc(USBTX, USBRX);                                  

DigitalOut led1(LED1);
DigitalOut led2(LED2);

//SEEED_CANMessage(int _id, const char *_data, char _len = 8, CANType _type = CANData, CANFormat _format = CANStandard)
int id = 1;
char can_data[MESSAGE_LENGTH] = "abcdefg"; // data being sent via CAN-BUS


int main()
{
    char *ptr = can_data; // *ptr is pointing to the beginning of the candata array
    SEEED_CANMessage canMsg(id, can_data, MESSAGE_LENGTH, CANData, CANStandard);  // initialize constructor
    
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
            printf("Data being transmitted is: \r\n");
            printData(ptr);
            ptr = can_data; // reset pointer
            led1 = !led1; // heartbeat
        }else{
            int reset_status = can.mode(SEEED_CAN::Reset); // reset canbus if there is a problem, returns 1 if successful, 0 otherwise
         }
         canMsg.id = id++; // counter for terminal viewing purposes
         led2 = !led2;
         wait(10);                                                  
    }
}

void printData(char* ptr){
    int counter = 0;
    while(counter < MESSAGE_LENGTH){
        printf("%c ", *ptr++);
        counter++;
    }
    printf("\r\n");
}