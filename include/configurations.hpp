#ifndef __H_CONFIGURATIONS_H__
#define __H_CONFIGURATIONS_H__

#include <string>
#include <vector>
#include <iostream>
#include "node.hpp"
#include "common.hpp"

/*
 * A data structure to keep the configurations to be used when generating the instances. It reads the configuration file given as a parameter to the
 * generator and stores all the information necessary to be used during the process.
 *
 */

struct Configurations{
	std::string iname; // name of the instance
	size_t num_points; // number of points
	NodeDistribution dist_type; // distribution type of nodes in space
	DepotType depot_type;
	size_t nclusters;
	double density; // density of clusters
	size_t max_route_time; // maximum time of routes in minutes
	size_t service_time; // waiting time at each location in minutes
	size_t time_window; // time window width
	size_t capacity; // the capacity of the vehicles
	size_t seed; // seed for RNG
	std::string osrm_file; // primary OSRM data file
	std::string land_file; // land data shapefile
	std::string node_file; // file with list of nodes
	std::vector<double> limits; // rectangle limits coordinates
	InstanceType type; // type of problem
	std::string locname; // location name
	std::string comment; // comment
	int verbose; //verbose level
	std::string inst_file; //output instance filename
	size_t full_nodes; 
	std::vector<Node> nodes; // nodes
	
	//read the specification area of the input stream
    static FileCode read_configurations(struct Configurations& con, std::istream& f);
};


#endif //__H_CONFIGURATIONS_H__
