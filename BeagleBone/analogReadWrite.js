var b = require('bonescript');
b.pinMode('P9_14', b.OUTPUT);

/*
analogWrite(pin, value, [freq], [callback])

pin: the BeagleBone pin identifier
value: duty cycle of the PWM as a value between 0 and 1
freq: frequency of the PWM in Hz (default is 2kHz)
callback: called upon completion
*/
b.analogWrite('P9_14', 0.7);

/*
analogRead(pin, [callback])
pin: the pin identifier
callback: called upon completion

Return Value: Number from 0 to 1 where 0 is 0V and 1 is the max input 
voltage (1.8V)
*/
b.analogRead('P9_36', printStatus);

function printStatus(x){
	console.log('x.value = ' + x.value);
}
