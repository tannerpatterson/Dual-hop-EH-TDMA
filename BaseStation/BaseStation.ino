/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/


void basestationFSM() {
  static enum { SYNC, ACTIVE, RESYNC } state = SYNC;

  switch (state) {
    case SYNC:
      Serial.println(BulkPacket);
      break;

    case ACTIVE:

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
