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
const int CLUSTER_NODES = 2;
const bool CLUSTER_HEAR = false;  // If cluster flags can hear each other flag
const int TIME_SLOT = 250; // In milliseconds (ms) 10^-3
const int ERROR = 0; // Transmission Time
const long ENERGY_HAVEST_RATE = 100; // Rate at each the energy is harvested
String HEADER = "2110";

/* FLAGS... and stuff*/
bool led_state = false;
bool Trans = false;

/* Timers */
unsigned long CurrentTime = 0;
unsigned long LastTime = 0;
unsigned long wait_time = 0;

/* Fancy Custer Head Stuff */
String packet ="";
String incomingString ="";
String SyncCheck = "";
int GlobalIDReceived = 0;
int ClusterIDReceived= 0;
int ClusterHeadCheck = 0;
int OverlapCheck = 0;

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
          // Timer to remove error and reduce packet size   
          CurrentTime = millis();
          incomingString = Serial.readStringUntil('\r');
          String g= incomingString.substring(0,1);         
          GlobalIDReceived = g.toInt();
          String c= incomingString.substring(1,2);
          ClusterIDReceived = c.toInt();
          SyncCheck = incomingString.substring(2,3);
          ClusterHeadCheck = SyncCheck.toInt();
          String o =incomingString.substring(3,4);
          OverlapCheck = o.toInt();

        /* Checking for Base Station Messages */
        // Basestation Sync Recieved
        // Might delete the ClusterIDReceived == CLUSTER_ID
        if(SyncCheck == "S" && (ClusterIDReceived == CLUSTER_ID || ClusterIDReceived == 0)){
          int Wait = (GLOBAL_ID-1)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          state = WAIT;
        }

        // Basestation Timeout Recieved (Case cluster out of energy)
        else if(SyncCheck == "T" && ClusterIDReceived == CLUSTER_ID){
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          state = WAIT;
        }

        // Basestation Overlap Recieved (Case cluster out of order)
        else if(SyncCheck == "O" && (ClusterIDReceived == CLUSTER_ID || OverlapCheck == CLUSTER_ID)){
          
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          state = WAIT;
        }


        /* Checking for Node / Cluster Head Messages */
        // Node/ Cluster Recieved
        else if(ClusterIDReceived == CLUSTER_ID){
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          if(CLUSTER_FLAG == 1){
            // Might Remove Node IDs
            if(packet == ""){
              unsigned long E = (unsigned long) ((NETWORK_NUMBER_OF_NODES-CLUSTER_NODES)*TIME_SLOT);
              unsigned long Dif = CurrentTime-LastTime-E;
              LastTime = CurrentTime;
              packet = packet+ "," + GlobalIDReceived + "," + Dif;
            }
            else{
              unsigned long Dif = CurrentTime-LastTime;
              LastTime = CurrentTime;
              packet = packet+ "," + GlobalIDReceived + "," + Dif;
            }
          }
          state = WAIT;
        }

        // Cluster head sync based on cluster head
        else if (CLUSTER_HEAR && CLUSTER_FLAG == 1 && ClusterHeadCheck == 1){
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;          
          state = WAIT;
        }
        
        Serial.flush();
      }
      break;

    case WAIT:
      // TODO: Wait for time slot, wait for resync, wait for stuff...idk just wait
      CurrentTime = millis();
      if(CurrentTime >= wait_time){
        state = TRANSMIT;
        Trans = true;
      }

      else{
        if(!Trans){
        //packet = packet + "FFFF" + millis();
        //Serial.println(packet);
        //packet = "";
        if(Serial.available() > 0) {
          //packet = packet + "bbbb" + millis();
          //Serial.println(packet);
          //packet = "";
          incomingString = Serial.readStringUntil('\r');
          //packet = packet + "aaaa," + incomingString + ","+millis();
          //Serial.println(packet);
          //packet = "";
          String g= incomingString.substring(0,1);
          GlobalIDReceived = g.toInt();
          String c= incomingString.substring(1,2);
          ClusterIDReceived = c.toInt();
          SyncCheck = incomingString.substring(2,3);
          ClusterHeadCheck = SyncCheck.toInt();
          String o =incomingString.substring(3,4);
          OverlapCheck = o.toInt();
          
        // Basestation Overlap Recieved (Case cluster out of order) - MIGHT GET CHANGED
          if(SyncCheck == "O" && (ClusterIDReceived == CLUSTER_ID || OverlapCheck == CLUSTER_ID)){
          //packet = packet + "WWWW" + millis();
          //Serial.println(packet);
          //packet = "";
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          }
          
          else if(SyncCheck == "T" && ClusterIDReceived == CLUSTER_ID){
          int Wait = ((NETWORK_NUMBER_OF_NODES+GLOBAL_ID-GlobalIDReceived)%NETWORK_NUMBER_OF_NODES)*TIME_SLOT;
          unsigned long WaitMath = (unsigned long) Wait;
          wait_time = CurrentTime+WaitMath;
          state = WAIT;
          }

          // Store Data
          else if(CLUSTER_FLAG == 1 && ClusterIDReceived == CLUSTER_ID ){
            // Might Remove Node IDs
            //packet = packet + "SSSS" + millis();
            //Serial.println(packet);
            //packet = "";
            if(packet == ""){
              unsigned long E = (unsigned long) ((NETWORK_NUMBER_OF_NODES-CLUSTER_NODES)*TIME_SLOT);
              unsigned long Dif = CurrentTime-LastTime-E;
              LastTime = CurrentTime;
              packet = packet+ "," + GlobalIDReceived + "," + Dif;
            }
            else{
              unsigned long Dif = CurrentTime-LastTime;
              LastTime = CurrentTime;
              packet = packet+ "," + GlobalIDReceived + "," + Dif;
            }
          }

          Serial.flush();
        }
        //packet = packet + "DDDD" + millis();
        //Serial.println(packet);
        //packet = "";
      }
      }
      break;

    case TRANSMIT:
      CurrentTime = millis();
      //Transmit
      String BulkPacket = HEADER+packet;
      if(CLUSTER_FLAG ==1){
        unsigned long Dif = CurrentTime-LastTime;
        LastTime = CurrentTime;
        BulkPacket = BulkPacket + "," + GLOBAL_ID + "," + Dif;
      }
      Serial.println(BulkPacket);
      packet = "";

      //Change LED state
      led_state = !led_state;
      digitalWrite(LED,led_state);

      // Check for suffecient energy
      if(energyAvailable(ENERGY_HAVEST_RATE)){
        int Wait = (NETWORK_NUMBER_OF_NODES*TIME_SLOT)-ERROR;
        unsigned long WaitMath = (unsigned long) Wait;
        wait_time = CurrentTime+WaitMath;
        Trans = false;
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
  Serial.setTimeout(10);
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  nodeFSM();
}
