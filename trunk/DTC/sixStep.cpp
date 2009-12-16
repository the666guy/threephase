/*
 * sixStep.cpp
 *
 *  Created on: Dec 13, 2009
 *      Author: will
 *      Copyright (c) 2009 Will Brunner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */

#include "DTC.h"
void setup() {
	//set up the output pins (nonportable)
	pinMode(UH, OUTPUT);
	UH_OFF;
	pinMode(VH, OUTPUT);
	VH_OFF;
	pinMode(WH, OUTPUT);
	WH_OFF;
	pinMode(UL, OUTPUT);
	UL_OFF;
	pinMode(VL, OUTPUT);
	VL_OFF;
	pinMode(WL, OUTPUT);

	//initialize the variables
	readyFlag = 0;
	switchState = 0;
	clockwise = 0;
	fault = 0;
	torque = 0;
	flux[0] = 0;
	flux[1] = 0;
	ADReg[0] = 0;//Iu
	ADReg[1] = 0;//Iv
	ADReg[2] = 0;//Iw
	ADReg[3] = 0;//Vbus
	chanPtr = 0;//goes 0--2

	//set up the ADC (nonportable)
	cli();
	ADMUX = 1;//start by reading Iv
	ADCSRB = 0;
	ADCSRA = 0xCF;//turn on the ADC, enable the ADC interrupt, and set the prescaler to 64, so the ADC clock will run at FCLK/64.  Samples take 14.5 ADC clocks, so about 58 microseconds @ 16MHz.  This results in a little less that 10-bit accuracy

	//set up timer 0 for the switch loop.  6 switches are done per cycle, so the frequency is speed*6.  We set the prescaler to 1024, so the minimum speed is about 10Hz. We put the timer in CTC mode, so it resets to 0 when a compare is made
	TCCR0A = 0x2;//00000010
	TCCR0B = 0x5;//00000101
	OCR0A = 0xFF;//set speed to minimum
	//enable interrupt on compare A:
	TIMSK0 = 0x2;//00000010
	sei();
}

//ADC interrupt service routine (nonportable)
ISR(ADC_vect) {//this interrupt is serviced every time the ADC completes a conversion
	ADReg[chanPtr+1] = ADCL;
	ADReg[chanPtr+1] += ADCH<<8;
	//ADReg[0] never gets updated-- consider doing so as a sanity check
	//TODO:add fault logic here
	chanPtr++;
	chanPtr = chanPtr%3;
	ADMUX = (chanPtr+1);//set the mux to the next channel.  We use ADC 1,2, and 3
	ADCSRA |= (1<<ADSC);//start another conversion
	readyFlag = 1;
}
//timer 0 interrupt service routine. (nonportable)
ISR(TIMER0_COMPA_vect ,ISR_NOBLOCK){//this one is run every time timer0 expires, and changes the switch states
	if(switchState == 0){
		if(clockwise) O_TO_VI;
		else O_TO_I;
	}
	else if(switchState == 1){
			if(clockwise) I_TO_VI;
			else I_TO_II;
	}
	else if(switchState == 2){
		if(clockwise) II_TO_I;
		else II_TO_III;
	}
	else if(switchState == 3){
		if(clockwise) III_TO_II;
		else III_TO_IV;
	}
	else if(switchState == 4){
		if(clockwise) IV_TO_III;
		else IV_TO_V;
	}
	else if(switchState == 5){
		if(clockwise) V_TO_IV;
		else V_TO_VI;
	}
	else if(switchState == 6){
		if(clockwise) VI_TO_V;
		else VI_TO_I;
	}
	switchState++;
if(switchState == 7) switchState = 1;
}
void loop() {
	if (readyFlag) {//only run the logic if we have a complete set of ADC data
		readyFlag = 0;
		//update flux vector
		switch (switchState) {
		case 1:
			flux[0] += ADReg[VBUS] * 2;
			break;
		case 2:
			flux[0] += ADReg[VBUS];
				//(ADReg[VBUS] * 7) >> 2;//sqrt(3)
			flux[1] += (ADReg[VBUS] * 7) >> 2;//sqrt(3)
			break;
		case 3:
			flux[0] -= ADReg[VBUS];
			flux[1] += (ADReg[VBUS] * 7) >> 2;//sqrt(3)
			break;
		case 4:
			flux[0] -= ADReg[VBUS] * 2;
			break;
		case 5:
			flux[0] -= ADReg[VBUS];
			flux[1] -= (ADReg[VBUS] * 7) >> 2;//sqrt(3)
			break;
		case 6:
			flux[0] += ADReg[VBUS];
			flux[1] -= (ADReg[VBUS] * 7) >> 2;//sqrt(3)
			break;
		}
		//calculate current vector ... consider using IU + IV + IW = 0 as a sanity check
		current[0] = (3* (ADReg[IW] - ADReg[IV])) >> 1;
		current[1] = (7*(ADReg[IW] + ADReg[IV])) >> 3;//FIXME
		//calculate torque
		torque = flux[0] * current[1] - flux[1] * current[0];
		Serial.println(torque);
	}
}
