#! /usr/bin/env python

import os
import sys
import fnmatch
import time
import shutil
import subprocess
import re

def readList(file):
	o = open(file)
	lines = o.read().splitlines()
	o.close()
	lines = filter(lambda line : len(line) > 0 and line[0] != "#", lines)
	return lines

solvers = readList("info/solver-list.txt")
instances = readList("info/benchmark-list.txt")
master = []
for solver in solvers:
	for instance in instances:
		lines = readList("output/" + instance + "." + solver + ".DT.run")
		dtstatus = "fail"
		for line in lines:
			if len(line) >= 20 and line[0:20] == "[runlim] status:\t\tok":
				dtstatus = "ok"
			if line[0:14] == "[runlim] real:":
				dttime = re.findall("\d+\.\d+", line)[0]
		lines = readList("output/" + instance + "." + solver + ".DT.out")
		dtresult = "fail"
		for line in lines:
			if line == "s VERIFIED":
				dtresult = "accept"
			elif line == "s NOT VERIFIED":
				dtresult = "reject"
		lines = readList("output/" + instance + "." + solver + ".SD.run")
		sdstatus = "fail"
		for line in lines:
			if len(line) >= 20 and line[0:20] == "[runlim] status:\t\tok":
				sdstatus = "ok"
			if line[0:14] == "[runlim] real:":
				sdtime = re.findall("\d+\.\d+", line)[0]
		lines = readList("output/" + instance + "." + solver + ".SD.out")
		sdresult = "fail"
		sdrat = 0
		for line in lines:
			if line == "s ACCEPTED":
				sdresult = "accept"
			elif line == "s REJECTED":
				sdresult = "reject"
			elif line[0:2] == "nr":
				sdrat = int(line[3:])
		lines = readList("output/" + instance + "." + solver + ".FD.run")
		fdstatus = "fail"
		for line in lines:
			if len(line) >= 20 and line[0:20] == "[runlim] status:\t\tok":
				fdstatus = "ok"
			if line[0:14] == "[runlim] real:":
				fdtime = re.findall("\d+\.\d+", line)[0]
		lines = readList("output/" + instance + "." + solver + ".FD.out")
		fdresult = "fail"
		for line in lines:
			if line == "s ACCEPTED":
				fdresult = "accept"
			elif line == "s REJECTED":
				fdresult = "reject"
		master.append([instance, solver, dtstatus,dttime,dtresult,sdstatus,sdtime,sdresult,fdstatus,fdtime,fdresult,sdrat])
total = len(master)
dtagsd = 0
totaldtagsd = 0
sdagfd = 0
totalsdagfd = 0
sd1fd0 = 0
sd0fd1 = 0
totalratdisc = 0
cadicaldisc = 0
cadicaltotal = 0
glucosedisc = 0
glucosetotal = 0
cominisatdisc = 0
cominisattotal = 0
mapledisc = 0
mapletotal = 0
for [instance, solver, dtstatus,dttime,dtresult,sdstatus,sdtime,sdresult,fdstatus,fdtime,fdresult,sdrat] in master:
	if dtstatus == "ok" and sdstatus == "ok":
		totaldtagsd = totaldtagsd + 1
		if dtresult == sdresult:
			dtagsd = dtagsd + 1
	if sdstatus == "ok" and fdstatus == "ok":
		totalsdagfd = totalsdagfd + 1
		if solver == "COMiniSatPS_Pulsar_drup":
			cominisattotal = cominisattotal + 1
		elif solver == "Maple_LCM_Dist":
			mapletotal = mapletotal + 1
		elif solver == "cadical-sc17-proof":
			cadicaltotal = cadicaltotal + 1
		elif solver == "glucose-4.1":
			glucosetotal = glucosetotal + 1
		if sdresult == fdresult:
			sdagfd = sdagfd + 1
		else:
			if solver == "COMiniSatPS_Pulsar_drup":
				cominisatdisc = cominisatdisc + 1
			elif solver == "Maple_LCM_Dist":
				mapledisc = mapledisc + 1
			elif solver == "cadical-sc17-proof":
				cadicaldisc = cadicaldisc + 1
			elif solver == "glucose-4.1":
				glucosedisc = glucosedisc + 1
			if sdrat != 0:
				totalratdisc = totalratdisc + 1
			if sdresult == "accept":
				sd1fd0 = sd1fd0 + 1
			elif fdresult == "accept":
				sd0fd1 = sd0fd1 + 1
print "DRAT-trim agrees with Rupee-SD in " + str(dtagsd) + " / " + str(totaldtagsd) + " instances solved by both"
print "Rupee-SD agrees with Rupee-FD in " + str(sdagfd) + " / " + str(totalsdagfd) + " instances solved by both"
print "Rupee-SD accepts while Rupee-FD rejects in " + str(sd1fd0) + " / " + str(totalsdagfd) + " instances solved by both"
print "Rupee-FD accepts while Rupee-SD rejects in " + str(sd0fd1) + " / " + str(totalsdagfd) + " instances solved by both"
print "Proof contains some RAT in " + str(totalratdisc) + " / " + str(sd1fd0 + sd0fd1) + " discrepant instances"
print "COMiniSatPS_Pulsar_drup produces discrepant proofs in " + str(cominisatdisc) + " / " + str(cominisattotal) + " instances"
print "Maple_LCM_Dist produces discrepant proofs in " + str(mapledisc) + " / " + str(mapletotal) + " instances"
print "cadical produces discrepant proofs in " + str(cadicaldisc) + " / " + str(cadicaltotal) + " instances"
print "glucose-4.1 produces discrepant proofs in " + str(glucosedisc) + " / " + str(glucosetotal) + " instances"
dttimes = []
sdtimes = []
fdtimes = []
for [instance, solver, dtstatus,dttime,dtresult,sdstatus,sdtime,sdresult,fdstatus,fdtime,fdresult,sdrat] in master:
	if dtstatus == "ok" and sdstatus == "ok" and fdstatus == "ok":
		if dtresult == "accept" and sdresult == "accept" and fdresult == "accept":
			dttimes.append(float(dttime))
			sdtimes.append(float(sdtime))
			fdtimes.append(float(fdtime))
dttimes.sort()
sdtimes.sort()
fdtimes.sort()
o = open("data.txt", "w")
i = 0
while i < len(dttimes):
	o.write(str(i) + "\t" + str(dttimes[i]) + "\t" + str(sdtimes[i]) + "\t" + str(fdtimes[i]) + "\n")
	i = i + 1
o.close()
