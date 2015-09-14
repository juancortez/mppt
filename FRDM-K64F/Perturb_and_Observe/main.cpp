/*************************** main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 14, 2015 at 14:26
 * Revised on: September 14, 2015 at 15:06
 *
 * Microcontroller: FRDM-K64F
 * Git Repository: https://github.com/juancortez-ut/mppt
 * Dependent Libraries: https://developer.mbed.org/users/mbed_official/code/mbed/
 * FRDM-K64F Pinout: https://developer.mbed.org/media/uploads/sam_grove/xk64f_page2.jpg.pagespeed.ic.XmUo-mk4LT.webp

 * Signal     | FRDM-K64F Pin  | Purpose
 *
 * PwmOut     |     PTD0       | Output PWM Signal to Boost Converter
 * DigitalOut |     LED1       | Heartbeat LED
 ****************************************************************************************/

 include "mbed.h"

// Create a DigitalOut connected to the specified pin
 DigitalOut myled(LED1);

int main(void){

	// constant reading and outputting
	while(1){
		myled = !myled; // toggle heartbeat to see that the program is running
		// read voltage
		// read current
		// calculate delta V
		// calculate delta Power
		wait(1); // set a one second delay per reading/writing
	}

}