/*************************** test_main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 * 
 * Perturb and Observe Algorithm: Testing File
 * 
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 16, 2015 at 14:23
 * Revised on: September 16, 2015 at 15:04s
 *
 * Purpose: The purpose of this file is to test the P&O algorithm and replicate 
 * hardware inputs through the use of the terminal command line.
 * 
 * Instructions: These instructions are written for Linux/Unix. 
 *			     Installing compiler for Linux and Mac:
 *					- For Mac, installing XCode command line tools (https://developer.apple.com/xcode/)
 *					- For Linux, run $sudo apt-get install g++
 *				 To compile code: $g++ -o runTest test_main.cpp
 * 				 To run code: $./runTest [inputVoltage] [inputCurrent]
 * 				 (i.e.) $./runTest 3.3 0.5
 *				 To close out of terminal: $[control]+c
 *
 ******************************************s**********************************************/
#include <stdio.h>
#include <stdlib.h>

/*
* Debugging P&O Algorithm
* 
* Inputs: input voltage and input current
* Output: PWM
*/

//TODO: Read input values from a text file with random values to disable manual entry
int main ( int argc, char **argv){

	int reading = 1; // counter to keep track of readings

	// expecting two inputs, input voltage and input current
	if ( argc != 3 ){
        /* We print argv[0] assuming it is the program name */
        printf( "The filename %s must use 3 arguments. You currently have %d argument(s).", argv[0], argc);
        return 0; // main error code
    }

	printf("\n\t\t\t----------Program starting----------\n\n");

	int firstArgument = true;
	float dutyCycle, originalVoltage, originalCurrent;
	printf("Please input starting duty cycle, original voltage, and original current, separated by spaces.\n");
	scanf("%f %f %f", &dutyCycle, &originalVoltage, &originalCurrent);
	float originalPower = originalCurrent * originalCurrent;
	
	while(1){
		// TODO: Angus spoke about a scale factor to multiple the readings by. Need to implement this.
		double inVoltage, inCurrent;
		
		// this is read from terminal
		if(firstArgument == true){
			sscanf(argv[1], "%lf", &inVoltage);
			sscanf(argv[2], "%lf", &inCurrent);
			firstArgument = false;
		} else{
			// ask user for new "hardware inputs"
			printf("Please input inputVoltage and inputCurrent readings:");
			scanf("%lf %lf", &inVoltage, &inCurrent);
			printf("\n");
		}

		float inPower = inVoltage * inCurrent; // Power = Voltage * Current
		printf("Input voltage is: %.6f, Input current is: %.6f, Input power is: %.6f \n", inVoltage, inCurrent, inPower);

		float deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
		float deltaPower = inPower - originalPower;
		printf("Delta Voltage is: %.2f, Delta Power is: %.2f \n", deltaVoltage, deltaPower);

		if(deltaPower == 0){
			// continue code and skip everything else
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
		printf("Duty Cycle set to: %lf\n\n", dutyCycle);

		printf("\t\t\t----------End of reading #%d----------\n\n", reading++);

		printf("Press ENTER to continue...."); // so the while loop can pause
		getchar();
		getchar();
		printf("\t\t\t----------Start of reading #%d----------\n\n", reading); // make space in terminal
	}

}

