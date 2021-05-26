"""
Copyright (C) 2021 by
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

# testsuite runner module for BNGlib API tests
import os
import sys
import shutil
import utils
import platform

THIS_DIR = os.path.dirname(os.path.realpath(__file__))


def collect_test_names():
    
    res = []
    print("Collecting tests in " + THIS_DIR)
    files = os.listdir(THIS_DIR)
    for name in files:
        name_w_dir = os.path.join(THIS_DIR, name)
        if os.path.isdir(name_w_dir) and name != 'shared' and name != '__pycache__':
            res.append(name)
    
    res.sort()
    return res

def run_single_test(build_dir, work_api_dir, name):
    
    test_path = os.path.join(build_dir, 'test', 'api', name, name + utils.EXE_EXT)
    if not os.path.exists(test_path):
        sys.exit("Fatal error: API test executable '" + test_path + 
                 "' does not exist, libbng must be built with tests first.")
        
    cmd = [test_path]
    log_file = os.path.join(work_api_dir, os.path.basename(name) + '.log')
    ec = utils.run(cmd, cwd=work_api_dir, fout_name=log_file, verbose=False)
    
    if (ec != 0):
        print("!FAIL " + test_path + ", log: " + log_file)
        return False
    else:
        print(" PASS " + test_path)
        return True
    

def run_tests(build_dir, work_dir):
    if not os.path.exists(work_dir):
        os.mkdir(work_dir)
        
    work_api_dir = os.path.join(work_dir, 'api') 
    if os.path.exists(work_api_dir):
        shutil.rmtree(work_api_dir)
    os.mkdir(work_api_dir)
    
    # run tests, expects that tests were built
    test_names = collect_test_names()
    
    num_tests = 0
    num_tests_failed = 0    
    
    for name in test_names:
        ok = run_single_test(build_dir, work_api_dir, name)
        num_tests += 1
        if not ok:
            num_tests_failed += 1

    if num_tests_failed == 0:
        print("API TESTING PASSED: " + str(num_tests) + " passed")
        return 0
    else:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("API TESTING FAILED: " + str(num_tests_failed) + " failed out of " + str(num_tests))
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        return 1


