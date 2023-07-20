/*
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 07/06/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
*/

#define LED 8

/* GLOBALS */
const int NETWORK_NUMBER_OF_NODES = 3; // Number of nodes on the network
const int CLUSTERS = 2; // Number of clusters on the network
const int TIME_SLOT = 2000; // In milliseconds (ms) 10^-3
const int THRESHOLD = 1000; // In milliseconds threshold for overlap
const int TIME_OUT = 3; // Number of phases till timeout
const int CLUSTERHEADS [CLUSTERS] ={2,3}; // Array full of custer head IDS
unsigned long LastRecievedTime [CLUSTERS] = {0};

/* Timers */
unsigned long CurrentTime = 0;
unsigned long PreviousTime = 0;

/* Counters */
int OutOfEnergyCount = 0;
int FullArrayCount = 0;

/* Flags */
bool FullArray = false;
/* Fancy Custer Head Stuff */
String packet ="";
String incomingString ="";
String SyncCheck = "";
int IdRecieved = 0;
int ClusterIDReceived= 0;
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
      if(LastRecievedTime[OutOfEnergyCount] <= millis()-(TIME_OUT* TIME_SLOT* NETWORK_NUMBER_OF_NODES) && FullArray){
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
          ClusterIDReceived = incomingString.substring(1,2).toInt();
          ClusterHeadCheck = incomingString.substring(2,3).toInt();
          incomingString = incomingString.substring(6);

          if(ClusterHeadCheck == 1){
          
            // Check for Cluster Head overlap (Might get deleted)
            PreviousTime = CurrentTime;
            CurrentTime = millis();
            if(clusterTransmissionError(CurrentTime,PreviousTime)){
              for(int index = 0; index < NETWORK_NUMBER_OF_NODES; index++){
                // Send Overlap Message
                if(IdRecieved == CLUSTERHEADS[index]){
                  int PreviousCluster = ((index-1)%CLUSTERS)+1;
                  String packet = IdRecieved+ClusterIDReceived+"O"+PreviousCluster;
                  Serial.println(packet);
                }
              }
            }

            else{    

                   
              //Fill Time Array (Used for timeout check) 
              int FullArrayCount = 0;
              for(int index = 0; index < CLUSTERS; index++){
                if(IdRecieved == CLUSTERHEADS[index]){
                  LastRecievedTime[index] = CurrentTime;
                }

                // Used for checking for a full array
                if(LastRecievedTime[index] != 0){
                  FullArrayCount++;
                }
              }

              //Check for full array
              if(FullArrayCount == CLUSTERS){
                FullArray = true;
              }


              // Seperating the packet and store times
              int StringCount = 0;
              String PacketTimes [NETWORK_NUMBER_OF_NODES];
              int TimeIndex = 0;

              while(incomingString.length() > 0){
                int CommaLocation = incomingString.indexOf(',');
                // Last message
                if(CommaLocation == -1){
                  PacketTimes[TimeIndex] = incomingString;
                }
                // Time messages
                else{
                  if(StringCount % 2 == 1){
                    PacketTimes[TimeIndex] = incomingString.substring(0,CommaLocation);
                    TimeIndex++;   
                  }
                }
                // Update Counter
                incomingString = incomingString.substring(CommaLocation+1); 
                StringCount++;
              }

              // Check Packet for erros
              int LastMessageTime = 0;
              int CurrentMessageTime = 0;
              for(int MessageTimeCheck = 0; MessageTimeCheck < TimeIndex; MessageTimeCheck++){
                  LastMessageTime = CurrentMessageTime;
                  CurrentMessageTime = PacketTimes[MessageTimeCheck].toInt();
                  int TimeDifference = CurrentMessageTime - LastMessageTime;
                  if(TimeDifference <= TIME_SLOT - THRESHOLD){
                    Serial.println(IdRecieved+ClusterIDReceived+"O0");
                    break;
                  }
              }
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
