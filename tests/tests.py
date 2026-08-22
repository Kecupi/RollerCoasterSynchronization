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
    """Test suite for testing programs behavior based on its arguments"""
    def setUp(self):
        """Function creating fixture"""
        subprocess.run(["make"], capture_output=True)

    def assertFailure(self, args, expected_msg):
        """Support function to run tests based on args"""
        with self.assertRaises(subprocess.CalledProcessError) as context:
            subprocess.run(["./proc_main"] + args, capture_output=True, text=True, check=True)
        self.assertEqual(context.exception.returncode, 1)
        msg_match = re.search(expected_msg, context.exception.stderr)
        self.assertIsNotNone(msg_match)

    def test_WrongArgCount(self):
        """Fails with no arguments"""
        regex = r"ERROR: Invalid number of arguments \(expected \d+, got \d+\)"
        self.assertFailure([], regex)
        self.assertFailure(["", "", "", "", ""], regex)
        self.assertFailure(["", "", "", "", "", "", ""], regex)

    def test_InvalidCharInArg(self):
        """Fails when one of the arguments containts char not convertable to int"""
        regex = r"ERROR: Invalid character inside argument during conversion"
        self.assertFailure(["1A", "1", "1", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4CC2", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "1", "1", "5D6"], regex)

    def test_InvalidArgType(self):
        """Fails when one of the arguments is not convertable to int"""
        regex = r"ERROR: Invalid type of argument, can't be converted to int"
        self.assertFailure(["A", "1", "1", "1", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "R2D2", "1", "1"], regex)
        self.assertFailure(["1", "1", "4", "1", "HKS", "1"], regex)

    def test_ArgOutOfRange(self):
        """Failes when one of the arguments is out of designated range"""
        regex = r"ERROR: .+ out of range \(expected \d+ <= N <= \d+, got -?\d+\)"
        self.assertFailure(["1", "1", "4", "1", "1", "-1"], regex) # negative value
        self.assertFailure(["10000", "1", "4", "1", "1", "1"], regex) # value too big
        self.assertFailure(["1", "0", "4", "1", "1", "1"], regex) # value lower than one of bounds

    def tearDown(self):
        """Function destroying fixture"""
        subprocess.run(["make", "clean"], capture_output=True)


if __name__ == "__main__":
    unittest.main()