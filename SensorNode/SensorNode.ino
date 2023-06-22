/*
Multi-Hop TDMA SENSOR NODE (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/03/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

// FSM for a sensor node or a cluster head.

/* GLOBALS */
const int GLOBAL_ID = 1;  // Node Global ID on the network.
const int CLUSTER_ID = 1; // ID corresponding to cluster that node belongs to.
const int TIME_SLOT = 1000; // In milliseconds (ms) 10^-3
const int ENERGY_HAVEST_RATE = 100; // Rate at each the energy is harvested
const int NETWORK_NUMBER_OF_NODES = 7; // Number of nodes on the network
const bool CLUSTER_FLAG = false;  // Whether or not the node serves as a cluster head

/* FLAGS... and stuff*/
bool sync_received = false;
bool led_state = false;

/* Fancy Custer Head Stuff */
// Something to store packet to be prepared, arr or a struct I think...doing arr for now
// Cluster Number, Node 1, Recieve Time 1, ... Node N-1, Recive Time N-1, Cluster Head, Send Time 
enum {N = 5}; // Number of sensor nodes in a cluster, includes cluster head
byte packet[1+(2*N)] = {}; // Not sure you can do this

// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

// Function to see if energy is still avaible
bool energyAvailable(int harvestRate) {
  if(random(0,100 < harvestRate)){
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
      while(!sync_received){
        
      }
      break;

    case WAIT:
      // TODO: Wait for time slot, wait for resync, wait for stuff...idk just wait

      break;

    case TRANSMIT:
      //Transmit Data
      if(!CLUSTER_FLAG){
        Serial.println(GLOBAL_ID);
        led_state = !led_state;
        digitalWrite(LED,led_state);
      }
      else{
        for(int i=0; i<N; i++){
          Serial.write(packet[i]);
          Serial.write(',');
          packet[i] = 0;
        }
        Serial.println();
        led_state = !led_state;
        digitalWrite(LED,led_state);
      }

      // Check for suffecient energy
      if(energyAvailable(ENERGY_HAVEST_RATE)){
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
