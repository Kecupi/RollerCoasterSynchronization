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
    def test_WrongCount(self):
        """Fails with no arguments"""
        with self.assertRaises(subprocess.CalledProcessError) as context:
            subprocess.run(["./proc_main"], capture_output=True, text=True, check=True)
        self.assertEqual(context.exception.returncode, 1)
        msg_match = re.search(r"ERROR: Invalid number of arguments \(expected \d+, got \d+\)", context.exception.stderr)
        self.assertIsNotNone(msg_match)
    def tearDown(self):
        """Function destroying fixture"""
        subprocess.run(["make", "clean"], capture_output=True)


if __name__ == "__main__":
    unittest.main()