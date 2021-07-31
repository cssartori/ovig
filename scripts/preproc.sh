#!/bin/bash

## script to perform the pre-processing step in OSM files using OSRM

## IMPORTANT: must change the profile for car use so that the metric
## used when computing edge values is distance/duration, in which
## case the triangle inequality will hold.

PROFD=""
if [ "$1" == "" ] ; then
    echo "Missing profile file."
    exit 0
else
    PROFD=$1
fi

MAPSD=""
if [ "$2" == "" ] ; then
    echo "Missing OSM map file."
    exit 0
else
    MAPSD=$2
fi

mapdir="$(dirname $MAPSD)/"
base_mapfile="$(basename $MAPSD)"
mapfile="$(cut -d'.' -f1 <<<$base_mapfile)"

echo "Using profile at: ${PROFD}"
echo "Using map directory: ${mapdir}"
echo "Using map base file: ${base_mapfile}"
echo "Using map name as: ${mapfile}"

## first extract the data from OSM file using the profile for cars
osrm-extract -p $PROFD $MAPSD

## partition and customize (whatever this means)
osrm-partition $mapdir$mapfile.osrm
osrm-customize $mapdir$mapfile.osrm

## execute the routing algorithm MLD
## only if we want to make HTTP requests I suppose
## osrm-routed --algorithm mld berlin-data2/berlin-latest.osrm
