/*
 *  sixStepSketch.c
 *  
 *
 *  Created by Will Brunner on 11/22/09.
 *  Copyright (c) 2009 Will Brunner
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *  THE SOFTWARE.
 */


int uh = 6;
int vh = 9;
int wh = 11;
int ul = 5;
int vl = 10;
int wl = 3;

void setup(){
	
	pinMode(uh,OUTPUT);
	pinMode(vh,OUTPUT);
	pinMode(wh,OUTPUT);
	pinMode(ul,OUTPUT);
	pinMode(vl,OUTPUT);
	pinMode(wl,OUTPUT);
	digitalWrite(uh, LOW);
	digitalWrite(vh, LOW);
	digitalWrite(wh, LOW);
	digitalWrite(ul, LOW);
	digitalWrite(vl, LOW);
	digitalWrite(wl, LOW);
	Serial.begin(115200);
}



void readAnalog(){  
	int a[6];
	for(int i = 0; i < 6; i++){
		a[i] = analogRead(i);
		Serial.print(a[i]);
		Serial.print(" ");
	}
	Serial.println("");
	int average = (a[0] + a[1] + a[2])/3;
	float cu = (a[0] - average)*0.01953;
	float cv = (a[1] - average)*0.01953;
	float cw = (a[2] - average)*0.01953;
	float vbus = (a[3] * 0.0537);
	Serial.print((int)(cu*1000.));
	Serial.print("mA ");
	Serial.print((int)(cv*1000.));
	Serial.print("mA ");
	Serial.print((int)(cw*1000.));
	Serial.print("mA ");
	Serial.print((int)(vbus*1000.));
	Serial.println("mV ");
}
void go(int del){
	
	
	//Serial.println("UV/W");
	digitalWrite(uh, HIGH);
	digitalWrite(vh, LOW);
	delay(del);
	//readAnalog();
	digitalWrite(wl, LOW);
	digitalWrite(vl, HIGH);
	delay(del);
	//readAnalog();
	
	//Serial.println("U->W");
	digitalWrite(uh, LOW);
	digitalWrite(wh, HIGH);
	delay(del);
	//readAnalog();
	digitalWrite(ul, HIGH);
	digitalWrite(vl, LOW);
	delay(del);
	
	//readAnalog();
	//Serial.println("V->W");
	digitalWrite(vh, HIGH);
	digitalWrite(wh, LOW);
	delay(del);
	//readAnalog();
	digitalWrite(ul, LOW);
	digitalWrite(wl, HIGH);
	delay(del);
	//readAnalog();
}
void reverse(int del){
	
	digitalWrite(ul, HIGH);
	digitalWrite(wl, LOW);
	delay(del);
	
	digitalWrite(wh, HIGH);
	digitalWrite(vh, LOW);
	delay(del);
	
	digitalWrite(ul, LOW);
	digitalWrite(vl, HIGH);
	delay(del); 
	
	digitalWrite(uh, HIGH);
	digitalWrite(wh, LOW);
	delay(del);
	
	digitalWrite(vl, LOW);
	digitalWrite(wl, HIGH);
	delay(del);
	
	digitalWrite(uh, LOW);
	digitalWrite(vh, HIGH);
	delay(del);
	
	
	
	
	
	
	
	//readAnalog();
	//Serial.println("V->W");
	
	//readAnalog();
	
	//readAnalog();
}
void loop(){
	for(float speed = 0.01; speed < 0.2; speed+= 0.005){
		int d = (int)(1./speed);
		go(d);
	}
	//readAnalog();
	for(float speed = 0.2; speed > 0.01; speed-= 0.005){
		int d = (int)(1./speed);
		go(d);
	}
	//readAnalog();
	for(float speed = 0.01; speed < 0.2; speed+= 0.005){
		int d = (int)(1./speed);
		reverse(d);
	}
	//readAnalog();
	for(float speed = 0.2; speed > 0.01; speed-= 0.005){
		int d = (int)(1./speed);
		reverse(d);
	}
	
	//readAnalog();
	
	
}