#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil
import subprocess
import stat
import csv

#------------------------
# General purpose
#------------------------

def readList(file):
	o = open(file)
	lines = o.read().splitlines()
	o.close()
	lines = filter(lambda line : line[0] != "#", lines)
	return lines

def makeExecutable(file):
	st = os.stat(file)
	os.chmod(file, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

def display(message):
	print "\n@@@ " + message + "\n"

def cleanup(path):
	if os.path.exists(path):
		shutil.rmtree(path)
	os.makedirs(path)

def remove(path):
	if os.path.exists(path):
		shutil.rmtree(path)

def githubGet(name, repository, path):
	subprocess.call("wget --no-check-certificate https://github.com/" + name + "/" + repository + "/archive/master.tar.gz -P " + path, shell=True)


#------------------------
# Directory structure
#------------------------

infoPath = "info/"
solversPath = "solvers/"
drattrimPath = "drat-trim/"
rupeePath = "rupee/"
outputPath = "output/"
proofsPath = "proofs/"
benchmarksPath = "benchmarks/"
runlimPath = "runlim/"
tempPath = "temp_/"
downloadPath = "temp_/download/"
unpackPath = "temp_/unpack/"
outputPath = "output/"
proofsPath = "proofs/"

#------------------------
# Solvers
#------------------------

def downloadSolver(solver):
	display("Downloading solver " + solver)
	cleanup(downloadPath)
	subprocess.call("wget https://baldur.iti.kit.edu/sat-competition-2017/solvers/main/" + solver + ".zip -P " + downloadPath, shell=True)

def unpackSolver(solver):
	display("Unpacking solver " + solver)
	cleanup(unpackPath)
	subprocess.call("unzip " + downloadPath + solver + ".zip -d " + unpackPath, shell=True)
	if solver == "Riss7":
		variants = ["Riss7_BVE", "Riss7_noPP"]
		unpackedpath = unpackPath
	elif solver == "satUZK-seq":
		variants = ["satUZK-seq_ge", "satUZK-seq_me", "satUZK-seq_sge", "satUZK-seq_sme"]
		unpackedpath = unpackPath
	elif solver == "CandyRSILi":
		variants = ["CandyRSILi"]
		unpackedpath = unpackPath + "Candy RSILi"
	elif solver == "CandyRSILv":
		variants = ["CandyRSILv"]
		unpackedpath = unpackPath + "Candy RSILv"
	elif solver == "CandySL21":
		variants = ["CandySL21"]
		unpackedpath = unpackPath + "Candy SL21"
	else:
		variants = [solver]
		unpackedpath = unpackPath + solver
	for variant in variants:
		remove(solversPath + variant)
		print "copying " + unpackedpath
		shutil.copytree(unpackedpath, solversPath + variant)

def buildSolver(solver):
	display("Building solver " + solver)
	if solver != "Riss7" and solver != "satUZK-seq":
		makeExecutable(solversPath + solver + "/starexec_build")
		if solver == "cadical-sc17-agile-proof" or solver == "cadical-sc17-proof" or solver == "lingeling-bbe":
			makeExecutable(solversPath + solver + "/build/build.sh")
		if solver == "CandyRSILi" or solver == "CandyRSILv":
			# this must be executed after Candy has been processed...
			shutil.copytree(solversPath + "Candy/lib/googletest", solversPath + solver + "/lib/googletest")
		subprocess.call("./starexec_build", cwd=solversPath + solver, shell=True)
	if solver == "Riss7":
		makeExecutable(solversPath + "Riss7_BVE/bin/riss")
		makeExecutable(solversPath + "Riss7_noPP/bin/riss")
	if solver == "bs_glucose":
		makeExecutable(solversPath + "bs_glucose/bin/bs_glucose1")
	if solver == "satUZK-seq":
		makeExecutable(solversPath + "satUZK-seq_ge/bin/satUZK-seq")
		makeExecutable(solversPath + "satUZK-seq_sge/bin/satUZK-seq")
		makeExecutable(solversPath + "satUZK-seq_me/bin/satUZK-seq")
		makeExecutable(solversPath + "satUZK-seq_sme/bin/satUZK-seq")
	if solver == "tch_glucose1":
		makeExecutable(solversPath + "tch_glucose1/bin/tch_glucose1")
	if solver == "tch_glucose2":
		makeExecutable(solversPath + "tch_glucose2/bin/tch_glucose2")
	if solver == "tch_glucose3":
		makeExecutable(solversPath + "tch_glucose3/bin/tch_glucose3")

def makeSolvers():
	cleanup(solversPath)
	solvers = readList("info/download-list.txt")
	for solver in solvers:
		downloadSolver(solver)
		unpackSolver(solver)
		buildSolver(solver)

#------------------------
# Checkers
#------------------------

def downloadChecker():
	display("Downloading drat-trim ")
	cleanup(downloadPath)
	githubGet("marijnheule", "drat-trim", downloadPath)

def unpackChecker():
	display("Unpacking drat-trim")
	cleanup(unpackPath)
	subprocess.call("tar -xf " + downloadPath + "/master.tar.gz -C " + unpackPath, shell=True)
	shutil.copytree(unpackPath + "drat-trim-master/", drattrimPath)

def buildChecker():#
	display("Building drat-trim")
	subprocess.call("make", cwd = drattrimPath, shell = True)
	makeExecutable(drattrimPath + "/drat-trim")
	display("Building lrat-check")
	subprocess.call("gcc lrat-check.c -O2 -o lrat-check", cwd = drattrimPath, shell=True)
	makeExecutable(drattrimPath + "/lrat-check")
	display("Building rupee")
	subprocess.call("make", cwd = rupeePath, shell = True)
	makeExecutable(rupeePath + "bin/rupee")
	makeExecutable(rupeePath + "bin/brattodrat")

def makeCheckers():
	remove(drattrimPath)
	downloadChecker()
	unpackChecker()
	buildChecker()

#------------------------
# Scripts
#------------------------

def generateBenchmarks():
	if os.path.exists(infoPath + "benchmark-list.txt"):
		os.remove(infoPath + "benchmark-list.txt")
	display("Downloading SAT Competition 2017 results")
	subprocess.call("wget --no-check-certificate https://baldur.iti.kit.edu/sat-competition-2017/results/main.csv -P " + infoPath, shell=True)
	os.rename(infoPath + "main.csv", infoPath + "satcomp-results.csv")
	display("Filtering instances")
	o = open(infoPath + "satcomp-results.csv")
	csvread = csv.reader(o)
	skipheader = False
	instances = set()
	for line in csvread:
		if skipheader:
			if line[4] == "UNSAT" and float(line[3]) <= 60.0:
				instances.add(line[0])
		else:
			skipheader = True
	o.close()
	o = open(infoPath + "benchmark-list.txt","w")
	for x in instances:
		o.write(x[4:-4] + "\n")
		print x
	o.close()

def getBenchmarks():
	cleanup(benchmarksPath)
	cleanup(tempPath)
	subprocess.call("wget --no-check-certificate https://baldur.iti.kit.edu/sat-competition-2017/benchmarks/Main.zip -P " + tempPath, shell=True)
	subprocess.call("unzip " + tempPath + "Main -d " + benchmarksPath, shell=True)
	files = os.listdir(benchmarksPath + "NoLimits")
	for file in files:
		shutil.move(benchmarksPath + "NoLimits/" + file, benchmarksPath)
	remove(benchmarksPath + "NoLimits")

def makeRunlim():
	remove(runlimPath)
	display("Unpacking runlim")
	cleanup(unpackPath)
	subprocess.call("tar -xf runlim-1.12.tar.gz -C " + unpackPath, shell=True)
	print unpackPath + "runlim-1.12"
	print runlimPath
	shutil.copytree(unpackPath + "runlim-1.12/", runlimPath)
	display("Building runlim")
	subprocess.call("./configure.sh", cwd=runlimPath, shell=True)
	subprocess.call("make", cwd=runlimPath, shell=True)

#------------------------
# Main
#------------------------

makeSolvers()
makeCheckers()
makeRunlim()
getBenchmarks()
generateBenchmarks()
remove(tempPath)
if not os.path.exists(outputPath):
	os.mkdir(outputPath)
if not os.path.exists(proofsPath):
	os.mkdir(proofsPath)
