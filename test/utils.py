"""
Copyright (C) 2019-2021 by
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

"""
This module contains diverse utility functions.
"""

import os
import sys
import subprocess
import shutil
import platform
import multiprocessing
from threading import Timer
from subprocess import Popen, PIPE
          

def get_cwd_no_link():
    # get current working directory even though we are in a symlinked directory
    # the shell argument must be set to True in this case
    if 'Windows' in platform.system():
        res = os.getcwd()
    else:
        cwd = Popen(['pwd'], stdout=PIPE, shell=True).communicate()[0].strip()
        res = cwd.decode('ascii')
     
    return res
          

def kill_proc(proc, f, timeout_is_fatal):
    proc.kill()
    f.write("Terminated after timeout")
    if timeout_is_fatal:
        sys.exit(1)

    
def print_file(file_name):
    with open(file_name, "r") as fin:
        for line in fin:
            print(line)


def run_with_ascii_output(cmd, cwd):
    # does not return exit code, neither provides timeout
    return Popen(cmd, cwd=cwd, stdout=PIPE).communicate()[0].strip().decode('ascii')
    


def execute(cmd, cwd, timeout_sec, timeout_is_fatal, outfile, shell=False):
    if shell:
        # for shell=True, the command must be a single string
        cmd = str.join(" ", cmd)
    
    proc = Popen(cmd, shell=shell, cwd=cwd, stdout=outfile, stderr=subprocess.STDOUT)
    timer = Timer(timeout_sec, kill_proc, [proc, outfile, timeout_is_fatal])
    try:
        timer.start()
        exit_code = proc.wait()
    finally:
        timer.cancel()
        
    return exit_code


# can be simplified by using subprocess.run from Python 3.5
def run(
        cmd, 
        cwd=os.getcwd(),
        fout_name="", 
        append_path_to_output=False, 
        print_redirected_output=False, 
        timeout_sec=300,
        timeout_is_fatal = True, 
        verbose=True,
        shell=False 
        ):
    if verbose:
        log("    Executing: '" + str.join(" ", cmd) + "' " + str(cmd) + " in '" + cwd + "'")

    if fout_name:
        if append_path_to_output:
            full_fout_path = os.path.join(cwd, fout_name)
        else:
            full_fout_path = fout_name
    
        with open(full_fout_path, "w") as f:
            f.write("cwd: " + cwd + "\n")
            f.write(str.join(" ", cmd) + "\n")  # first item is the command being executed
            
            # run the actual command
            exit_code = execute(cmd, cwd, timeout_sec, timeout_is_fatal, f, shell=shell)

        if (print_redirected_output):
            print_file(full_fout_path)
            
    else:
        exit_code = execute(cmd, cwd, timeout_sec, timeout_is_fatal, sys.stdout, shell=shell)

    if verbose:
        log("Exit code: " + str(exit_code))
    return exit_code


def log(msg):
    print("* " + msg)
    sys.stdout.flush()


def warning(msg):
    print("* Warning: " + msg)
    sys.stdout.flush()


def fatal_error(msg):
    print("* Error: " + msg)
    sys.stdout.flush()
    sys.exit(1)    
   

def check_ec(ec, cmd):
    if ec != 0:
        cmd_str = str.join(" ", cmd)
        fatal_error("Error: command '" + cmd_str + "' failed, terminating.")
        
        
def get_nr_cores():
    cpu_count = multiprocessing.cpu_count()
    if cpu_count > 1:
        return int(cpu_count/2)
    else:
        return 1
