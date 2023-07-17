/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

/* GLOBALS */
const int NETWORK_NUMBER_OF_NODES = 5; // Number of nodes on the network
const int CLUSTERS =2; // Number of clusters on the network
const int TIME_SLOT = 2000; // In milliseconds (ms) 10^-3
const int THRESHOLD = 0; // In milliseconds Threshold for overlap
const int TIME_OUT = 5; // Number of phases till timeout

/* Timers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;

/* Counters */
int OutOfEnergyCount = 0;

/* Arrays */
int ClusterHeadIDs [CLUSTERS] ={};
unsigned long LastRecievedTime [CLUSTERS] = {};

/* Flags */
bool FullArray = false;

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
  static enum { START, ACTIVE, RESTART } state = START;

  switch (state) {
    case START:
      CurrentTime = millis();
      Serial.println("00S");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      if(LastRecievedTime[OutOfEnergyCount] <= millis()-(TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES) && FullArray){
        int GlobalID = ClusterHeadIDs[OutOfEnergyCount];
        int ClusterID = OutOfEnergyCount + 1;
        String packet = GlobalID+ClusterID+"R";
        Serial.println("packet");
      }

      // Check for incoming packets 
      else{
        if(Serial.available() > 0) {  

          // Check for Cluster Head overlap
          PreviousTime = CurrentTime;
          CurrentTime = millis();
          if(clusterTransmissionError(CurrentTime,PreviousTime)){
            state = RESTART;
          }

          // Seperate bulk packet and check for errors
          else{
            
          // TODO: Recieve bulk packet and seperate it
          
          }    
      } 

      Serial.flush();
      OutOfEnergyCount = (OutOfEnergyCount+1)%CLUSTERS;
      break;

    case RESTART:
      // Send hault message to network
      Serial.println("00H");
      // Arbitrary wait to make sure all nodes are in sync state
      delay(TIME_SLOT);
      state = START;
      break;

    default:
      state = START;
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
