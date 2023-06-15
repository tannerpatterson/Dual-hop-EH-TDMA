"""
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull
College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/14/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.
"""

# Base Station FSM Implemented in Python to allow for data saving.
# Should be able to perform all base station duties and export data for 
# analysis.

import serial

# Global Variables
time_slot = 1000
number_of_nodes = 7
timeout = (number_of_nodes*time_slot*3)
threshold = 0   # Placeholder till testing
time_array = [0] * number_of_nodes
time_diff_array = [0] * (number_of_nodes-1)
state = "SYNC"

# Flags... and stuff I suppose
last_message = ""
current_message = ""
previous_resync = 0
current_timestamp = 0
last_timestamp = 0
packet_count = 0
last_packet_count = 0
start_recv = 0
recieved_acks = 0
resync_sent_count = 0
packet_recieved = False