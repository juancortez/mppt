/*************************** main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 14, 2015 at 14:26
 * Revised on: October 7, 2015 at 13:05
 *
 * Microcontroller: FRDM-K64F
 * Git Repository: https://github.com/juancortez-ut/mppt
 * Dependent Libraries: https://developer.mbed.org/users/mbed_official/code/mbed/
 * FRDM-K64F Pinout: https://developer.mbed.org/media/uploads/sam_grove/xk64f_page2.jpg.pagespeed.ic.XmUo-mk4LT.webp
 *
 * Signal     | FRDM-K64F Pin       | Purpose
 *
 * PwmOut     |     PTD0            | Output PWM Signal to Boost Converter
 * DigitalOut |     LED1            | Heartbeat LED
 * AnalogIn   |     PTB2            | Hall Sensor In
 * AnalogIn   |     PTB3            | Hall Sensor Out
 * AnalogIn   |     PTB10           | Voltage In
 * AnalogIn   |     PTB11           | Voltage Out
 * RawSerial  |     USBTX, USBRX    | Serial Communication
 * 
 * Serial Communication on MAC
 * 
 * $ cd /dev && screen `ls | grep tty.usbmodem`
 ****************************************************************************************/

 /**
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
* Duty 
**/

/**
* TODO: Write interrupt service routine.
* 
* Interrupt Addresses: https://developer.mbed.org/users/mbed_official/code/mbed/file/e188a91d3eaa/TARGET_K64F/MK64F12.h
* Timer Interrupt: https://developer.mbed.org/handbook/Ticker
* InterruptManager: https://developer.mbed.org/users/minicube/code/mbed-src-LPC1114FN28/docs/b3acfef78949/classmbed_1_1InterruptManager.html
*  NOTE: If I use interrupt manager, import #include <InterruptManager.h>
*
*/

#include "mbed.h"
 
//Define Constants
#define PWM_PERIOD_us        25
#define V_IN_MULT            50.989761
#define V_OUT_MULT           51.011235
#define I_IN_DIV             0.09776
#define I_OUT_DIV            0.09605
#define HALL_IN_NO_CURRENT   2.524
#define HALL_OUT_NO_CURRENT  2.514
#define AIN_MULT             3.3

 // Create a PwmOut connected to the specific pin
 PwmOut mypwm(PTD0);

// Create a DigitalOut pin for a heartbeat
 DigitalOut led1(LED1);
 DigitalOut led2(LED2);

 // Create two AnalogIn pins
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

// Global Variables and starting Voltage and Current values
 int heartbeat = 0;
float pulseWidth;
float dutyCycle;
float originalVoltage = 60;
float originalCurrent = 1;
float originalPower = originalVoltage * originalCurrent;


 void perturb_and_observe(void){
    float inHallSensorRaw = i_hall_in.read() * AIN_MULT ;
    float outHallSensorRaw = i_hall_out.read() * AIN_MULT;

    pc.printf("inHallSensorRaw %.6f, outHallSensorRaw %.6f\r\n", inHallSensorRaw, outHallSensorRaw);

    float outCurrent = (outHallSensorRaw - HALL_OUT_NO_CURRENT)/ I_OUT_DIV;
    float outVoltage = (v_out.read() * AIN_MULT) * V_OUT_MULT; // 120V
    float inCurrent = (inHallSensorRaw - HALL_IN_NO_CURRENT)/ I_IN_DIV;
    float inVoltage = (v_in.read() * AIN_MULT) * V_IN_MULT;

    pc.printf("outVoltage %.6f, inVoltage %.6f, inCurrent %.6f, outCurrent %.6f\r\n", outVoltage, inVoltage, inCurrent, outCurrent);

    float inPower = inVoltage * inCurrent; // Power = Voltage * Current
    pc.printf("Input voltage is: %.6f, Input current is: %.6f, Input power is: %.6f \r\n", inVoltage, inCurrent, inPower);
    float deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
    float deltaPower = inPower - originalPower;
    pc.printf("Delta Voltage is: %.6f, Delta Power is: %.6f \r\n", deltaVoltage, deltaPower);

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
    pulseWidth = dutyCycle * PWM_PERIOD_us; // duty cycle = pulsewidth/period -> pulsewidth = duty cycle * period

    // set the PWM period, specified in micro-seconds (int), keeping the duty cycle the same
    mypwm.period_us(PWM_PERIOD_us); 
    // set the PWM pulsewidth, specified in milli-seconds (int), keeping the period the same
    mypwm.pulsewidth_us(pulseWidth);

    // read(): return the current output duty-cycle setting, measured as a percentage (float)
    pc.printf("pwm set to %.2f %%\r\n", mypwm.read() * 100);
    pc.printf("\r\n");
 }

int main(void){
    // \r is an escape character for the terminal emulator
    pc.printf("Program starting...\r\n");
    timer.attach(&perturb_and_observe, 2); // interrupt every n seconds

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