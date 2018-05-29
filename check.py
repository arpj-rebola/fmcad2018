#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil
import subprocess

#------------------------
# General purpose
#------------------------

def readList(file):
	o = open(file)
	lines = o.read().splitlines()
	o.close()
	lines = filter(lambda line : line[0] != "#", lines)
	return lines

def removeFile(path):
	if os.path.exists(path):
		os.remove(path)

def display(message):
	print "\n@@@ " + message + "\n"

#------------------------
# Directory structure and constants
#------------------------

memoryLimit = 8192
timeLimit = 3000

solversPath = "solvers/"
drattrimPath = "drat-trim/drat-trim"
rupeePath = "rupee/bin/rupee"
solverListPath = "info/solver-list.txt"
runlimPath = "runlim/runlim"
benchmarksPath = "benchmarks/"
outputPath = "output/"
proofsPath = "proofs/"

#------------------------
# Path generators
#------------------------

def cnfPath(instance):
	return benchmarksPath + instance + ".cnf"

def dratPath(solver, instance):
	return proofsPath + instance + "." + solver + ".drat"

def drattrimOutputPath(solver, instance):
	return outputPath + instance + "." + solver + ".DT.out"

def rupeesdOutputPath(solver, instance):
	return outputPath + instance + "." + solver + ".SD.out"

def rupeefdOutputPath(solver, instance):
	return outputPath + instance + "." + solver + ".FD.out"

def drattrimRunlimPath(solver, instance):
	return outputPath + instance + "." + solver  + ".DT.run"

def rupeesdRunlimPath(solver, instance):
	return outputPath + instance + "." + solver + ".SD.run"

def rupeefdRunlimPath(solver, instance):
	return outputPath + instance + "." + solver + ".FD.run"

#------------------------
# Command line generators
#------------------------

def commandDrattrim(solver, instance):
	return drattrimPath + " " + cnfPath(instance) + " " + dratPath(solver, instance)

def commandRupeesd(solver, instance):
	return rupeePath + " " + cnfPath(instance) + " " + dratPath(solver, instance) + " -skip-deletion -stats -binary"

def commandRupeefd(solver, instance):
	return rupeePath + " " + cnfPath(instance) + " " + dratPath(solver, instance) + " -full-deletion -stats -binary"

def limitCommand(command, out, run):
	return runlimPath + " -o " + run + " -s " + str(memoryLimit) + " -r " + str(timeLimit) + " " + command + " > " + out

#------------------------
# Execution
#------------------------

solver = sys.argv[1]
instance = sys.argv[2]
if os.path.exists(dratPath(solver, instance)):
	display("Running DRAT-trim over CNF formula " + cnfPath(instance) + " and DRAT proof " + dratPath(solver, instance))
	subprocess.call(limitCommand(commandDrattrim(solver, instance), drattrimOutputPath(solver, instance), drattrimRunlimPath(solver, instance)), shell=True)
	display("Running rupee with skip-deletion over CNF formula " + cnfPath(instance) + " and DRAT proof " + dratPath(solver, instance))
	subprocess.call(limitCommand(commandRupeesd(solver, instance), rupeesdOutputPath(solver, instance), rupeesdRunlimPath(solver, instance)), shell=True)
	display("Running rupee with full-deletion over CNF formula " + cnfPath(instance) + " and DRAT proof " + dratPath(solver, instance))
	subprocess.call(limitCommand(commandRupeefd(solver, instance), rupeefdOutputPath(solver, instance), rupeefdRunlimPath(solver, instance)), shell=True)