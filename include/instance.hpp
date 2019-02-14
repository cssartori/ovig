#ifndef __H_INSTANCE_H__
#define __H_INSTANCE_H__

#include "common.hpp"
#include "node.hpp"
#include <vector>

/*
 * Data structure to keep the information about the generated instance. This is used in the end
 * of the process to write the instance to a text file.
 *
 */

class Instance{
public:
	std::string name;
	std::string comment;
	NodeDistribution dist_type;
	size_t nclusters;
	double density;
	DepotType depot;
	std::string location;
	size_t size;
	InstanceType type;
	size_t max_capacity; // vehicle's max capacity
	size_t time_window;
	double max_route_time; // the same as depot time window
	std::vector<Node> nodes; // nodes (locations) in the input graph
	std::vector< std::vector< double > > matrix; // matrix of times/costs

	static FileCode write_instance_file(const std::string& filename, const Instance& inst);
	
};


#endif //__H_INSTANCE_H__
