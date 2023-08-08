/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

/* GLOBALS */
const int NETWORK_NUMBER_OF_NODES = 2; // Number of nodes on the network
const int CLUSTERS = 2; // Number of clusters on the network
const int TIME_SLOT = 1000; // In milliseconds (ms) 10^-3
const int THRESHOLD = 100; // In milliseconds threshold for overlap
const int TIME_OUT = 3; // Number of phases till timeout
const int CLUSTERHEADS [2] ={1,2}; // Array full of custer head IDS
unsigned long LastRecievedTime [CLUSTERS] = {0};
unsigned long OverlapError = 0;

/* Timers / Trackers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;
int LastId = 0;

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

// Function that checks for overlap in packets
//bool clusterTransmissionError (unsigned long Current, unsigned long Previous) {
  //unsigned long TimeDif = (Current - Previous);
  //int u = (TIME_SLOT + THRESHOLD);
  //unsigned long Overlap = (unsigned long) u;
  //if(TimeDif <= Overlap){
    //Serial.println("True");
    //return true;    
  //}
  //else{
    //return false;    
  //}
//}

void basestationFSM() {
  //static enum { START, ACTIVE, RESTART } state = START;
  static enum { START, ACTIVE} state = START;
  switch (state) {
    case START:
      OverlapError = (unsigned long) (TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES*CLUSTERS);
      delay(OverlapError);
      Serial.println("00S0");
      state = ACTIVE;
      break;

    case ACTIVE:
      // Check for timeout
      unsigned long var = millis()-OverlapError;
      if(LastRecievedTime[OutOfEnergyCount] < var && FullArray){
        //packet = packet + "gggg" + LastRecievedTime[OutOfEnergyCount] + "p" + (millis()-OverlapError);
        //Serial.println(packet);
        //packet = "";
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
          //packet = packet + "zzzz" + incomingString;
          //Serial.println(packet);
          //packet = "";
          String r = incomingString.substring(0,1);
          IdRecieved = r.toInt();
          String t = incomingString.substring(1,2);
          ClusterIDReceived = t.toInt();
          String q = incomingString.substring(2,3);
          ClusterHeadCheck = q.toInt();
          //packet = packet + "mmmm" + ClusterHeadCheck;
          //Serial.println(packet);
          //packet = "";
          incomingString = incomingString.substring(6);

          if(ClusterHeadCheck == 1){
            // Check for Cluster Head overlap 
            //packet = packet + "ch";
            //Serial.println(packet);
            //packet = "";
            PreviousTime = CurrentTime;
            CurrentTime = millis();
            unsigned long TimeDif = (CurrentTime-PreviousTime);
            //packet = packet + "dddd" + TimeDif;
            //Serial.println(packet);
            //packet = "";
            int u = (TIME_SLOT - THRESHOLD);
            unsigned long Overlap = (unsigned long) u;

            if(TimeDif <= Overlap){
              unsigned long tes = Overlap-TimeDif;
              //packet = packet + "ifff" + tes;
              //Serial.println(packet);
              //packet = "";

              for(int index = 0; index < CLUSTERS; index++){
                // Send Overlap Message
                if(IdRecieved == CLUSTERHEADS[index] && IdRecieved != LastId){
                  //packet = packet + "ente";
                  //Serial.println(packet);
                  //packet = "";
                  LastId = IdRecieved;
                  int PreviousIndex = (CLUSTERS + (index-1))%CLUSTERS;
                  int PreviousCluster = CLUSTERHEADS[PreviousIndex];
                  packet = packet+IdRecieved+ClusterIDReceived+"O"+PreviousCluster;
                  Serial.println(packet);
                  packet = "";

                }
              }
            }

            else{        
              //packet = packet + "eeee";
              //Serial.println(packet);
              //packet = "";               
              //Fill Time Array (Used for timeout check) 
              for(int index = 0; index < CLUSTERS; index++){
                if(IdRecieved == CLUSTERHEADS[index]){
                  LastRecievedTime[index] = millis();
                  FullArrayCount++;
                  //packet = packet + "pppp" + LastRecievedTime[index] + "p" + CLUSTERHEADS[index];
                  //Serial.println(packet);
                  //packet = "";
                }
              }
              
              LastId = IdRecieved;

              //Check for full array
              if(FullArrayCount == TIME_OUT* NETWORK_NUMBER_OF_NODES && !FullArray){
                FullArray = true;
              }

              /*
              // Seperating the packet and store times
              int StringCount = 0;
              int PacketTimes [NETWORK_NUMBER_OF_NODES] = {};
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
              unsigned long LastMessageTime = 0;
              unsigned long CurrentMessageTime = 0;
              for(int MessageTimeCheck = 0; MessageTimeCheck < TimeIndex; MessageTimeCheck++){
                  LastMessageTime = CurrentMessageTime;
                  int MessageTime = PacketTimes[MessageTimeCheck];
                  CurrentMessageTime = (unsigned long)MessageTime;
                  unsigned long TimeDifference = CurrentMessageTime - LastMessageTime;
                  unsigned long Error = (unsigned long)(TIME_SLOT - THRESHOLD);
                  if(TimeDifference <= Error){
                    packet = packet + IdRecieved+ClusterIDReceived+"O0";
                    Serial.println(packet);
                    packet = "";
                    break;
                  }
              } */
            }  
          }  
      } 
      Serial.flush();
      OutOfEnergyCount = (OutOfEnergyCount+1)%CLUSTERS;
      break;

    //case RESTART:
      // Send hault message to network
      //Serial.println("00H");
      // Arbitrary wait to make sure all nodes are in sync state
      //delay(TIME_SLOT);
      //state = START;
      //break;

    //default:
      //state = START;
      //break;
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
