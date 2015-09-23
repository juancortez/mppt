# How to build and execute test_main.cpp

##Instructions:   

	These instructions are written for Linux/Unix. 
		Installing compiler for Linux and Mac:
			- For Mac, installing XCode command line tools (https://developer.apple.com/xcode/)
			- For Linux, run $sudo apt-get install g++
		To compile code: $g++ -o runTest test_main.cpp
		To run code: $./runTest
		To close out of terminal: $[control]+c
	
	These instructions are written for Windows users.
		Install an IDE (preferable Eclipse)
		Create a new project, and make a new file called main.cpp
	 	Copy and paste the code from the test_main.cpp file in the repository
		Right click on the project in the Project explorer view, and create two new files called inputs.txt and outputs.txt
		Follow the instructions below, titled 'File Format for "inputs.txt"'
 
 
##File Format for "inputs.txt": 
In order for this program to appropriately read the input text file, it must be formatted in the following way: (spaces and enters important)


[Input Voltage] [Input Current]  
[Original Duty Cycle] [Original Voltage] [Original Current]    
(empty line)  
[Input Voltage #1] [Input Current #1]  
...  
[Input Voltage #n] [Input Current #n]  
