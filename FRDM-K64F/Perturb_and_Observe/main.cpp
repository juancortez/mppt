/*************************** main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 14, 2015 at 14:26
 * Revised on: October 4, 2015 at 16:48
 *
 * Microcontroller: FRDM-K64F
 * Git Repository: https://github.com/juancortez-ut/mppt
 * Dependent Libraries: https://developer.mbed.org/users/mbed_official/code/mbed/
 * FRDM-K64F Pinout: https://developer.mbed.org/media/uploads/sam_grove/xk64f_page2.jpg.pagespeed.ic.XmUo-mk4LT.webp

 * Signal     | FRDM-K64F Pin  | Purpose
 *
 * PwmOut     |     PTD0       | Output PWM Signal to Boost Converter
 * DigitalOut |     LED1       | Heartbeat LED
 * AnalogIn   |     A0         | Voltage In
 * AnalogIn   |     A1         | Current In
 * PWMOut     |     PTD0       | PWM Out
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

#include "mbed.h"
 
 #define V_IN_MULT 50.989761
 #define V_OUT_MULT 51.011235

 // Create a PwmOut connected to the specific pin
 PwmOut mypwm(PTD0);

// Create a DigitalOut pin for a heartbeat
 DigitalOut heartbeat(LED1);

 // Create two AnalogIn pins
 AnalogIn i_hall_in(PTB2);
 AnalogIn i_hall_out(PTB3);
 AnalogIn v_in(PTB10);
 AnalogIn v_out(PTB11);

 // A serial port (UART) for communication with other serial devices
 // Serial (PinName tx, PinName rx, const char *name=NULL)
 Serial pc(USBTX, USBRX);

int main(void){

    // \r is an escape character for the terminal emulator
    pc.printf("Program starting...\r\n");

    //TODO: find the starting duty, voltage, and current ratings
    //float dutyCycle = 0.5;
    float dutyCycle;
    float originalVoltage = 60;
    float originalCurrent = 1;
    float originalPower = originalVoltage * originalCurrent;
    
    while(1){
        heartbeat = !heartbeat; // toggle heartbeat to see that the program is running
          
        float inHallSensorRaw = i_hall_in.read();
        float outHallSensorRaw = i_hall_out.read();
        
        float outCurrent = (outHallSensorRaw - 2.514)/ 0.09605;
        float outVoltage = v_out.read() * V_OUT_MULT; // 120V
        float inCurrent = (inHallSensorRaw - 2.524)/ 0.09776;
        float inVoltage = v_in.read() * V_IN_MULT;
        
        float inPower = inVoltage * inCurrent; // Power = Voltage * Current
        pc.printf("Input voltage is: %.6f, Input current is: %.6f, Input power is: %.6f \n", inVoltage, inCurrent, inPower);
        float deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
        float deltaPower = inPower - originalPower;
        pc.printf("Delta Voltage is: %.6f, Delta Power is: %.6f \n", deltaVoltage, deltaPower);
        
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
        
        dutyCycle = (outVoltage - inVoltage) / (outVoltage);

        // TODO: Power Mosfet with new dutyCycle

        // set the PWM period, specified in micro-seconds (int), keeping the duty cycle the same
        mypwm.period_us(25); // TODO: figure out what number to put here
        // set the PWM pulsewidth, specified in milli-seconds (int), keeping the period the same
        mypwm.pulsewidth_us(dutyCycle);

        // read(): return the current output duty-cycle setting, measured as a percentage (float)
        pc.printf("pwm set to %.2f %%\r\n", mypwm.read() * 100);

        wait(1); // set a one second delay per reading/writing
    }

}

