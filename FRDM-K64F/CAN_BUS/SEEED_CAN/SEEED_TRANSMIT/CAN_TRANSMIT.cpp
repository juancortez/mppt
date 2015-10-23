/*************************** CAN_RECEIVE.cpp ***************************************************
* Maximum Power Point Tracker Project for EE 464R
* 
* CAN_TRANSMIT Program - This program is loaded onto one of the FRDM boards with the CAN-BUS Shield
* attached. This program transmits data via CAN-BUS. In order to see outputs of this program, another
* FRDM with a CAN-BUS Shield must be connected. 

* The results of the program running is locate in the SEEED_TRANSMIT.png file.
* 
* Author: Juan Cortez
* Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
* Created on: October 20, 2015 at 16:10
* Revised on: October 23, 2015 at 16:32
* 
* Serial Communication on MAC
* 
* $ cd /dev && screen `ls | grep tty.usbmodem`
*
****************************************************************************************/

#include "mbed.h"
#include "seeed_can.h"
#include "stdlib.h"
#define MESSAGE_LENGTH 8

// prints contents that are in the 8 character array
void printData(char*); 

// converts a char variable into an 8 character array
void convertToCharArray(char*, float);

// prints out all the character elements inside an 8 character array
void printStatus(int);

// initialize CAN_BUS pin values with 500k baud rate
SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);

// for debugging purposes
Serial pc(USBTX, USBRX);                                  

// heartbeats
DigitalOut led1(LED1);
DigitalOut led2(LED2);

int main()
{
    int id = 7; // ID of the CAN_BUS Message TODO: determine which ID to send
    char can_data[MESSAGE_LENGTH] = {}; // data being transmitted over CAN_BUS

    /* 
    * This is all example data that I will be sending to the CAN-BUS receiver, in the following order: 
    * outVoltage, inCurrent, inVoltage, and outCurrent
    */
    char *readingString[] = {
        "OutVoltage", "InCurrent",
        "InVoltage", "OutCurrent"
    };
    float dataRead[] = {
        120.1234, 4.3234, 23.1232, 3.1232    
    };
    int readingNumber = 0; // counter for which data is being transmitted
    
    printf("SEEED_TRANSMIT Program Starting...\r\n"); 
    
    int can_open_status = can.open(500000, SEEED_CAN::Normal); // initialize CAN-BUS Shield
    printStatus(can_open_status); // prints status of initialization
        
    while (1) {       
        convertToCharArray(*&can_data, dataRead[readingNumber]); // convert float to an 8 char number
        
        // transmit 'can_data' to the receiving CAN-BUS      
        if (can.write(SEEED_CANMessage(id, can_data, MESSAGE_LENGTH, CANData, CANStandard))) { 
            /*
            * CAN-BUS TRANSMIT will send reading values in this order: 
            * "OutVoltage:", "InCurrent:", "InVoltage", "OutCurrent:"
            */
            printf("Data being transmitted is: %s\r\n", readingString[readingNumber]);
            readingNumber = (readingNumber + 1) % 4; // modulus 4 since there are 4 data elements in readingString array
            printData(*&can_data);
            led1 = !led1; // heartbeat
        }else{
            int reset_status = can.mode(SEEED_CAN::Reset); // reset canbus if there is a problem, returns 1 if successful, 0 otherwise
         }
         convertToCharArray(*&can_data, dataRead[readingNumber]); // convert the next value being sent to a char array
         led2 = !led2;
         wait(1);                                                  
    }
}

/* Prints the status of CAN_BUS initialization */
void printStatus(int status){
     if(status == 1){
        printf("CAN Bus Shield successfully initialized!\r\n"); 
        printf("Sending a message via CANbus...\r\n");   
    } else{
        printf("CAN BUS Shield initialization failed...\r\n");    
    }
}

/*
* This function converts float numbers to a char array. Limit of 7 characters.
*
* Inputs: char* ptr - points to the beginning of buffer[8], the char array where
*                     we will place the result
*         float val - the value we want converted to a char array
*
* Output: 3 whole number values, dot, 3 decimal points
*
* Expected Result: 23.2345 ---->  [0, 2, 3, ., 2, 3, 4]
* Expected Result: 120.1234 ----> [1, 2, 0, ., 1, 2, 3]
*/
void convertToCharArray(char* ptr, float val){
    memset(ptr, 0, MESSAGE_LENGTH); // empty array  
    ptr = ptr + 7; // point the pointer to the end of the array
    int expandedVal = val * 10000; // 23.2345 -> 232345, 120.123 -> 120123
    int counter = 0;
    int digit; 
    while (expandedVal > 0) {
        digit = expandedVal % 10;
        *ptr = digit + '0'; // convert to a char
        counter++; ptr--;
        if(counter == 4){
            *ptr = '.';
            ptr--;
        }
        expandedVal /= 10;
    }
}

/*
* This function prints out all the character elements inside an 8 character array
*/
void printData(char* ptr){
    int counter = 0;
    while(counter < MESSAGE_LENGTH){
        printf("%c", *ptr++);
        counter++;
    }
    printf("\r\n");
}
