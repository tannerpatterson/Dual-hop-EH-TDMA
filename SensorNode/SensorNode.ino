/*
Multi-Hop TDMA SENSOR NODE (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/03/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

// FSM for a sensor node or a cluster head.


/* GLOBALS */
const int GLOBAL_ID = 1;  // Node Global ID on the network.
const int CLUSTER_ID = 1; // ID corresponding to cluster that node belongs to.
const bool CLUSTER_FLAG = false;  // Whether or not the node serves as a cluster head
const int TIME_SLOT = 1000; // In milliseconds (ms) 10^-3

bool SUFF_ENERGY = false; // Refer to HARVEST state

/* FLAGS... and stuff*/


/* Fancy Custer Head Stuff */
// Something to store packet to be prepared, arr or a struct I think...doing arr for now
enum {N = 5} // Number of sensor nodes in a cluster, includes cluster head
byte packet[N] = {}; // Not sure you can do this

// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

// State Machine
void nodeFSM() {
  static enum { HARVEST, SYNC, WAIT, TRANSMIT } state = SYNC;

  switch (state) {
    case HARVEST:
      // TODO: Keep rolling the die until energy is harvested
      break;
    case SYNC:
      // TODO: Synchronize with a cluster head, base station or with another node in the cluster.
      break;
    case WAIT:
      // TODO: Wait for time slot, wait for resync, wait for stuff...idk just wait
      break;
    case TRANSMIT:
      // TODO: Transmit...idk...that is all all... 
      // this is its own state to separate it from rest of program, for convenience sake
      break;
    default:
      state = HARVEST;
      break;
  }
}

void setup() {
  Serial.begin(9600); // Baud 9600
}

void loop() {
  nodeFSM();
}
