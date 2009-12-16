/*
 * DTC.h
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


#ifndef DTC_H_
#define DTC_H_
//this file defines constants and macros for the DTC application.
//#include "WProgram.h"//include arduino libs
//#include <avr/interrupt.h>
#include <stdint.h>
//this section defines arduino pins for the arduino shield (nonportable)
#define UL 5
#define UH 6
#define VL 10
#define VH 9
#define WL 3
#define WH 11
//analog inputs
#define IU 0
#define IV 1
#define IW 2
#define VBUS 3
//this section defines macros for switching (nonportable)
#define UL_ON {digitalWrite(UL,HIGH);}
#define UH_ON {digitalWrite(UH,HIGH);}
#define VL_ON {digitalWrite(VL,HIGH);}
#define VH_ON {digitalWrite(VH,HIGH);}
#define WL_ON {digitalWrite(WL,HIGH);}
#define WH_ON {digitalWrite(WH,HIGH);}
#define UL_OFF {digitalWrite(UL,LOW);}
#define UH_OFF {digitalWrite(UH,LOW);}
#define VL_OFF {digitalWrite(VL,LOW);}
#define VH_OFF {digitalWrite(VH,LOW);}
#define WL_OFF {digitalWrite(WL,LOW);}
#define WH_OFF {digitalWrite(WH,LOW);}
#define BLANK {for(int i = 0; i < 1; i++);}//this is a delay for blanking time
//this section defines macros for transitions
#define O_TO_I {WL_OFF; BLANK; WH_ON;}
#define O_TO_II {WL_OFF; VL_OFF; BLANK; WH_ON; VH_ON;}
#define O_TO_III {VL_OFF; BLANK; VH_ON;}
#define O_TO_IV {VL_OFF; UL_OFF; BLANK; VH_ON; UH_ON;}
#define O_TO_V {UL_OFF; BLANK; UH_ON;}
#define O_TO_VI {WL_OFF; UL_OFF; BLANK; WH_ON; UH_ON;}

#define I_TO_O {WH_OFF; BLANK; WL_ON;}
#define I_TO_II {VL_OFF; BLANK; VH_ON;}
#define I_TO_III {WH_OFF; VL_OFF; BLANK; WL_ON; VH_ON;}
#define I_TO_IV {WH_OFF; VL_OFF; UL_OFF; BLANK; WL_ON; VH_ON; UH_ON;}
#define I_TO_V {WH_OFF; UL_OFF; BLANK; WL_ON; UH_ON;}
#define I_TO_VI {UL_OFF; BLANK; UH_ON;}

#define II_TO_I {VH_OFF; BLANK; VL_ON;}
#define II_TO_III {WH_OFF; BLANK; WL_ON;}
#define II_TO_IV {WH_OFF; UL_OFF; BLANK; WL_ON; UH_ON;}
#define II_TO_V {WH_OFF; VH_OFF; UL_OFF; BLANK; WL_ON; VL_ON; UH_ON;}
#define II_TO_VI {VH_OFF; UL_OFF; BLANK; VL_ON; UH_ON;}
#define II_TO_VII {UL_OFF; BLANK; UH_ON;}

#define III_TO_O {VH_OFF; BLANK; VL_ON;}
#define III_TO_I {VH_OFF; WL_OFF; BLANK; VL_ON; WH_ON;}
#define III_TO_II {WL_OFF; BLANK; WH_ON;}
#define III_TO_IV {UL_OFF; BLANK; UH_ON;}
#define III_TO_V {VH_OFF; UL_OFF; BLANK; VL_ON; UH_ON;}
#define III_TO_VI {WL_OFF; VH_OFF; UL_OFF; BLANK; WH_ON; VL_ON; UH_ON;}

#define IV_TO_I {WL_OFF; VH_OFF; UH_OFF; BLANK; WH_ON; VL_ON; UL_ON;}
#define IV_TO_II {WL_OFF; UH_OFF; BLANK; WH_ON; UL_ON;}
#define IV_TO_III {UH_OFF; BLANK; UL_ON;}
#define IV_TO_V {VH_OFF; BLANK; VL_ON;}
#define IV_TO_VI {WL_OFF; VH_OFF; BLANK; WH_ON; VL_ON;}
#define IV_TO_VII {WL_OFF; BLANK; WH_ON;}

#define V_TO_O {UH_OFF; BLANK; UL_ON;}
#define V_TO_I {WL_OFF; UH_OFF; BLANK; WH_ON; UL_ON;}
#define V_TO_II {WL_OFF; VL_OFF; UH_OFF; BLANK; WH_ON; VH_ON; UL_ON;}
#define V_TO_III {VL_OFF; UH_OFF; BLANK; VH_ON; UL_ON;}
#define V_TO_IV {VL_OFF; BLANK; VH_ON;}
#define V_TO_VI {WL_OFF; BLANK; WH_ON;}

#define VI_TO_I {UH_OFF; BLANK; UL_ON;}
#define VI_TO_II {VL_OFF; UH_OFF; BLANK; VH_ON; UL_ON;}
#define VI_TO_III {WH_OFF; VL_OFF; UH_OFF; BLANK; WL_ON; VH_ON; UL_ON;}
#define VI_TO_IV {WH_OFF; VL_OFF; BLANK; WL_ON; VH_ON;}
#define VI_TO_V {WH_OFF; BLANK; WL_ON;}
#define VI_TO_VII {VL_OFF; BLANK; VH_ON;}

#define VII_TO_I {VH_OFF; UH_OFF; BLANK; VL_ON; UL_ON;}
#define VII_TO_II {UH_OFF; BLANK; UL_ON;}
#define VII_TO_III {WH_OFF; UH_OFF; BLANK; WL_ON; UL_ON;}
#define VII_TO_IV {WH_OFF; BLANK; WL_ON;}
#define VII_TO_V {WH_OFF; VH_OFF; BLANK; WL_ON; VL_ON;}
#define VII_TO_VI {VH_OFF; BLANK; VL_ON;}

//define global variables.  They all have to be volatile because they get changed during interrupts
volatile uint8_t fault;
volatile float flux[2];
volatile int32_t current[2];
volatile int32_t torque;
volatile int32_t ADReg[4];
volatile uint8_t chanPtr;
volatile uint8_t switchState;
volatile uint8_t sector;
volatile int32_t torqueSetpoint;
volatile uint32_t fluxSetpoint;
volatile int32_t upperTorque;
volatile int32_t lowerTorque;
volatile uint32_t upperFlux;
volatile uint32_t lowerFlux;
volatile uint8_t clockwise;
volatile uint8_t readyFlag;
#endif /* DTC_H_ */
