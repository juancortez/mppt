/*************************** test_main.cpp ***************************************************
 * Maximum Power Point Tracker Project for EE 464R
 *
 * Perturb and Observe Algorithm: Testing File
 *
 * Author: Juan Cortez
 * Team: Angus Ranson, Muhammad Bukhari, Rana Madkour, Josh Frazor, Zach Pavlich
 * Created on: September 16, 2015 at 14:23
 * Revised on: September 21, 2015 at 14:08
 *
 * Purpose: The purpose of this file is to test the P&O algorithm and replicate
 * hardware inputs through the use of the terminal command line (or) the Eclipse IDE.
 * This program reads inputs from an input file called, "inputs.txt."
 *
 * Instructions: These instructions are written for Linux/Unix.
 *			     Installing compiler for Linux and Mac:
 *					- For Mac, installing XCode command line tools (https://developer.apple.com/xcode/)
 *					- For Linux, run $sudo apt-get install g++
 *				 To compile code: $g++ -o runTest test_main.cpp
 * 				 To run code: $./runTest
 *
 * 				 These instructions are written for Windows users.
 * 				 Install an IDE (preferable Eclipse)
 *				 Create a new project, and make a new file called main.cpp
 *					- copy and paste the entire code below
 * 				 Right click on the project in the Project explorer view, and create a new file called inputs.txt
 * 				 Follow the instructions below, titled 'File Format for "inputs.txt"'
 *
 *
 * File Format for "inputs.txt": In order for this program to appropriately read the input text file,
 * 								 it must be formatted in the following way: (spaces and enters important)
 * 								 [Input Voltage] [Input Current]
 *								 [Original Duty Cycle] [Original Voltage] [Original Current]
 *								 (empty line)
 *							     [Input Voltage #1] [Input Current #1]
 *							     ...
 *							     [Input Voltage #n] [Input Current #n]
 *
 *****************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*
* Debugging P&O Algorithm
*
* Inputs: input voltage and input current
* Output: PWM
*/

//TODO: Read input values from a text file with random values to disable manual entry
int main ( int argc, char **argv){

	FILE *ptr_file; // pointer to the external file
	char fileName[11] = "inputs.txt"; // name of the file (with null termination)

	ptr_file = fopen(fileName,"r"); //The fopen statement opens a file “output.txt” in the write (w) mode.
	if (!ptr_file){
		printf("The filename %s does not exist.", fileName);
		return 0;
	}

	int reading = 1; // counter to keep track of readings
	char buf[100]; // can read 100 characters per line
	printf("\n\t\t\t----------Program starting----------\n\n");

	// TODO: Angus spoke about a scale factor to multiple the readings by. Need to implement this.
	// read inputVoltage and inputCurrent from textfile
	double inVoltage, inCurrent, inPower;
	fgets(buf, 100, ptr_file);
	inVoltage = atof(strtok(buf, " -\n"));
	inCurrent = atof(strtok(NULL, " -\n"));
	inPower = inVoltage * inCurrent; // Power = Voltage * Current

	// read starting dutyCycle, originalVoltage, and originalCurrent from textfile
	double dutyCycle, originalVoltage, originalCurrent, originalPower;
	fgets(buf, 100, ptr_file);
	dutyCycle = atof(strtok(buf, " -\n"));
	originalVoltage = atof(strtok(NULL, " -\n"));
	originalCurrent = atof(strtok(NULL, " -\n"));
	originalPower = originalCurrent * originalVoltage;

	printf("Previous voltage is: %.6f, Previous current is: %.6f, Previous power is: %.6f \n\n", originalVoltage, originalCurrent, originalPower);

	while (fgets(buf,1000, ptr_file) != NULL){
		printf("\t\t\t----------Start of reading #%d----------\n\n", reading); // make space in terminal
		if(strlen(buf) != 1){
			inVoltage = atof(strtok(buf, " -\n"));
			inCurrent = atof(strtok(NULL, " -\n"));
			inPower = inVoltage * inCurrent;
		}
		printf("Input voltage is: %.6f, Input current is: %.6f, Input power is: %.6f \n", inVoltage, inCurrent, inPower);
		double deltaVoltage = inVoltage - originalVoltage; // also known as Perturbation
		double deltaPower = inPower - originalPower;
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
	}


	fclose(ptr_file);

}

