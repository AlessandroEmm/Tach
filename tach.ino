#include <SwitecX25.h>

/*
  Copyright (c) 2015, Scott Brynen - snafu.ca
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted
  provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this list of
  conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, this list of
  conditions and the following disclaimer in the documentation and/or other materials provided
  with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Serial Meter Firmware
   1.0 - Oct 2015 SB - Initial Release; 3 speeds w/ meter select
*/

#include <SwitecX25.h>
const int STEPS = 315 * 3;  // 315 degrees of range = 315x3 steps = 945 steps
#define MAXRPM 7000.0
#define MAXPOSITION 724.0
#define MAXPOSITIONOFFSET 500.0 / 7000.0 
#define MINRPM 500.0
#define MINRPMPOSITION 0


byte engineCylinders = 4; // for tach calculation (pulses per revolution = 2 * cylinders / cycles)
byte engineCycles = 4; // for tach calculation
int refreshInterval = 375; // milliseconds between sensor updates
unsigned long previousMillis = 0;
int tachPin = 2;

 int lastPos = 0;
 int lastZero = 0;
 int currentRPM = 0;
 volatile int RPMpulses = 0;

SwitecX25 motor1(STEPS, 0, 1, 3, 4);

/***
  setup() - set the IO pins, and sweep the meter to reset
***/
void setup(void) {
  pinMode(tachPin, INPUT_PULLUP); // enable internal pullup for tach pin
  attachInterrupt(0, countRPM, FALLING);
  motor1.zero();
  motor1.setPosition(STEPS);
  motor1.updateBlocking();
  delay(2000);
    motor1.setPosition(502);
  motor1.updateBlocking();

   delay(2000);
      motor1.setPosition(724);
  motor1.updateBlocking();
     delay(2000);

  delay(2000);
    motor1.setPosition(502);
  motor1.updateBlocking();

   delay(2000);
      motor1.setPosition(determineTachPos(2000));
  motor1.updateBlocking();
     delay(2000);
     
}

/***
  loop() - main code here
***/
void loop(void) {
  static bool forward = true;
  if(millis() - previousMillis > refreshInterval)
  {
    previousMillis = millis();
    currentRPM = getRPM();
    motor1.setPosition(determineTachPos(currentRPM));
  }

  motor1.update();
}

void countRPM()
{
  RPMpulses++;
}

int getRPM()
{
  int RPM = int(RPMpulses * (30000.0 / float(refreshInterval)) * engineCycles / engineCylinders  ); // calculate RPM
  RPMpulses = 0; // reset pulse count to 0
  RPM = min(7000, RPM); // don't return value larger than 7000
  return RPM;
}

int determineTachPos(int currentRPM) {
   if (currentRPM < MINRPM) return 0;
   else if (currentRPM > MAXRPM) return 800;
   else  { 
    int rpmPosition =  ((MAXPOSITION/MAXRPM)* float(currentRPM)) * (1+(MAXPOSITIONOFFSET-(MINRPM/ float(currentRPM)))) ;
    return rpmPosition; }
  }
