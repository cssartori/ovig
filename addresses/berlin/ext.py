## A script to read a CSV file in Open Addresses format,
## and extract all addresses that have the city field
## as "Berlin". The resulting output are < only > the
## geo locations (lat,lon) of each unique
## available address in the input file (redirect in stdin)
##
## The output format can be read directly by the instance
## generator in order to create more realistic instances
## in the city of Berlin, Germany.
##
## Carlo Sulzbach Sartori, 2018.

import sys;

CITY = "BERLIN" ## "constant" value for the city

head = sys.stdin.readline() ## skip the header

locs = set() ## a set to keep unique addresses
for line in sys.stdin:
	cl = line.split(",") ## split csv
	city = cl[5].upper() ## upper case city field
	if city == CITY:
		gps = ("%s %s" % (cl[1], cl[0])) ## invert geo locations
		locs.add(gps) ## add to set

## print all unique locations in set
pc=0 ## id counter
print "NODES" ## needed for the instance generator
for l in locs:
	print "%d %s" % (pc+1, l) ## print "id lat lon"
	pc += 1
print "EOF" ## needed for the instance generator
