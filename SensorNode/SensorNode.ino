/*
Multi-Hop TDMA SENSOR NODE (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/03/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

// FSM for a sensor node or a cluster head.

/* GLOBALS */
const int GLOBAL_ID = 2;  // Node Global ID on the network.
const int CLUSTER_ID = 1;  // ID corresponding to cluster that node belongs to.
const int CLUSTER_FLAG = 1;  // Whether or not the node serves as a cluster head
const int NETWORK_NUMBER_OF_NODES = 5; // Number of nodes on the network
const int TIME_SLOT = 2000; // In milliseconds (ms) 10^-3
const long ENERGY_HAVEST_RATE = 100; // Rate at each the energy is harvested
const bool CLUSTER_HEAR = true;  // If cluster flags can hear each other flag
String HEADER = "";

const int ERROR = 0; // Transmission Time

/* FLAGS... and stuff*/
bool led_state = false;
unsigned long wait_time = 0;

/* Fancy Custer Head Stuff */
// Something to store packet to be prepared, arr or a struct I think...doing arr for now
String packet ="";
String incomingString ="";
String SyncCheck = "";
int IdRecieved = 0;
int ClusterNumberReceived= 0;
int ClusterHeadCheck = 0;


// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

// Function to see if energy is still avaible
bool energyAvailable(int harvestRate) {
  if(random(0,100) <= harvestRate){
    return true;
  }
  else{
    return false;
  }
}

// State Machine
void nodeFSM() {
  static enum { DEAD, SYNC, WAIT, TRANSMIT } state = SYNC;

  switch (state) {
    case DEAD:
      if(energyAvailable(ENERGY_HAVEST_RATE)) {
        state = SYNC;
      }
      break;

    case SYNC:
      if(Serial.available() > 0) {   
        HEADER = HEADER + GLOBAL_ID + CLUSTER_ID + CLUSTER_FLAG; 
        incomingString = Serial.readStringUntil('\r');
        SyncCheck = incomingString.substring(0,1);
        IdRecieved = incomingString.substring(0,1).toInt();
        ClusterNumberReceived = incomingString.substring(1,2).toInt();
        ClusterHeadCheck = incomingString.substring(2,3).toInt();

        // Sync Recieved
        if(SyncCheck == "S"){
          wait_time = millis()+(GLOBAL_ID-1)*TIME_SLOT;
          state = WAIT;
        }

        // Node/ Cluster Recieved
        else if(ClusterNumberReceived == CLUSTER_ID){
          wait_time = millis()+((GLOBAL_ID-IdRecieved)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          if(CLUSTER_FLAG == 1){
            packet = packet+SyncCheck;

            // TODO figure out best way to deferiniate this
            //packet = packet+millis();
          }
          state = WAIT;
        }

               // Cluster head sync based on cluster head
        else if (CLUSTER_HEAR && CLUSTER_FLAG == 1 && ClusterHeadCheck == 1){
          wait_time = millis()+((GLOBAL_ID-IdRecieved)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
            packet = packet+SyncCheck;

            // TODO figure out best way to deferiniate this
            //packet = packet+millis();
          state = WAIT;
        }
        
        Serial.flush();
      }
      break;

    case WAIT:
      // TODO: Wait for time slot, wait for resync, wait for stuff...idk just wait
      if(millis() >= wait_time){
        state = TRANSMIT;
      }
      else{
        if(Serial.available() > 0) {
          incomingString = Serial.readStringUntil('\r');
          SyncCheck = incomingString.substring(0,1);
          IdRecieved = incomingString.substring(0,1).toInt();
          ClusterNumberReceived = incomingString.substring(1,2).toInt();

          // ReSync Recieved
          if(SyncCheck == "R"){
            // TODO: Implement Cluster Recync
            packet = "";
            state = SYNC;
          }

          // Store Data
          if(CLUSTER_FLAG == 1 && ClusterNumberReceived == CLUSTER_ID ){
            packet = packet+SyncCheck;

            // TODO figure out best way to deferiniate this
            //packet = packet+millis();
          }
          Serial.flush();
        }
      }
      break;

    case TRANSMIT:
      //Transmit

      String BulkPacket = HEADER+packet;
      Serial.println(BulkPacket);
      packet = "";
      //Change LED state
      led_state = !led_state;
      digitalWrite(LED,led_state);

      // Check for suffecient energy
      if(energyAvailable(ENERGY_HAVEST_RATE)){
        wait_time = millis()+(NETWORK_NUMBER_OF_NODES*TIME_SLOT)-ERROR;
        state = WAIT;
      }
      else{
        state = DEAD;
      }
      break;

    default:
      state = DEAD;
      break;
  }
}

void setup() {
  Serial.begin(9600); // Baud 9600
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  nodeFSM();
}
