#!/usr/bin/env python

"""
This is the python script that is responsible for spawning a simulation
"""

import sys
import os
import subprocess

from termcolors import *

# get GRAPHITE_HOME from environment variable, or use pwd
def get_graphite_home():
    graphite_home = os.environ.get('GRAPHITE_HOME')
    if graphite_home == None:
        cwd = os.getcwd()
        warning_msg = "GRAPHITE_HOME undefined. Setting GRAPHITE_HOME to %s" % (cwd)
        print "\n%s" % (pWARNING(warning_msg))
        return cwd

    return graphite_home

# get PIN_HOME from Makefile.config
def get_pin_home(graphite_home):
    p = subprocess.Popen(("grep" , "-w", "PIN_HOME", graphite_home + "/Makefile.config"), stdout = subprocess.PIPE)
    lbl = p.communicate()[0]
    lpath = lbl.split('=')[1]
    return lpath.strip();

# spawn_job:
#  start up a command on one machine
#  can be called by spawn_master.py or spawn.py

def spawn_job(proc_num, command, graphite_home):
    # Set LD_LIBRARY_PATH using PIN_HOME from Makefile.config
    os.environ['LD_LIBRARY_PATH'] =  "%s/intel64/runtime" % get_pin_home(graphite_home)
    os.environ['CARBON_PROCESS_INDEX'] = "%d" % (proc_num)
    proc = subprocess.Popen(command, shell=True, preexec_fn=os.setsid, env=os.environ)
    return proc
