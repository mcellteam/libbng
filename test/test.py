#!/usr/bin/env python3

"""
Copyright (C) 2020-2021 by
The Salk Institute for Biological Studies

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

# this is a runner for BNG tests

import os
import sys

THIS_DIR = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(THIS_DIR, 'parser'))

import parser_test

if __name__ == '__main__':
    
    if len(sys.argv) >= 2:
        test_app = os.path.abspath(sys.argv[1])
    else:     
        test_app = parser_test.DEFAULT_TEST_APP

    if len(sys.argv) >= 3:
        extra_args = [sys.argv[2]]
    else: 
        extra_args = [] 
        
    print("Using test application " + test_app)
    
    # parser tests  
    parser_test_ec = parser_test.run_tests(test_app, extra_args)
    
    print("*** Summary ***")
    print("parser tests: " + "PASSED" if parser_test_ec == 0 else "FAILED")
    
    sys.exit(parser_test_ec)
    