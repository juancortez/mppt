/*************************** main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 14, 2015 at 14:26
 * Revised on: September 14, 2015 at 16:31
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
 * AnalogIn   |     A1 		   | Current In
 * PWMOut     |     PTD0       | PWM Out
 ****************************************************************************************/

 include "mbed.h"

 // Create a PwmOut connected to the specific pin
 PWMOut mypwm(PTD0);

// Create a DigitalOut pin for a heartbeat
 DigitalOut heartbeat(LED1);

 // Create two AnalogIn pins
 AnalogIn inputVoltage(A0);
 AnalogIn inputCurrent(A1);

 // A serial port (UART) for communication with other serial devices
 // Serial (PinName tx, PinName rx, const char *name=NULL)
 Serial pc(USBTX, USBRX);

int main(void){

	// \r is an escape character for the terminal emulator
	pc.printf("Program starting...\r\n");

	//TODO: find the starting duty, voltage, and current ratings
	float dutyCycle = 0.5;
	float originalVoltage = 3.3;
	float originalCurrent = 0.5;
	float originalPower = originalCurrent * originalCurrent;
	
	while(1){
		heartbeat = !heartbeat; // toggle heartbeat to see that the program is running

		// TODO: Angus spoke about a scale factor to multiple the readings by. Need to implement this.
		float inVoltage = inputVoltage.read();
		float inCurrent = inputCurrent.read();
		float inPower = inVoltage * inCurrent; // Power = Voltage * Current
		pc.printf("Input voltage is: %.2f, Input current is: %.2f, Input power is: %.2f", inVoltage, inCurrent, inPower);
		float deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
		float deltaPower = currentPower - originalPower;
		pc.printf("Delta Voltage is: %.2f, Delta Power is: %.2f," deltaVoltage, deltaPower);

		if(deltaPower == 0){
			break;
		} else if(deltaPower > 0){
			if(deltaVoltage > 0){
				dutyCycle += deltaVoltage; // increase PWM
			} else {
				dutyCycle -= deltaVoltage; // decrease PWM
			}
		} else{ // deltaPower < 0
			if(deltaVoltage > 0){
				dutyCycle -= deltaVoltage; // decrease PWM
			} else {
				dutyCycle += deltaVoltage; // increase PWM
			}
		}
		originalVoltage = inVoltage; // replace old voltage with current voltage
		originalPower = inPower; // replace old power with current power

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

