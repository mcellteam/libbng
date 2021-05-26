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

# testsuite runner for BNGL parser

import os
import sys
import shutil
import utils
import platform

THIS_DIR = os.path.dirname(os.path.realpath(__file__))
BNGL_EXT = '.bngl'

BNG_DIR = 'bng'
DEFAULT_TEST_APP = 'parser_tester_libbng'

    
# returns a list of TestInfo objects
def get_test_files(dir):
    res = []
    test_set_full_path = os.path.join(THIS_DIR, dir)
    print("Collecting tests in " + test_set_full_path)
    files = os.listdir(test_set_full_path)
    for name in files:
        name_w_dir = os.path.join(test_set_full_path, name)
        if os.path.isfile(name_w_dir) and os.path.splitext(name_w_dir)[1] == BNGL_EXT:
            res.append(name_w_dir)
    return res
   

# returns true if test passed
def run_single_test(work_parser_dir, test_app, test_file, extra_args):

    expected_ec = 0
    expected_outputs = []
    with open(test_file, 'r') as f:
        line = f.readline()
        
        if '# FAIL' in line:
            expected_ec = 1 # exit code 1
        elif '# OK' in line:
            expected_ec = 0
        else:
            utils.fatal_error(test_file + ": First line must be either '# FAIL' or '# OK'")

        line = f.readline()
        while line.startswith('# OUTPUT:'):
            expected_outputs.append(line[len('# OUTPUT:'):].strip())    
            line = f.readline()
            
    cmd = [test_app, test_file]
    cmd += extra_args
    log_file = os.path.join(work_parser_dir, os.path.basename(test_file) + '.log')
    ec = utils.run(cmd, cwd=work_parser_dir, fout_name=log_file, verbose=False)
    
    if (ec != expected_ec):
        print("!FAIL " + test_file + ": exit code was " + str(ec) + ", expected " + str(expected_ec) + 
              ", log: " + log_file)
        return False
    
    with open(log_file, 'r') as f:
        log_content = f.read()
    
    for output in expected_outputs:
        if output not in log_content:
            print("!FAIL " + test_file + ": did not find  '" + output + "' in " + log_file)
            return False
    
    print(" PASS " + test_file)
    return True
   
   
def run_tests(build_dir, work_dir, extra_args):
    if not os.path.exists(work_dir):
        os.mkdir(work_dir)
        
    work_parser_dir = os.path.join(work_dir, 'parser') 
    if os.path.exists(work_parser_dir):
        shutil.rmtree(work_parser_dir)
    os.mkdir(work_parser_dir)
    
    test_app = os.path.join(build_dir, BNG_DIR, DEFAULT_TEST_APP + utils.EXE_EXT)
    
    if not os.path.exists(test_app):
        sys.exit("Fatal error: parser test application '" + test_app + "' was not found.")
    
    num_tests = 0
    num_tests_failed = 0        
    
    tests = get_test_files('negative')
    tests += get_test_files('positive')
    tests.sort()
    for t in tests:
        ok = run_single_test(work_parser_dir, test_app, t, extra_args[0])
        num_tests += 1
        if not ok:
            num_tests_failed += 1
    
    
    if num_tests_failed == 0:
        print("PARSER TESTING PASSED: " + str(num_tests) + " passed")
        return 0
    else:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("PARSER TESTING FAILED: " + str(num_tests_failed) + " failed out of " + str(num_tests))
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        return 1

    