"""
Multi-Hop TDMA BASE STATION (Energy Harvesting Simulation) - T.J Smull College of Engineering, Ohio Northern University
Author: Tanner Patterson, Robert J. Hayek, Dr. Ahmed Ammar
Date: 06/14/2023
Copyright (c) 2023, Ohio Northern University, All rights reserved.

Base Station FSM Implemented in Python to allow for data saving. Should be able to perform all base station duties and
export data for analysis.
"""
from datetime import datetime
import serial

if __name__ == "__main__":
    count = 0
    output_file = input("Enter output file name: ")
    port = input("Enter serial port: ")
    xbee = serial.Serial(
        port=port,
        baudrate=9600,
    )
    f = open(f'{output_file}.txt', 'w')
    while True:
        try:
            line = xbee.readline()
            count = count + 1
            data = f'{line.decode().rstrip()}'
            f.write(f'{datetime.now()}, {count}, {data}\n')
            print(f'{datetime.now()}, {count}, {data}')
        except KeyboardInterrupt:
            f.close()
            break