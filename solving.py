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

#------------------------
# Directory structure and constants
#------------------------

memoryLimit = 8192
timeLimit = 300

solversPath = "solvers/"
drattrimPath = "drat-trim/drat-trim"
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

def solverOutputPath(solver, instance):
	return outputPath + instance + "." + solver + ".SS.out"

def checkerOutputPath(solver, instance):
	return outputPath + instance + "." + solver + ".DT.out"

def solverRunlimPath(solver, instance):
	return outputPath + instance + "." + solver + ".SS.run"

def checkerRunlimPath(solver, instance):
	return outputPath + instance + "." + solver  + ".DT.run"

#------------------------
# Command line generators
#------------------------

def commandSolver(solver, instance):
	proofpath = dratPath(solver, instance)
	if solver == "abcdsat_r17":
		post = "abcdsat_r17/bin/abcdsat_r17 " + cnfPath(instance) + " -certified -certified-output=" + proofpath
	elif solver == "bs_glucose":
		relcnf = os.path.relpath(os.getcwd() + "/" + cnfPath(instance), solversPath + "bs_glucose/bin/")
		relbrat = os.path.relpath(os.getcwd() + "/" + dratPath(solver, instance), solversPath + "bs_glucose/bin/")
		post = "bs_glucose1 " + relcnf + " -model -certified -verb=0 -certified-output=" + relbrat
	elif solver == "cadical-sc17-agile-proof":
		post = "cadical-sc17-agile-proof/bin/cadical --no-rephase --restartint=400 " + cnfPath(instance) + " " + proofpath
	elif solver == "cadical-sc17-proof":
		post = "cadical-sc17-proof/bin/cadical " + cnfPath(instance) + " " + proofpath
	elif solver == "Candy":
		post = "Candy/bin/candy -inprocessing=0 -verb=0 -model -certified -certified-output=" + proofpath + " " + cnfPath(instance)
	elif solver == "CandyRSILi":
		post = "CandyRSILi/bin/candy -verb=0 -model -certified -certified-output=" + proofpath + " -gate-patterns -no-gate-semantic -rsil-enable -rsil-min-gate-frac=0.7 -rsil-only-miters -gate-timeout=30 -rs-time-limit=60 -rsil-mode=implicationbudgeted -rsil-imp-budgets=10000 -rs-rounds=131072 -rs-abort-by-rrat -rs-rrat=0.0005 -rs-max-conj-size=2 " + cnfPath(instance)
	elif solver == "CandyRSILv":
		post = "CandyRSILv/bin/candy -verb=0 -model -certified -certified-output=" + proofpath + " -gate-patterns -no-gate-semantic -rsil-enable -rsil-min-gate-frac=0.7 -rsil-only-miters -gate-timeout=30 -rs-time-limit=60 -rsil-mode=vanishing -rsil-van-halflife=100000000 -rs-rounds=131072 -rs-abort-by-rrat -rs-rrat=0.0005 -rs-max-conj-size=2 " + cnfPath(instance)
	elif solver == "CandySL21":
		post = "CandySL21/bin/candy -inprocessing=21 -verb=0 -model -certified -certified-output=" + proofpath + " " + cnfPath(instance)
	elif solver == "COMiniSatPS_Pulsar_drup":
		post = "COMiniSatPS_Pulsar_drup/bin/minisat_static -drup-file=" + proofpath + " " + cnfPath(instance)
	elif solver == "GHackCOMSPS_drup":
		post = "GHackCOMSPS_drup/bin/GHackCOMSPS -model -vbyte -certified -certified-output=" + proofpath + " " + cnfPath(instance)
	elif solver == "glu_vc":
		post = "glu_vc/bin/glu_vc -model -verb=0 " + cnfPath(instance) + " -certified -certified-output=" + proofpath
	elif solver == "glucose-3.0+width":
		post = "glucose-3.0+width/bin/glucose_static " + cnfPath(instance) + " -model -certified -verb=0 -certified-output=" + proofpath
	elif solver == "glucose-4.1":
		post = "glucose-4.1/bin/glucose -vbyte -certified -certified-output=" + proofpath + " -model " + cnfPath(instance)
	elif solver == "lingeling-bbe":
		post = "lingeling-bbe/bin/lingeling -v " + cnfPath(instance) + " -t " + proofpath
	elif solver == "MapleCOMSPS_CHB_VSIDS_drup":
		post = "MapleCOMSPS_CHB_VSIDS_drup/bin/MapleCOMSPS_CHB_VSIDS -drup-file=" + proofpath + " " + cnfPath(instance)
	elif solver == "MapleCOMSPS_LRB_VSIDS_2_drup":
		post = "MapleCOMSPS_LRB_VSIDS_2_drup/bin/MapleCOMSPS_LRB_VSIDS_2 -drup-file=" + proofpath + " " + cnfPath(instance)
	elif solver == "MapleCOMSPS_LRB_VSIDS_drup":
		post = "MapleCOMSPS_LRB_VSIDS_drup/bin/MapleCOMSPS_LRB_VSIDS -drup-file=" + proofpath + " " + cnfPath(instance)
	elif solver == "Maple_LCM":
		post = "Maple_LCM/bin/glucose_static  " + cnfPath(instance) + "  -drup-file=" + proofpath
	elif solver == "Maple_LCM_Dist":
		post = "Maple_LCM_Dist/bin/glucose_static " + cnfPath(instance) + " -drup-file=" + proofpath
	elif solver == "MapleLRB_LCM":
		post = "MapleLRB_LCM/bin/glucose_static " + cnfPath(instance) + "  -drup-file=" + proofpath
	elif solver == "MapleLRB_LCMoccRestart":
		post = "MapleLRB_LCMoccRestart/bin/glucose_static " + cnfPath(instance) + "  -drup-file=" + proofpath
	elif solver == "Riss7_BVE":
		post = "Riss7_BVE/bin/riss -config=plain_BVE:BVEEARLY " + cnfPath(instance) + " -proofFormat=DRAT -proof=" + proofpath
	elif solver == "Riss7_noPP":
		post = "Riss7_noPP/bin/riss -config= " + cnfPath(instance) + " -no-enabled_cp3 -proofFormat=DRAT -proof=" + proofpath
	elif solver == "satUZK-seq_ge":
		post = "satUZK-seq_ge/bin/satUZK-seq --emulate Glucose --with-model --with-proof=drat-binary --proof-file " + proofpath + " " + cnfPath(instance)
	elif solver == "satUZK-seq_me":
		post = "satUZK-seq_me/bin/satUZK-seq --emulate MiniSat --with-model --with-proof=drat-binary --proof-file " + proofpath + " " + cnfPath(instance)
	elif solver == "satUZK-seq_sge":
		post = "satUZK-seq_sge/bin/satUZK-seq --simplify --emulate Glucose --with-model --with-proof=drat-binary --proof-file " + proofpath + " " + cnfPath(instance)
	elif solver == "satUZK-seq_sme":
		post = "satUZK-seq_sme/bin/satUZK-seq --simplify --emulate MiniSat --with-model --with-proof=drat-binary --proof-file " + proofpath + " " + cnfPath(instance)
	elif solver == "tch_glucose1":
		post = "tch_glucose1/bin/tch_glucose1 " + cnfPath(instance) + " -model -certified -verb=0 -certified-output=" + proofpath
	elif solver == "tch_glucose2":
		post = "tch_glucose2/bin/tch_glucose2 " + cnfPath(instance) + " -model -certified -verb=0 -certified-output=" + proofpath
	elif solver == "tch_glucose3":
		post = "tch_glucose3/bin/tch_glucose3 " + cnfPath(instance) + " -model -certified -verb=0 -certified-output=" + proofpath
	if solver == "bs_glucose":
		return post
	else:
		return solversPath + post

def limitCommand(command, out, run):
	return runlimPath + " -o " + run + " -s " + str(memoryLimit) + " -r " + str(timeLimit) + " " + command + " > " + out

#------------------------
# Execution
#------------------------

def callSolver(solver, instance):
	if solver != "bs_glucose":
		subprocess.call(limitCommand(commandSolver(solver, instance), solverOutputPath(solver, instance), solverRunlimPath(solver, instance)), shell=True)
	else:
		rellim = os.path.relpath(runlimPath, solversPath + "bs_glucose/bin/")
		relout = os.path.relpath(solverOutputPath(solver, instance), solversPath + "bs_glucose/bin/")
		relrun = os.path.relpath(solverRunlimPath(solver, instance), solversPath + "bs_glucose/bin/")
		relcmd = commandSolver(solver, instance)
		cwd = os.getcwd()
		os.chdir(solversPath + "bs_glucose/bin")
		subprocess.call(rellim + " -o " + relrun + " -s " + str(memoryLimit) + " -r " + str(timeLimit) + " " + relcmd + " > " + relout, shell=True)
		os.chdir(cwd)

#------------------------
# Workflow
#------------------------

if not os.path.exists(outputPath):
	os.mkdir(outputPath)
if not os.path.exists(proofsPath):
	os.mkdir(proofsPath)
solvers = readList("info/solver-list.txt")
instances = readList("info/benchmark-list.txt")
i = 0
for solver in solvers:
	for instance in instances:
		i = i + 1
		print str(i) + ":  " + solver + " + " + instance
		callSolver(solver, instance)
