#!/usr/bin/env python
import sys;
sys.path.insert(0, "..")
import settings
import subprocess
import os

CWD = os.path.abspath(os.getcwd())
os.chdir(settings.NS3_PATH)
subprocess.check_call('./waf --pyrun "%s/sphinx-build -b html %s/source %s/build/html"'%(CWD, CWD, CWD), shell=True)
