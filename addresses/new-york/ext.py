## A script to read a CSV file from the New York
## taxis database, kept by University of Illinois
##
## Link:
## https://databank.illinois.edu/datasets/IDB-9610843
## The description can be found at:
## https://databank.illinois.edu/datafiles/18odq/display
##
## The script extracts data location from the august, 2013
## dataset, from the days 1, 2, and 3. Additionally, only
## from requests served from 9 AM to 5 PM (8 hour window).
##
## Carlo Sulzbach Sartori, 2018.

import sys;
import random;

SH = 9 ## starting hour : 8 AM
EH = 17 ## ending hour : 6 PM
TOTAL_HOURS = EH - SH ## change this if day changes!
DEPOT_LTW = float(TOTAL_HOURS)*60

## date from which to take the instance data
DAYS = [1,2,3] 
MONTH = 8
YEAR = 2013

ll = sys.stdin.readline()
lines = []
for line in sys.stdin:
	lines.append(line)

c=0
idi=0

print "NODES"

iter = 0
while iter < len(lines):
	line = lines[iter]
	cl = line.split(",")
	tp = (cl[5].split(" ")[1]).split(":") ## pickup time
	dp = (cl[5].split(" ")[0]).split("-") ## pickup date
	td = (cl[6].split(" ")[1]).split(":") ## dropoff time
	dd = (cl[6].split(" ")[0]).split("-") ## dropoff date
	start_p = (int(tp[0])-SH)*60+int(tp[1])
	end_p = (EH-int(tp[0]))*60-int(tp[1])
	start_d = (int(td[0])-SH)*60+int(td[1])
	end_d = (EH-int(td[0]))*60-int(td[1])

	
	if (start_p >= 0 and end_p >= 0 and start_d >= 0 and end_d >= 0
		and int(dd[2]) in DAYS and int(dd[1]) == MONTH and int(dd[0]) == YEAR):
		rlen = abs(60*(int(td[0])-int(tp[0]))-abs(int(td[1])-int(tp[1]))) ## race length

		ptb = (int(tp[0])-SH)*60+int(tp[1]) ## pickup base tw
		dtb = (int(td[0])-SH)*60+int(td[1]) ## delivery base tw

		if(rlen >= 10 and int(cl[8]) > 1000 and ptb < dtb and
		40.0 < float(cl[11]) and float(cl[11]) < 42.0 and
		-74.0 < float(cl[10]) and float(cl[10]) < -72.0):
			if ptb < dtb and ptb < DEPOT_LTW and dtb < DEPOT_LTW:
				print "%d %s %s %s %d 0 %d" % (idi,cl[11],cl[10],cl[7],ptb,idi+1)
				cl[13] = cl[13][:-2]
				print "%d %s %s -%s %d %d 0" % (idi+1,cl[13],cl[12],cl[7],dtb,idi)
				idi+=2	
	iter += 1

print "EOF"

# print "ROUTE-TIME: %d" % (TOTAL_HOURS*60)
# print "SEED: 0"
# print "CAPACITY: %d" % (CAP)
# print "OSRM-DATA: ny-data/new-york-latest.osrm"
# print "LAND-DATA: new-york-latest-free/gis_osm_landuse_a_free_1.shp"
# print "TYPE: PDPTW"
# print "LOCATION: New-York"
# print "COMMENT: Sartori & Buriol"
# print "EOF"
	
#print "C = %d" % (c)
