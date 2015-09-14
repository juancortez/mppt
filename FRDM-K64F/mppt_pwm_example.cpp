include "mbed.h"

// Create a PwmOut connected to the specified pin
PwmOut mypwm(PTD0);

// Create a DigitalOut connected to the specified pin
DigitalOut myled(LED1);

int main() {
		
		// Sets the PWM period, specific in micro-seconds (int), keeping the duty cycle the same   
    mypwm.period_us(25);
		// Set the PWM pulsewidth, specific in milli-seconds (int), keeping the period the same
    mypwm.pulsewidth_us(10);
  
		// read(): return the current output duty-cycle setting, measured as a percentage (float)
    printf("pwm set to %.2f %%\n", mypwm.read() * 100);
    
    while(1) {
        myled = !myled;
        wait(1);
    }
}
