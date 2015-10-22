// Converts float data into a char array data of size 8.

#include <stdio.h>
#include <string.h>

void printData(char*);
void convertToCharArray(char*, float);

int main(void){
	float test1 = 23.2345;
	float test2 = 120.1234;
	float test3 = 2.5343;
	float test4 = 0.12345;
	float test5 = 1980.2343123;
	char buffer[8] = {}; // initialize an empty data buffer

	char *ptr = buffer;
	convertToCharArray(ptr, test1);
	ptr = buffer;
	printData(ptr);
	memset(&buffer[0], 0, sizeof(buffer)); // empty array

	ptr = buffer;
	convertToCharArray(ptr, test2);
	ptr = buffer;
	printData(ptr);
	memset(&buffer[0], 0, sizeof(buffer)); // empty array

	ptr = buffer;
	convertToCharArray(ptr, test3);
	ptr = buffer;
	printData(ptr);
	memset(&buffer[0], 0, sizeof(buffer)); // empty array

	ptr = buffer;
	convertToCharArray(ptr, test4);
	ptr = buffer;
	printData(ptr);
	memset(&buffer[0], 0, sizeof(buffer)); // empty array

	ptr = buffer;
	convertToCharArray(ptr, test5);
	ptr = buffer;
	printData(ptr);
	memset(&buffer[0], 0, sizeof(buffer)); // empty array

	return 1;
}

void printData(char* ptr){
    int counter = 0;
    while(counter < 8){
        printf("%c", *ptr++);
        counter++;
    }
    printf("\n");
}

/*
* This function converts float numbers to a char array. Limit of 7 characters.
*
* Inputs: char* ptr - points to the beginning of buffer[8], the char array where
* 			  		  we will place the result
*		  float val - the value we want converted to a char array
*
* Output: 3 whole number values, dot, 3 decimal points
*
* Expected Result: 23.2345 ---->  [0, 2, 3, ., 2, 3, 4]
* Expected Result: 120.1234 ----> [1, 2, 0, ., 1, 2, 3]
*/
void convertToCharArray(char* ptr, float val){	
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
