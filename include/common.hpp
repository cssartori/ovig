#ifndef __H_COMMON_H__
#define __H_COMMON_H__

#include <random>
#include <string>
#include "geolocation.hpp"
#include "constants.hpp"

/*
 * This file keeps some common definitions and constant values used throughout the whole
 * project.
 */

//uncomment below in case of using the land areas file (shape file): probably only useful if generating completely random locations
//#define __USE_GDAL_LAND_AREA__

extern std::mt19937_64 rng;

enum class NodeType{Pickup, Delivery, Depot};
enum class GenType{Rectangle, Circle};
enum class FileCode{ReadError, ReadOk, WriteError, WriteOk};
enum class NodeDistribution{Cluster, Random, ClusterRandom, Unknown};
enum class DepotType{Central, Random};
enum class InstanceType{PDPTW, CVRP, VRPTW, PDP, Unknown}; // TODO: add more types as CVRP, VRPTW, ...
enum class TimeHorizon{Short, Medium, Long};

const static std::string DEFAULT_CONFIG_FILE = "configuration.txt";
const static std::string DEFAULT_INST_FILE = "instance.txt";

const static double EPSILON = 1e-8;
const static int NO_NODE = -1;

const static int LNORTH = 0;
const static int LSOUTH = 1;
const static int LEAST = 2;
const static int LWEST = 3;

inline std::string type_to_string(InstanceType type){
	if(type == InstanceType::PDPTW) return Constants::STR_TYPE_PDPTW;
	else if(type == InstanceType::CVRP) return Constants::STR_TYPE_CVRP;
	else if(type == InstanceType::VRPTW) return Constants::STR_TYPE_VRPTW;
	else if(type == InstanceType::PDP) return Constants::STR_TYPE_PDP;
	
	return std::string("Unknown");
}

inline std::string type_to_string(NodeDistribution type){
	if(type == NodeDistribution::Cluster) return Constants::STR_DIST_CLUSTER;
	else if(type == NodeDistribution::Random) return Constants::STR_DIST_RANDOM;
	else if(type == NodeDistribution::ClusterRandom) return Constants::STR_DIST_CLUST_RAND;
	
	return std::string("Unkown");
}

inline std::string type_to_string(DepotType type){
	if(type == DepotType::Central) return Constants::STR_DEPOT_CENTRAL;
	else if(type == DepotType::Random) return Constants::STR_DEPOT_RANDOM;
	
	return std::string("Unkown");
}


inline InstanceType string_to_type1(std::string str){
	if(str == Constants::STR_TYPE_PDPTW) return InstanceType::PDPTW;
	else if(str == Constants::STR_TYPE_CVRP) return InstanceType::CVRP;
	else if(str == Constants::STR_TYPE_PDP) return InstanceType::PDP;
	else if(str == Constants::STR_TYPE_VRPTW) return InstanceType::VRPTW;

	return InstanceType::Unknown;
}

inline NodeDistribution string_to_type2(std::string str){
	if(str == Constants::STR_DIST_CLUSTER) return NodeDistribution::Cluster;
	else if(str == Constants::STR_DIST_RANDOM) return NodeDistribution::Random;
	else if(str == Constants::STR_DIST_CLUST_RAND) return NodeDistribution::ClusterRandom;

	return NodeDistribution::Unknown;
}


#endif //__H_COMMON_H__
