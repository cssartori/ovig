## A script to read a CSV file in Open Addresses format,
## and extract all addresses that have the (lat,lon)
## within the boundaries of Barcelona city, Spain.
## This is roughly between southwest (41.303256, 2.061980)
## and northeast (41.465391, 2.231467).
## For some reason the Spain data of Open Addresses does
## not have any information on the city field, so
## it is not possible to filter by city as in the others.
## The resulting output are < only > the
## geo locations (lat,lon) of each unique
## available address in the input file (redirect in stdin)
##
## The output format can be read directly by the instance
## generator in order to create more realistic instances
## in the city of Barcelona, Spain.
##
## Carlo Sulzbach Sartori, 2018.

import sys, random;

head = sys.stdin.readline()

##CITY = "BARCELONA" ## actually useless, it is here for reference only

locs = set() ## a set to keep unique addresses
for line in sys.stdin:
	cl = line.split(",") ## split csv
	## cl[0] is longitude, cl[1] is latitude
	if len(cl[1]) > 6: ## that is, if it was informed a latitude field (some entries have none)
		if float(cl[0]) > 2.05 and float(cl[0]) < 2.24 and float(cl[1]) > 41.29 and float(cl[1]) < 41.47:
			gps = ("%s %s" % (cl[1], cl[0])) ## invert geo locations
			locs.add(gps) ## add to set

## print all unique locations in set
pc=0 ## id counter
print "NODES" ## needed for the instance generator
for l in locs:
	print "%d %s" % (pc+1, l) ## print "id lat lon"
	pc += 1
print "EOF" ## needed for the instance generator
