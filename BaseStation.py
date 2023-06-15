"""
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull
College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/14/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.

Base Station FSM Implemented in Python to allow for data saving. Should be able to perform all base station duties and
export data for analysis.

"""
from datetime import datetime
import serial

# Global Variables
time_slot = 1000
number_of_nodes = 7
timeout = (number_of_nodes * time_slot * 3)
threshold = 0  # Placeholder till testing
time_array = [0] * number_of_nodes
time_diff_array = [0] * (number_of_nodes - 1)

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


# Helper function to receive one packet, append to an output file, and return it to its caller.
def __receive_packet(port: str, output: str) -> str:
    xbee = serial.Serial(
        port=port,
        baudrate=9600,
    )
    f = open(f'{output}.txt', 'a')
    raw_data = f'{xbee.readline().decode().rstrip()}'
    compiled_data = f'{datetime.now()}, {raw_data}\n'  # time of received packet, packet data
    f.write(compiled_data)
    f.close()
    return compiled_data


def fsm():
    state = 'sync'
    print("Finite State Machine Method")
    match state:
        case 'sync':
            print('State 1')
        case 'recv':
            print("State 2")

        case 'resync':
            print("State 3")
        case _:
            state = 'sync'
            print('Default state')


if __name__ == '__main__':
    fsm()
