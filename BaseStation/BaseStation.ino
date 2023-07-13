/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

/* GLOBALS */
const int NETWORK_NUMBER_OF_NODES = 5; // Number of nodes on the network
const int TIME_SLOT = 2000; // In milliseconds (ms) 10^-3
const int THRESHOLD = 0; // In milliseconds Threshold for overlap
const int TIME_OUT = 5; // Number of phases till timeout

/* Timers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;

// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

// Function that checks for overlap in packets
bool clusterTransmissionError (unsigned long Current, unsigned long Previous) {
  unsigned long TimeDif = (Current - Previous);
  unsigned long Overlap = (unsigned long) (TIME_SLOT - THRESHOLD);
  if(TimeDif <= Overlap){
    return true;
  }
  else{
    return false;
  }
}

// Function that checks for overlap within a packet
bool packetTransmissionError ( ){
  // TODO Write this
}


void basestationFSM() {
  static enum { SYNC, ACTIVE, RESYNC } state = SYNC;

  switch (state) {
    case SYNC:
      CurrentTime = millis();
      Serial.println("S00");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      if(millis() >= TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES* CurrentTime){
        state = SYNC;
      }

      // Check for incoming packets 
      else{
        if(Serial.available() > 0) {  
          // Check for bulk packet overlap
          PreviousTime = CurrentTime;
          CurrentTime = millis();
          if(clusterTransmissionError(CurrentTime,PreviousTime)){
            state = RESYNC;
          }

          // Seperate bulk packet and check for errors
          else{
          // TODO: Recieve bulk packet and seperate it
          
          }    
      } 

      break;

    case RESYNC:

      break;

    default:
      state = SYNC;
      break;
  }
  }
}

void setup() {
  Serial.begin(9600); // Baud 9600
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  basestationFSM();
}
