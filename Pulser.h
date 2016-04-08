#ifndef Pulser_h
#define Pulser_h

#include "Arduino.h"

class Pulser {
	
   public:
    Pulser();
	
    Pulser(int pinA, int pinB, float freq); 
    void setup(int clockA, int clockB, float freq);
	
  	void setPins(int a, int b);
  	void setFrequency(float freq);
  	
  	void reset();
    void pause();
    void resume();
  	void step(unsigned long microseconds);
  
    float freq = -1;
    bool paused = false;
    bool active = false;
    
  protected:
	
    int pinA;
    int pinB;
	  int pulsePin;

  	unsigned long pulseMicros; // current position within pulse
  	unsigned long pulseDuration; //
  	
    void pulseStart();
    void pulseEnd();
};
 
#endif
