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
sys.path.append(os.path.join(THIS_DIR, 'api'))

WORK_DIR = os.path.join(THIS_DIR, 'work')

RELEASE_DIR = 'release'
BUILD_DIR = os.path.join(THIS_DIR, '..', 'build', RELEASE_DIR)

import parser_test
import api_test

if __name__ == '__main__':
    
    if len(sys.argv) == 2 and sys.argv[1] == '--help':
        print("Arguments: BUILD_DIR [EXTRA_PARSER_TEST_ARGS]")
        sys.exit(0)
        
    if len(sys.argv) >= 2:
        build_dir = os.path.abspath(sys.argv[1])
    else:     
        build_dir = BUILD_DIR

    if len(sys.argv) >= 3:
        extra_parser_test_args = [sys.argv[2:]]
    else: 
        extra_parser_test_args = [''] 
        
    print("Using BNGlib build in " + build_dir)
    
    # parser tests  
    parser_test_ec = parser_test.run_tests(build_dir, WORK_DIR, extra_parser_test_args)
    
    # api tests
    api_test_ec = api_test.run_tests(build_dir, WORK_DIR)
    
    print("*** Summary ***")
    print("parser tests: " + "PASSED" if parser_test_ec == 0 else "FAILED")
    print("api tests   : " + "PASSED" if api_test_ec == 0 else "FAILED")
    
    sys.exit(parser_test_ec + api_test_ec)
    