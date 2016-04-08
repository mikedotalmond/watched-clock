#include <PulseInZero.h>
#include "Pulser.h"

/**
* @author Mike Almond - https://github.com/mikedotalmond
* */

// enable debug/trace over Serial...
#define DEBUG false

Pulser pulse;

const float SpeedOfSound = 343.2; // ~speed of sound (m/s) in air, at 20°C         
const float MicrosecondsPerMillimetre = 1000.0 / SpeedOfSound; // microseconds per millimetre - sound travels 1 mm in ~2.9us
const float MicrosecondsToMillimetres = (1.0 / MicrosecondsPerMillimetre);
const float MicrosecondsToMillimetres2 = MicrosecondsToMillimetres / 2.0; // beam travels the distance twice... so halve the time.

const int PING_ECHO_PIN = 2; // interrupt zero on uno,nano
const int PING_TRIGGER_PIN = 4;

const int CLOCK_A = 7;
const int CLOCK_B = 8;
//
const float minMotorFreq = 8.0;
const float maxMotorFreq = 16.0;

// keeping track of time
unsigned long lastMicros;


// ping <-> echo
const float maxDistance=1500; //mm
const float minDistance=100; //mm
unsigned long pingTimer;
const unsigned long pingDelay = 250000; // send out a ping every .25 seconds  
const unsigned long pingDelay_PowerSave = 2500000; // send out a ping every 2.5 seconds 

const unsigned long PowersaveTimeoutMicros = 1000000 * 60 * 5; // save power after 5 mins of no presnce detection
unsigned long microsSinceDetection = 0;
bool powerSave = false; // when power saving, clock motor stops, and the ping rate reduces

// sensed distance
float millimetres = 0.0;
float normalisedDistance = 0; // normalised 0-1

void setup() {
  
  #if DEBUG 
    Serial.begin(9600); 
  #endif
  
  lastMicros = 0;
  pingTimer = 0;
  
  pulse.setup(CLOCK_A, CLOCK_B, 8.0);
  
  pinMode(PING_TRIGGER_PIN, OUTPUT);
  digitalWrite(PING_TRIGGER_PIN, LOW);
  // set up PulseInZero (pulseIn alternative using interrupt 0 - pin2 on an Arduino Uno)
  // use PulseInZero::begin() in place of pulseIn() and pulseComplete will fire when a pulse completes
  PulseInZero::setup(pingPulseComplete);
}

unsigned long pulseTimer = 0;
void loop() {
  
  unsigned long time = micros();
  unsigned long dt = time - lastMicros;
  lastMicros = time;

  //
  // squarewave clock pulse driver
  pulse.step(dt);
  pulseTimer += dt;  
  unsigned long pulseTickInterval = 500000 + 1000000 * (1-normalisedDistance);
  if(pulseTimer > pulseTickInterval*2) pulseTimer = 0;
  //
  bool doPulse = (pulseTimer < pulseTickInterval || normalisedDistance > 0.5) && normalisedDistance > 0.0;
  if(!doPulse || powerSave) pulse.pause();
  else if(pulse.paused) pulse.paused = false;

  //
  // ultrasonic range finder
  pingTimer += dt;
  if(pingTimer > (powerSave ? pingDelay_PowerSave : pingDelay)){  
    pingTimer = 0;
    ping();
  }
  
  if(!powerSave && normalisedDistance == 1.0){
    microsSinceDetection += dt;
    if(microsSinceDetection > PowersaveTimeoutMicros){        
      powerSave = true;
      pulse.setFrequency(minMotorFreq);      
      #if DEBUG
      Serial.println("POWERSAVE on");
      #endif
    }
  }
}



/**
* Trigger the outward ping pulse
*/
void ping(){
  digitalWrite(PING_TRIGGER_PIN, HIGH);
  delayMicroseconds(10); 
  digitalWrite(PING_TRIGGER_PIN, LOW);  
  PulseInZero::begin(); // start listening for the echo pulse on interrupt 0
}

/**
* Pulse complete callback hanlder for PulseInZero 
* @param duration - pulse length in microseconds
*/
void pingPulseComplete(unsigned long duration) {

  int millimetres = MicrosecondsToMillimetres2 * duration;
  
  if(millimetres >= maxDistance){
    // out of range (http://users.ece.utexas.edu/~valvano/Datasheets/HCSR04b.pdf)
    millimetres = maxDistance;
  } else if (millimetres < minDistance) {
    millimetres = minDistance;
  } else if(powerSave){
    
    powerSave = false;
    microsSinceDetection = 0;
    normalisedDistance = 0;
    pulse.setFrequency(minMotorFreq);
    
    #if DEBUG 
    Serial.println("POWERSAVE off");
    #endif
    
    return;
  }

  float val = (millimetres-minDistance)/(maxDistance-minDistance);
  normalisedDistance += (val-normalisedDistance) * .333; // 
  
  if(normalisedDistance > 0.999) normalisedDistance = 1.0;
  else if (normalisedDistance < 0.001) normalisedDistance = 0;
  
  float freq = minMotorFreq + (maxMotorFreq-minMotorFreq) * normalisedDistance;  
  pulse.setFrequency(freq);
    
  #if DEBUG 
    Serial.print(normalisedDistance);
    Serial.print(" - ");
    Serial.print(freq);
    Serial.println(" Hz");
  #endif
}
