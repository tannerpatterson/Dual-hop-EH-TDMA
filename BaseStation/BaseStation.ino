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
const int THRESHOLD = 0; // In milliseconds threshold for overlap
const int TIME_OUT = 5; // Number of phases till timeout
cont int ERROR = 0; // In milliseconds time for transmission error
const int CLUSTERHEADS [CLUSTERS] ={}; // Array full of custer head IDS

/* Timers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;

/* Counters */
int OutOfEnergyCount = 0;

/* Arrays */
unsigned long LastRecievedTime [CLUSTERS] = {};

/* Fancy Custer Head Stuff */
String packet ="";
String incomingString ="";
String SyncCheck = "";
int IdRecieved = 0;
int ClusterNumberReceived= 0;
int ClusterHeadCheck = 0;

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
      Serial.println("00S0");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      if(LastRecievedTime[OutOfEnergyCount] <= millis()-(TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES)-ERROR){
        int GlobalID = CLUSTERHEADS[OutOfEnergyCount];
        int ClusterID = OutOfEnergyCount + 1;
        String packet = GlobalID+ClusterID+"T0";
        Serial.println(packet);
      }

      // Check for incoming packets 
      else{
        if(Serial.available() > 0) {  
          //Check to make sure it is a CH packet
          incomingString = Serial.readStringUntil('\r');
          IdRecieved = incomingString.substring(0,1).toInt();
          ClusterHeadCheck = incomingString.substring(2,3).toInt();
          if(ClusterHeadCheck == 1){
          
            // Check for Cluster Head overlap (Might get deleted)
            PreviousTime = CurrentTime;
            CurrentTime = millis();
            if(clusterTransmissionError(CurrentTime,PreviousTime)){
              for(int index = 0; index < NETWORK_NUMBER_OF_NODES; index++){
                // Send Overlap Message
                if(IdRecieved == CLUSTERHEADS[index]){
                  int Cluster1 = index+1;
                  int Cluster2 = ((index-1)%CLUSTERS)+1;
                  String packet = IdRecieved+Cluster1+"H"+Cluster2;
                  Serial.println(packet);
                }
              }
            }

            //Fill Time Array

            // Seperate bulk packet
            else{            
            
            
          
            }  
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
