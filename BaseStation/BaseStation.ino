/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

const int NETWORK_NUMBER_OF_NODES = 5; // Number of nodes on the network
const int TIME_SLOT = 2000; // In milliseconds (ms) 10^-3
const int THRESHOLD = 0;
const int TIME_OUT = 5 // Number of phases till timeout


unsigned long CurrentReceiveTime = 0;
unsigned long PreviousReceiveTime = 0;

// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

// Function that checks for overlap in packets
bool clusterTransmissionError (unsigned long Current, unsigned long Previous) {
   TimeDif = (Current - Previous).toInt();
  if(TimeDif <= TIME_SLOT - THRESHOLD){
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

// Function that checks for a dead network
bool timeOut(unsigned long Previous){
  unsigned long TimeOutCheck = TIME_OUT*TIME_SLOT*NETWORK_NUMBER_OF_NODES*millies();
  if(Previous > TimeOutCheck){
    return true;
  }
  else{
    return false;
  }
}



void basestationFSM() {
  static enum { SYNC, ACTIVE, RESYNC } state = SYNC;

  switch (state) {
    case SYNC:
      Serial.println("S00");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      timeOut(PreviousReceiveTime);
      if(Serial.available() > 0) {  
        // Check for overlap in packets
        PreviousReceiveTime = CurrentReceiveTime;
        CurrentReceiveTime = millis();
        if(clusterTransmissionError(CurrentReceiveTime,PreviousReceiveTime)){
          state = RESYNC;
        }
        else{
        // TODO: Recieve bulk packet and seperate it
          
        }    
      } 

      break;

    case RESYNC:

      break;

    default:
      state = DEAD;
      break;
  }



void setup() {
  Serial.begin(9600); // Baud 9600
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  basestationFSM();
}
