#!/usr/bin/env python
import sys;
sys.path.insert(0, "..")
import settings
import subprocess
import os
import argparse

parser = argparse.ArgumentParser(description='imalse-doc')
parser.add_argument('-b', '--build', default='html',
        help="""
	Please use \`make <target>' where <target> is one of
	| html       to make standalone HTML files
	| dirhtml    to make HTML files named index.html in directories
	| singlehtml to make a single large HTML file
	| pickle     to make pickle files"
	| json       to make JSON files"
	| htmlhelp   to make HTML files and a HTML help project
	| qthelp     to make HTML files and a qthelp project
	| devhelp    to make HTML files and a Devhelp project
	| epub       to make an epub
	| latex      to make LaTeX files, you can set PAPER=a4 or PAPER=letter
	| latexpdf   to make LaTeX files and run them through pdflatex
	| text       to make text files
	| man        to make manual pages
	| changes    to make an overview of all changed/added/deprecated items
	| linkcheck  to check all external links for integrity
	| doctest    to run all doctests embedded in the documentation (if enabled)
    """
        )

args, exper_args = parser.parse_known_args()

CWD = os.path.abspath(os.getcwd())
os.chdir(settings.NS3_PATH)
# subprocess.check_call('./waf --pyrun "%s/sphinx-build -b html %s/source %s/build/html"'%(CWD, CWD, CWD), shell=True)
subprocess.check_call('./waf --pyrun "%s/sphinx-build -b %s %s/source %s/build/%s"'%(CWD, args.build, CWD, CWD, args.build), shell=True)
