/*
 * DTC.cpp
 *
 *  Created on: Nov 22, 2009
 *      Author: will
 *            Copyright (c) 2009 Will Brunner

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
 */
#include "DTC.h"
#include <iostream>//for testing only
#include <cmath>
using namespace std;//for testing only

void setup() {
	//set up the output pins (nonportable)
	/*
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
	*/
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
	uint32_t torqueTolerance = 1000000;
	uint32_t fluxTolerance = 10;
	torqueSetpoint = 10000000;
	fluxSetpoint = 10000;
	upperTorque = torqueSetpoint + torqueTolerance;
	lowerTorque = torqueSetpoint - torqueTolerance;
	upperFlux = fluxSetpoint + fluxTolerance;
	lowerFlux = fluxSetpoint - fluxTolerance;
	//set up the ADC (nonportable)
/*
	cli();
	ADMUX = 1;//start by reading Iv
	ADCSRB = 0;
	ADCSRA = 0xCF;//turn on the ADC, enable the ADC interrupt, and set the prescaler to 64, so the ADC clock will run at FCLK/64.  Samples take 14.5 ADC clocks, so about 58 microseconds @ 16MHz.  This results in a little less that 10-bit accuracy
	sei();
	*/
}
/*
//ADC interrupt service routine (nonportable
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
*/
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
			//	cout << "torque = " << (int)torque << "\n";//for testing only
				//calculate sector
				if(flux[0] > 0) {//sector 1, 2 or
					if(1.73*flux[1] >= flux[0]) sector =2;
		else if (1.73*flux[1] < -flux[0]) sector = 6;
		else sector = 1;
		}
		else{//sector 3,4 or 5
		if(1.73*flux[1] >= -flux[0]) sector = 3;
		else if (1.73*flux[1] < flux[0]) sector = 5;
		else sector = 4;
		}
		//cout << flux[0] << " " << flux[1] << " " << (int)sector << "\n";
		//do bang-bang control
		int8_t nextSwitchState = 0;
		uint32_t fluxMag = (uint32_t)sqrt(flux[0]*flux[0]+flux[1]*flux[1]);
		//cout << (int)torque << " " << fluxMag << "\n";
		if (torque >= upperTorque){
		//	cout << "torque is too high "<< torque << " " << upperTorque;
		if(fluxMag >= upperFlux){
		//	cout << " and flux is too high" << fluxMag << " " << upperFlux << "\n";
			if(clockwise) nextSwitchState = sector + 2;
			nextSwitchState = sector + 4;
		}
		else if(fluxMag < lowerFlux){
		//	cout << " and flux is too low" << fluxMag << " " << lowerFlux << "\n";
			if(clockwise) nextSwitchState = sector;
			else nextSwitchState = sector;
		}
		else{
		//	cout << " and flux is within band\n";
			nextSwitchState = 0;
		}
		}
		else if(torque < lowerTorque){
		//	cout << "torque is too low "<< (int)torque << " " << upperTorque << " " << fluxMag << " " << upperFlux <<" " << lowerFlux;
		if(fluxMag >= upperFlux){
		//	cout << " and flux is too high" << fluxMag << " " << upperFlux << "\n";
			if(clockwise) nextSwitchState = sector + 4;
			else nextSwitchState = sector + 2;
		}
		else if(fluxMag < lowerFlux){
		//	cout << " and flux is too low" << fluxMag << " " << lowerFlux << "\n";
			if(clockwise) nextSwitchState = sector + 5;
			else nextSwitchState = sector + 1;
		}
		else{
		//	cout << " and flux is within band\n";
			if(clockwise) nextSwitchState = sector + 5;
			else nextSwitchState = sector + 1;
		}
		}
		else nextSwitchState = 0;
		if(nextSwitchState > 6) nextSwitchState -= 6;
		//cout << "switching from state " << (int)switchState << " to " << (int)nextSwitchState << "\n";
		switch(switchState){
		case 0:
			switch(nextSwitchState){
				case 0:
					break;
				case 1:
					O_TO_I;
					switchState = 1;
					break;
				case 2:
					O_TO_II;
					switchState = 2;
					break;
				case 3:
					O_TO_III;
					switchState = 3;
					break;
				case 4:
					O_TO_IV;
					switchState = 4;
					break;
				case 5:
					O_TO_V;
					switchState = 5;
					break;
				case 6:
					O_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 1:
			switch(nextSwitchState){
				case 0:
					I_TO_O;
					switchState = 0;
					break;
				case 1:
					break;
				case 2:
					I_TO_II;
					switchState = 2;
					break;
				case 3:
					I_TO_III;
					switchState = 3;
					break;
				case 4:
					I_TO_IV;
					switchState = 4;
					break;
				case 5:
					I_TO_V;
					switchState = 5;
					break;
				case 6:
					I_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 2:
			switch(nextSwitchState){
				case 0:
					II_TO_VII;
					switchState = 7;
					break;
				case 1:
					II_TO_I;
					switchState = 1;
					break;
				case 2:
					break;
				case 3:
					II_TO_III;
					switchState = 3;
					break;
				case 4:
					II_TO_IV;
					switchState = 4;
					break;
				case 5:
					II_TO_V;
					switchState = 5;
					break;
				case 6:
					II_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 3:
			switch(nextSwitchState){
				case 0:
					III_TO_O;
					switchState = 0;
					break;
				case 1:
					III_TO_I;
					switchState = 1;
					break;
				case 2:
					III_TO_II;
					switchState = 2;
					break;
				case 3:
					break;
				case 4:
					III_TO_IV;
					switchState = 4;
					break;
				case 5:
					III_TO_V;
					switchState = 5;
					break;
				case 6:
					III_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 4:
			switch(nextSwitchState){
				case 0:
					IV_TO_VII;
					switchState = 7;
					break;
				case 1:
					IV_TO_I;
					switchState = 1;
					break;
				case 2:
					IV_TO_II;
					switchState = 2;
					break;
				case 3:
					IV_TO_III;
					switchState = 3;
					break;
				case 4:
					break;
				case 5:
					IV_TO_V;
					switchState = 5;
					break;
				case 6:
					IV_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 5:
			switch(nextSwitchState){
				case 0:
					V_TO_O;
					switchState = 0;
					break;
				case 1:
					V_TO_I;
					switchState = 1;
					break;
				case 2:
					V_TO_II;
					switchState = 2;
					break;
				case 3:
					V_TO_III;
					switchState = 3;
					break;
				case 4:
					V_TO_IV;
					switchState = 4;
					break;
				case 5:
					break;
				case 6:
					V_TO_VI;
					switchState = 6;
					break;
			}
			break;
		case 6:
			switch(nextSwitchState){
				case 0:
					VI_TO_VII;
					switchState = 7;
					break;
				case 1:
					VI_TO_I;
					switchState = 1;
					break;
				case 2:
					VI_TO_II;
					switchState = 2;
					break;
				case 3:
					VI_TO_III;
					switchState = 3;
					break;
				case 4:
					VI_TO_IV;
					switchState = 4;
					break;
				case 5:
					VI_TO_V;
					switchState = 5;
					break;
				case 6:
					break;
			}
			break;
		case 7:
			switch(nextSwitchState){
				case 0:
					break;
				case 1:
					VII_TO_I;
					switchState = 1;
					break;
				case 2:
					VII_TO_II;
					switchState = 2;
					break;
				case 3:
					VII_TO_III;
					switchState = 3;
					break;
				case 4:
					VII_TO_IV;
					switchState = 4;
					break;
				case 5:
					VII_TO_V;
					switchState = 5;
					break;
				case 6:
					VII_TO_VI;
					switchState = 6;
					break;
			}
			break;
		}

	}
}

int main(){
	setup();
	int i = 1;//for testing only
	while(1) {
		i++;
		//if(i%1000) clockwise ^= 1;
		readyFlag = 1;//for testing only
		ADReg[VBUS] = 10+i%5;//for testing only
		ADReg[2] = 100;//for testing only
		ADReg[3] = 100;//for testing only
		loop();
		//if(sector == 6){
		cout << flux[0] << " " << flux[1] << " " << current[0] << " " << current[1] <<" "<< (int)sector << "\n";
		//}
	}

}
