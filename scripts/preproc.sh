#!/bin/bash

## script to perform the pre-processing step in OSM files using OSRM

## IMPORTANT: must change the profile for car use so that the metric
## used when computing edge values is distance/duration, in which
## case the triangle inequality will hold.

## first extract the data from OSM file using the profile for cars
osrm-extract -p profiles/car.lua berlin-data2/berlin-latest.osm.pbf

## partition and customize (whatever this means)
osrm-partition berlin-data2/berlin-latest.osrm
osrm-customize berlin-data2/berlin-latest.osrm

## execute the routing algorithm MLD
osrm-routed --algorithm mld berlin-data2/berlin-latest.osrm
