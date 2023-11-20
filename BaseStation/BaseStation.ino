/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

/* GLOBALS */
const int NETWORK_NUMBER_OF_NODES = 3; // Number of nodes on the network
const int CLUSTERS =  1; // Number of clusters on the network
const int TIME_SLOT = 500; // In milliseconds (ms) 10^-3
const int CLUSTERTHRESHOLD = 430; // In milliseconds threshold for cluster overlap
const int PACKETTHRESHOLD = 70; // In milliseconds threshold for packet overlap
const int TIME_OUT = 3; // Number of phases till timeout
const int CLUSTERHEADS [1] ={3}; // Array full of custer head IDS
unsigned long LastRecievedTime [CLUSTERS] = {0};
unsigned long OverlapError = 0;

/* Timers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;

/* Counters */
int OutOfEnergyCount = 0;
int FullArrayCount = 0;

/* Flags */
bool FullArray = false;
bool flag = true;

/* Fancy Custer Head Stuff */
String packet ="";
String SyncCheck = "";
int IdRecieved = 0;
int ClusterIDReceived= 0;
int ClusterHeadCheck = 0;



// Allows for a software reset, like the `RED` button, Easy one-liner
void(* softwareReset) (void) = 0; //declare reset function @ address 0

void basestationFSM() {
  static enum { START, ACTIVE} state = START;
  switch (state) {
    case START:
      OverlapError = (unsigned long) (TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES);
      delay(OverlapError);
      Serial.println("00S0");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      unsigned long var = millis()-OverlapError;
      if(LastRecievedTime[OutOfEnergyCount] < var && FullArray && false){
        LastRecievedTime[OutOfEnergyCount] = millis();
        int GlobalID = CLUSTERHEADS[OutOfEnergyCount];
        int ClusterID = OutOfEnergyCount + 1;
        packet = packet+GlobalID+ClusterID+"T0";
        Serial.println(packet);
        packet = "";
      }

      // Check for incoming packets 
      else{
        String incomingString ="";
        if(Serial.available() > 0) {  
          //Check to make sure it is a CH packet
          incomingString = Serial.readStringUntil('\r');
          String r = incomingString.substring(0,1);
          IdRecieved = r.toInt();
          String t = incomingString.substring(1,2);
          ClusterIDReceived = t.toInt();
          String q = incomingString.substring(2,3);
          ClusterHeadCheck = q.toInt();
          incomingString = incomingString.substring(6);

          if(ClusterHeadCheck == 1 && IdRecieved!=0 ){
            // Check for Cluster Head overlap 
            PreviousTime = CurrentTime;
            CurrentTime = millis();
            unsigned long TimeDif = (CurrentTime-PreviousTime);
            int u = (TIME_SLOT - CLUSTERTHRESHOLD);
            unsigned long Overlap = (unsigned long) u;

            if(TimeDif <= Overlap){
              packet = packet+IdRecieved+ClusterIDReceived+"OC";
              Serial.println(packet);
              packet = "";
            }

            else{            
              //Fill Time Array (Used for timeout check) 
              for(int index = 0; index < CLUSTERS; index++){
                if(IdRecieved == CLUSTERHEADS[index]){
                  LastRecievedTime[index] = millis();
                  FullArrayCount++;
                }
              }
              
              //Check for full array
              if(FullArrayCount == TIME_OUT* NETWORK_NUMBER_OF_NODES && !FullArray){
                FullArray = true;
              }

              // Seperating the packet and store times
              int StringCount = 0;
              int PacketTimes [NETWORK_NUMBER_OF_NODES] = {0};
              int TimeIndex = 0;

              while(incomingString.length() > 0){
                int CommaLocation = incomingString.indexOf(',');
                // Last message
                if(CommaLocation == -1){
                  PacketTimes[TimeIndex] = incomingString.toInt();
                  break;
                }
                // Time messages
                else{
                  if(StringCount % 2 == 1){
                    String Holder = incomingString.substring(0,CommaLocation);
                    PacketTimes[TimeIndex] = Holder.toInt();
                    TimeIndex++;   
                  }
                }
                // Update Counter
                incomingString = incomingString.substring(CommaLocation+1); 
                StringCount++;
              }

              // Check Packet for erros
              if(PacketTimes[1] != 0){
              unsigned long CurrentMessageTime = (unsigned long) TIME_SLOT;
              for(int MessageTimeCheck = 0; MessageTimeCheck <= TimeIndex; MessageTimeCheck++){
                  int MessageTime = PacketTimes[MessageTimeCheck];
                  if(MessageTime >= TIME_SLOT*NETWORK_NUMBER_OF_NODES){
                    MessageTime = TIME_SLOT;
                  }
                  CurrentMessageTime = (unsigned long)MessageTime;
                  unsigned long Error = (unsigned long)(TIME_SLOT +(TIME_SLOT - PACKETTHRESHOLD));

                  if(CurrentMessageTime >= Error || CurrentMessageTime <= PACKETTHRESHOLD){
                    packet = packet + IdRecieved+ClusterIDReceived+"OP";
                    Serial.println(packet);
                    packet = "";
                    break;
                  }
              } 
            }  
            }
          }  
      } 
      Serial.flush();
      OutOfEnergyCount = (OutOfEnergyCount+1)%CLUSTERS;
      break;

    default:
      state = START;
      break;
  }
  }
}

void setup() {
  Serial.begin(9600); // Baud 9600
  Serial.setTimeout(50);
  pinMode(LED, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
  basestationFSM();
}
