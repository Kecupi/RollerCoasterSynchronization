#!/usr/dev/bin python3
"""!
@file tests.py
@brief Tests for process synchronization project

The program runs makefile to generate the binary with certain parameters,
check if  arguments are right, if order of output lines is right and
cleans after finishing checking everything.

@author Stepan Horenek
@date 21. 8. 2026
"""
import os
import subprocess

os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/..") # change to project main directory

process = subprocess.run(["make"])
subprocess.run(["./proc_main", "2", "10", "4", "10", "10", "10"])
with open("log.out", "r") as f:
    for line in f:
        print(line)
subprocess.run(["make", "clean"])