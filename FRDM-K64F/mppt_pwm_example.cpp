include "mbed.h"

PwmOut mypwm(PTD0);

DigitalOut myled(LED1);

int main() {
    
    mypwm.period_us(25);
    mypwm.pulsewidth_us(10);
  
    printf("pwm set to %.2f %%\n", mypwm.read() * 100);
    
    while(1) {
        myled = !myled;
        wait(1);
    }
}
