#ifndef __H_CONSTANTS_H__
#define __H_CONSTANTS_H__

#include <string>

namespace Constants{
	const size_t NUM_NDF_NONE = 0;
	const size_t NUM_NDF_LOCS = 1;
	const size_t NUM_NDF_FULL = 2;
	
	const size_t NUM_HORIZON_SHORT = 120;
	
	const std::string STR_DIST_CLUSTER = "cluster";
	const std::string STR_DIST_RANDOM = "random";
	const std::string STR_DIST_CLUST_RAND = "cluster-random";

	//requires implementing
	const std::string STR_TYPE_PDPTW = "PDPTW";
	const std::string STR_TYPE_PDP = "PDP";
	const std::string STR_TYPE_VRPTW = "VRPTW";
	const std::string STR_TYPE_CVRP = "CVRP";

	const std::string STR_DEPOT_CENTRAL = "central";
	const std::string STR_DEPOT_RANDOM = "random";

	const std::string STR_CON_NAME = "NAME";
	const std::string STR_CON_SIZE = "SIZE";
	const std::string STR_CON_DIST = "DISTRIBUTION";
	const std::string STR_CON_CLUSTERS = "CLUSTERS";
	const std::string STR_CON_NODE_FILE = "NODE-FILE";
	const std::string STR_CON_DENSITY = "DENSITY";
	const std::string STR_CON_ROUTE_TIME = "ROUTE-TIME";
	const std::string STR_CON_SEED = "SEED";
	const std::string STR_CON_CAPACITY = "CAPACITY";
	const std::string STR_CON_DEPOT = "DEPOT";
	//const std::string STR_CON_DEMAND = "DEMAND";
	const std::string STR_CON_TIME_WINDOW = "TIME-WINDOW";
	const std::string STR_CON_SERV_TIME = "SERVICE-TIME";
	const std::string STR_CON_OSRM_FILE = "OSRM-FILE";
	const std::string STR_CON_LAND_FILE = "LAND-FILE";
	const std::string STR_CON_LIMITS = "LIMITS";
	const std::string STR_CON_TYPE = "TYPE";
	const std::string STR_CON_LOCATION = "LOCATION";
	const std::string STR_CON_COMMENT = "COMMENT";

	const std::string STR_NDF_NODES = "NODES";

	const std::string STR_EOF = "EOF";
	const char CHR_SEPARATOR = ':';
	const char CHR_COMMENT = '#';
};
#endif //__H_CONSTANTS_H__
