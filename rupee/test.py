#! /usr/bin/env python

import os
import sys
import fnmatch
import time

# tool = sys.argv[1]
instancelist = map(lambda x: x[:-5], os.listdir("test/proof"))
for x in instancelist:
	print "rupee-sd on " + x + ":"
	out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/proof/" + x + ".drat -lrat test/lrat/" + x + ".sd.lrat -skip-deletion")
	print out.read()
	print ""
	print "rupee-fd on " + x + ":"
	out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/proof/" + x + ".drat -lrat test/lrat/" + x + ".fd.lrat -full-deletion")
	print out.read()
	print ""

# if tool == "rupee-sd":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/drat"))
# 	for x in instancelist:
# 		print "rupee-sd on " + x + ":"
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/drat/" + x + ".drat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -skip-deletion")
# 		print out.read()
# elif tool ==  "rupee-fd":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/drat"))
# 	for x in instancelist:
# 		print "rupee-fd on " + x + ":"
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/drat/" + x + ".drat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -full-deletion")
# 		print out.read()
# elif tool ==  "rupee-bin-sd":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/brat"))
# 	for x in instancelist:
# 		print "rupee-bin-sd on " + x + ":"
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/brat/" + x + ".brat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -skip-deletion -binary")
# 		print out.read()
# elif tool ==  "rupee-bin-fd":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/brat"))
# 	for x in instancelist:
# 		print "rupee-bin-fd on " + x + ":"
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/brat/" + x + ".brat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -full-deletion -binary")
# 		print out.read()
# elif tool ==  "lratcheck":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/lrat"))
# 	for x in instancelist:
# 		print "lratcheck on " + x + ":"
# 		out = os.popen("bin/lratcheck test/cnf/" + x + ".cnf test/lrat/" + x + ".lrat")
# 		print out.read()
# elif tool ==  "sickcheck":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/sick"))
# 	for x in instancelist:
# 		print "sickcheck on " + x + ":"
# 		out = os.popen("bin/sickcheck test/cnf/" + x + ".cnf test/drat/" + x + ".drat test/sick/" + x + ".sick")
# 		print out.read()
# elif tool ==  "sickcheck-bin":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/sick"))
# 	for x in instancelist:
# 		print "sickcheck-bin on " + x + ":"
# 		out = os.popen("bin/sickcheck-bin test/cnf/" + x + ".cnf test/drat/" + x + ".drat test/sick/" + x + ".sick")
# 		print out.read()
# elif tool == "conversion":
# 	instancelist = map(lambda x : x[:-5], os.listdir("test/proof"))
# 	for x in instancelist:
# 		print "drat-to-brat on " + x + ":"
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/proof/" + x + ".drat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -full-deletion")
# 		for l in out.read().splitlines():
# 			if l[0] == 's':
# 				print l
# 		os.popen("bin/drattobrat test/proof/" + x + ".drat test/brat/" + x + ".brat")
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/brat/" + x + ".brat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -full-deletion -binary")
# 		for l in out.read().splitlines():
# 			if l[0] == 's':
# 				print l
# 		os.popen("bin/brattodrat test/brat/" + x + ".brat test/drat/" + x + ".drat")
# 		out = os.popen("bin/rupee test/cnf/" + x + ".cnf test/drat/" + x + ".drat -lrat test/lrat/" + x + ".lrat -recheck test/sick/" + x + ".sick -full-deletion")
# 		for l in out.read().splitlines():
# 			if l[0] == 's':
# 				print l
# else:
# 	print "input error"
