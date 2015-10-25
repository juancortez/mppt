 /********************************************************************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 14, 2015 at 14:26
 * Revised on: October 25, 2015 at 11:36
 *
 * GitHub Repository: https://github.com/juancortez-ut/mppt
 *
 * Dependent Hardware: 
 *          - Microcontroller: Freescale FRDM-K64F
 *          - CAN_BUS: CAN-BUS Shield v.1.2 by SEEED Studios
 *
 * Dependent Libraries:
 *          - mbed Library: https://developer.mbed.org/users/mbed_official/code/mbed/
 *          - SEEEED_CAN_LIBRARY: /mppt/FRDM-K64F/CAN_BUS/SEEED_CAN/SEEED_CAN_LIBRARY
 *
 * FRDM-K64F Pinout: https://developer.mbed.org/media/uploads/sam_grove/xk64f_page2.jpg.pagespeed.ic.XmUo-mk4LT.webp
 *
 * Signal     | FRDM-K64F Pin       | Purpose
 *
 * PwmOut     |     PTC11           | Output PWM Signal to Boost Converter
 * DigitalOut |     LED1            | Heartbeat LED
 * AnalogIn   |     PTB2            | Hall Sensor In
 * AnalogIn   |     PTB3            | Hall Sensor Out
 * AnalogIn   |     PTB10           | Voltage In
 * AnalogIn   |     PTB11           | Voltage Out
 * RawSerial  |     USBTX, USBRX    | Serial Communication
 * 
 * Program Description: This program is written for the Freescale FRDM-K64F microcontroller.
 * The description of the P&O algorithm is found below, in the comment section titled, 
 * 'Perturb & Observe Algorithm'. The binary of this file is located in the following directory
 * of the Github repository: '/mppt/FRDM-K64F/Perturb_and_Observe/Compiled Code'. Once the binary
 * of this program is loaded onto the board, the program will begin by initializing the
 * CAN_BUS Shield and waits for the user to press the 'SW3' button of the FRDM-K64 microcontroller.
 * When 'SW3' is pressed, the interrupt is handled and it begins the P&O algorithm. The FRDM-K64F
 * microcontroller reads in 5 different values from the Boost Converter and calculates the
 * most optimal duty cycle. Then, the CAN_BUS Shield sends the following values to another
 * FRDM-K64F receiving CAN_BUS board: outVoltage, inCurrent, inVoltage, outCurrent, efficiency. The
 * program continues this cycle until the user presses 'SW3', which stops the P&O algorithm. All outputs
 * are printed using the RawSerial command. To view the contents being printed, Windows users can use
 * the program 'PuTTy', and Mac users can use the built-in terminal function, 'screen.'
 *
 *****************************************************************************************************/


/**
*                                   Perturb & Observe Algorithm
*
* The most widely used algorithm is the P&O algorithm. The P&O algorithm perturbs the duty cycle
* which controls the power converter, in this way it takes steps over the p-v characteristic to 
* find the MPPT. This perturbation causes a new operating point with a different output power. In 
* case this output power is larger than the previous output power, this point is set as the new 
* operating point. In case it is lower, the same power point is adjusted to a lower or higher working 
* voltage, depending on the previous step direction. (http://bit.ly/1L73nzE)
*
*
* Pulse Width (PW) is the elapsed time between the rising and falling edges of a single pulse.
* 
* Pulse Repetition Interval (PRI) is the time between sequential pulses.
*
* Pulse Repetition Frequency (PRF) is the reciprocal of PRI. The basic unit of measure for PRF
* is hertz (Hz). 
*
* Duty Cycle describes the "On Time" for a pulsed signal. We can report duty cycle in units of time, 
* but usually as a percentage. To calculate a signals' duty cycle, we need to know the signal's pulse
* width and repetition frequency.
*
**/

#include "mbed.h"
#include "seeed_can.h"
#include "stdlib.h"
 
//Define Constants
#define PWM_PERIOD_us        25
#define V_IN_MULT            50.989761
#define V_OUT_MULT           51.011235
#define I_IN_DIV             0.09776
#define I_OUT_DIV            0.09605
#define HALL_IN_NO_CURRENT   2.524
#define HALL_OUT_NO_CURRENT  2.514
#define AIN_MULT             3.3
#define MESSAGE_LENGTH       8
#define READING_COUNT        6

 // Create a PwmOut connected to the specific pin
 PwmOut mypwm(PTC11);

// Create a DigitalOut pin for a heartbeat
 DigitalOut led1(LED1);
 DigitalOut led2(LED2);

 // Create a hardware interrupt with an onboard switch
 InterruptIn sw3(PTA4);

 // Create four AnalogIn pins
 AnalogIn i_hall_in(PTB2);
 AnalogIn i_hall_out(PTB3);
 AnalogIn v_in(PTB10);
 AnalogIn v_out(PTB11);

 // A serial port (UART) for communication with other serial devices
 // RawSerial (PinName tx, PinName rx, const char *name=NULL)
 RawSerial pc(USBTX, USBRX);

 // A Ticker is used to call a function at a recurring interval
 // void Attach(T *tptr, void(T::*mptr)(void), float t)
 // Attach a member function to be called by the Ticker, specifying time in seconds
 Ticker timer;
 
 // prints contents that are in the 8 character array
void printData(char*); 

// converts a char variable into an 8 character array
void convertToCharArray(char*, float);

// prints out all the character elements inside an 8 character array
void printStatus(int);

// Global Variables and starting Voltage and Current values
int heartbeat = 0;
int start = 0;
float pulseWidth;
float dutyCycle;
float originalVoltage = 60;
float originalCurrent = 1;
float originalPower = originalVoltage * originalCurrent;

// Global Variables used for SEEED_CAN Transmitter
// initialize CAN_BUS pin values with 500k baud rate
SEEED_CAN can(SEEED_CAN_CS,SEEED_CAN_IRQ, SEEED_CAN_MOSI, SEEED_CAN_MISO, SEEED_CAN_CLK , 500000);
int id = 7; // ID of the CAN_BUS Message TODO: determine which ID to send
char can_data[MESSAGE_LENGTH] = {}; // data being transmitted over CAN_BUS

char *readingString[] = {
    "OutVoltage", "InCurrent",
    "InVoltage", "OutCurrent",
    "Efficiency"
};
int readingNumber = 0; // counter for which data is being transmitted

 void perturb_and_observe(void){
    
    /* Actual reading values */
    float inHallSensorRaw = i_hall_in.read() * AIN_MULT ;
    float outHallSensorRaw = i_hall_out.read() * AIN_MULT;
    float outCurrent = (outHallSensorRaw - HALL_OUT_NO_CURRENT)/ I_OUT_DIV;
    float outVoltage = (v_out.read() * AIN_MULT) * V_OUT_MULT; // 120V
    float inCurrent = (inHallSensorRaw - HALL_IN_NO_CURRENT)/ I_IN_DIV;
    float inVoltage = (v_in.read() * AIN_MULT) * V_IN_MULT;
    
    /* Test reading values */
    /*
    float outCurrent = 0.8930;
    float outVoltage = 119.281; // 120V
    float inCurrent = 1.23243;
    float inVoltage = 45.2132;
    */

    //pc.printf("outVoltage %.6f, inVoltage %.6f, inCurrent %.6f, outCurrent %.6f\r\n", outVoltage, inVoltage, inCurrent, outCurrent);

    float inPower = inVoltage * inCurrent; // Power = Voltage * Current
    float outPower = outPower = outVoltage * outCurrent;

    pc.printf("Input Readings: \r\n");
    pc.printf("Inputs: Voltage: %.6f, Current: %.6f, Power: %.6f \r\n", inVoltage, inCurrent, inPower);
    pc.printf("Outputs: Voltage: %.2f, Current: %.2f, Power: %.2f\r\n", outVoltage, outCurrent, outPower);

    float deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
    float deltaPower = inPower - originalPower;
    //pc.printf("Delta Voltage is: %.6f, Delta Power is: %.6f \r\n", deltaVoltage, deltaPower);

    if(deltaPower == 0){
        // continue code and skip everything else
    } else if(deltaPower > 0){
        if(deltaVoltage > 0){
            inVoltage += deltaVoltage; // increase duty cycle
        } else {
            inVoltage -= deltaVoltage; // decrease duty cycle
        }
    } else{ // deltaPower < 0
        if(deltaVoltage > 0){
            inVoltage -= deltaVoltage; // decrease duty cycle
        } else {
            inVoltage += deltaVoltage; // increase duty cycle
        }
    }
    originalVoltage = inVoltage; // replace old voltage with current voltage
    originalCurrent = inCurrent;
    originalPower = inPower; // replace old power with current power

    //compute duty cyle and set pulsewidth
    dutyCycle = (outVoltage - inVoltage) / (outVoltage);

    //pc.printf("Duty cycle to: %.2f\r\n", dutyCycle);

    // error handling: duty cycle must NOT go above 0.8, or 80%
    if(dutyCycle >= 0.8){
        dutyCycle = 0.80;
    }

    pulseWidth = dutyCycle * PWM_PERIOD_us; // duty cycle = pulsewidth/period -> pulsewidth = duty cycle * period

    // set the PWM period, specified in micro-seconds (int), keeping the duty cycle the same
    mypwm.period_us(PWM_PERIOD_us); 
    // set the PWM pulsewidth, specified in milli-seconds (int), keeping the period the same
    mypwm.pulsewidth_us(pulseWidth);

    // read(): return the current output duty-cycle setting, measured as a percentage (float)
    pc.printf("PWM: %f %%\r\n", mypwm.read() * 100);

    float efficiency = (outPower / inPower) * 100;

    pc.printf("Efficiency: %.2f %%\r\n", efficiency);
    pc.printf("\r\n");
    
    // Put all the mpptReadings into an array so they can be transmitted via CAN_BUS
    float mpptReadings[READING_COUNT] = {outVoltage, inCurrent, inVoltage, outCurrent, efficiency};
    
    pc.printf("CAN_BUS transmitting...\r\n");
    for(readingNumber = 0; readingNumber < READING_COUNT - 1; readingNumber){
        convertToCharArray(*&can_data, mpptReadings[readingNumber]); // convert float to an 8 char number
         if (can.write(SEEED_CANMessage(id, can_data, MESSAGE_LENGTH, CANData, CANStandard))) { 
            /*
            * CAN-BUS TRANSMIT will send mppt values in this order: 
            * "OutVoltage:", "InCurrent:", "InVoltage", "OutCurrent:, "Efficiency:"
            */
            pc.printf("%s: ", readingString[readingNumber]);
            printData(*&can_data);
            readingNumber++;
            led1 = !led1; // heartbeat
        }else{
            int reset_status = can.mode(SEEED_CAN::Reset); // reset canbus if there is a problem, returns 1 if successful, 0 otherwise
        }
        wait(0.5);
    } // end of for loop
    pc.printf("\r\n\r\n");
 }

/* This function gets called when 'SW3' of the onboard FRDM-K64F is pressed. */
 void interruptHandler(){
    start ^= 1; // flip between 0 or 1
    if(start == 1){
        timer.attach(&perturb_and_observe, 2); // interrupt every n seconds
    } else{
        timer.detach();
    }
 }

int main(void){  
    // \r is an escape character for the terminal emulator
    pc.printf("Program starting...\r\n");
    int can_open_status = can.open(500000, SEEED_CAN::Normal); // initialize CAN-BUS Shield
    printStatus(can_open_status); // prints status of initialization
    
    // perturb and observe algorithm will begin when SW3 is pressed. If pressed again, it will stop.
    sw3.rise(&interruptHandler);
    
    // heartbeat to make sure the program is running
    while(1){
        if(heartbeat == 0){
            led1 = !led1;
        } else{
            led2 = !led2;
        }
        wait(3); // flash heartbeat every n seconds
    }
}

/* Prints the status of CAN_BUS initialization */
void printStatus(int status){
     if(status == 1){
        pc.printf("CAN Bus Shield successfully initialized!\r\n\r\n"); 
    } else{
        pc.printf("CAN BUS Shield initialization failed...\r\n");    
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
        pc.printf("%c", *ptr++);
        counter++;
    }
    pc.printf("\r\n");
}