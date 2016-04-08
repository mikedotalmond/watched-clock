/*Copyright (c) 2013 Mike Almond - @mikedotalmond - https://github.com/mikedotalmond

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
THE SOFTWARE.*/

/**
* @author Mike Almond - https://github.com/mikedotalmond
* */


#include "Arduino.h"
#include "Pulser.h"

Pulser::Pulser() {
	
}

Pulser::Pulser(int pinA, int pinB, float freq){
	setup(pinA, pinB, freq);
}

/**
* @public
**/	
void Pulser::setup(int pinA, int pinB, float f){	
	setPins(pinA, pinB);
  setFrequency(f);
	reset();
  pulseStart();
}

void Pulser::setPins(int a, int b){
	pinA = a; pinMode(a, OUTPUT);
	pinB = b; pinMode(b, OUTPUT);
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
}

void Pulser::setFrequency(float f){
  if(freq!=f){
    freq=f;
    pulseDuration = 1000000 / freq / 2;
  }
}


/**
* @public 
* Called every time in main loop
**/
void Pulser::step(unsigned long microseconds){
	
	pulseMicros += microseconds;
	
	if(active){
		if(pulseMicros >= pulseDuration) {
		  pulseEnd();
		}
	} else if(!paused){ 
		pulseStart();
	}
}

void Pulser::pause(){
  paused = true;
}
void Pulser::resume(){
  paused = false;
}

/**
* @public 
* Reset the counters and re-trigger a tick/pulse
*/
void Pulser::reset(){
	active = false;
	pulseMicros = 0;
	digitalWrite(pinA, LOW);
	digitalWrite(pinB, LOW);
}

void Pulser::pulseEnd(){
  active = false;
  digitalWrite(pulsePin, LOW);
}
/**
*
*/
void Pulser::pulseStart(){
	active = true;
	pulseMicros = 0;
	pulsePin = pulsePin == pinA ? pinB : pinA;
	digitalWrite(pulsePin, HIGH);
}
