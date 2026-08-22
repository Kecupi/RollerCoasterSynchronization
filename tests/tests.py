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
import re
import subprocess
import unittest
# text color macros

os.chdir(os.path.dirname(os.path.abspath(__file__)) + "/..") # change to project main directory

class ArgumentsTests(unittest.TestCase):
    """! Test suite for testing programs behavior based on its arguments
    Test suite contains tests for wrong count of arguments, invalid character in argument,
    invalid type of argument and argument out of range
    """
    def setUp(self):
        """!Function creating fixture
        Function runs Makefile to build program
        """
        subprocess.run(["make"], capture_output=True)

    def assertFailure(self, args, expected_msg):
        """!Support function to run tests based on args
        Function runs program with given arguments, controls return value and right error message being printed
        @param args arguments for program to be executed with
        @param expected_msg error message to be found on stderr after program execution
        """
        with self.assertRaises(subprocess.CalledProcessError) as context:
            subprocess.run(["./proc_main"] + args, capture_output=True, text=True, check=True)
        self.assertEqual(context.exception.returncode, 1)
        msg_match = re.search(expected_msg, context.exception.stderr)
        self.assertIsNotNone(msg_match)

    def test_WrongArgCount(self):
        """!Test for checking that program fails if given wrong amount of arguments"""
        regex = r"ERROR: Invalid number of arguments \(expected \d+, got \d+\)"
        self.assertFailure([], regex)
        self.assertFailure(["", "", "", "", ""], regex)
        self.assertFailure(["", "", "", "", "", "", ""], regex)

    def test_InvalidCharInArg(self):
        """!Test for checking program fails when one of the arguments containts char not convertible to int"""
        regex = r"ERROR: Invalid character inside argument during conversion"
        self.assertFailure(["1A", "1", "1", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4CC2", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "1", "1", "5D6"], regex)

    def test_InvalidArgType(self):
        """!Test for checking program fails when one of the arguments is not convertible to int"""
        regex = r"ERROR: Invalid type of argument, can't be converted to int"
        self.assertFailure(["A", "1", "1", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "R2D2", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "1", "HKS", "1"], regex)

    def test_ArgOutOfRange(self):
        """!Test for checking program fails when one of the arguments is out of designated range"""
        regex = r"ERROR: .+ out of range \(expected \d+ <= N <= \d+, got -?\d+\)"
        self.assertFailure(["1", "1", "4", "1", "1", "-1"], regex) # negative value
        self.assertFailure(["10000", "1", "4", "1", "1", "1"], regex) # value too big
        self.assertFailure(["1", "0", "4", "1", "1", "1"], regex) # value lower than one of bounds

    def tearDown(self):
        """!Function destroying fixture
        Function runs 'clean' method of Makefile
        """
        subprocess.run(["make", "clean"], capture_output=True)

class ProcessTests(unittest.TestCase):
    def setUp(self):
        """!Function creating fixture
        Function runs Makefile to build program
        """
        subprocess.run(["make"], capture_output=True)
    def assertProcMsgExists(self, args, expected_msg, expected_cnt=1):
        """!Support function to check whether process did certain action based on log file
        @param args arguments to run the program with
        @param expected_msg message to find in log files
        @param expected_cnt count of regex matches expected in log file
        """
        result = subprocess.run(["./proc_main"] + args, capture_output=True, text=True, check=True)
        self.assertEqual(result.returncode, 0)
        with open("log.out", "r") as log:
            msg_match = re.findall(expected_msg, log.read())
        self.assertEqual(len(msg_match), expected_cnt)
        
    def test_ProcessesCreated(self):
        """!Test for checking whether certain child process was initialized"""
        self.assertProcMsgExists(["2", "10", "4", "10", "10", "10"], r"\d+: D: started")
        self.assertProcMsgExists(["2", "10", "4", "10", "10", "10"], r"\d+: C \d+: started", 2)
        self.assertProcMsgExists(["2", "10", "4", "10", "10", "10"], r"\d+: V \d+: started", 10)
    def tearDown(self):
        """Function destroying fixture"""
        subprocess.run(["make", "clean"], capture_output=True)

if __name__ == "__main__":
    unittest.main()