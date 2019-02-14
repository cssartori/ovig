#ifndef __H_ROUTER_H__
#define __H_ROUTER_H__

#include <vector>

#include "osrm/engine_config.hpp"
#include "osrm/osrm.hpp"

#include "common.hpp"

#include <string>
#include <utility>
#include <cstdlib>

/*
 * A class that keeps the router function.
 * In summary, it handles all calls to the OSRM tool to compute realistic travel times based on OSM data.
 *
 */

class Router {
	
public:
	Router(std::string osrm_data);
	void route(const std::vector<GeoLocation>& locations, std::vector< std::vector<double> >& matrix);
 
private:
	osrm::EngineConfig config;
	osrm::OSRM *osrm;
	
	void table_route(const std::vector<GeoLocation> &locations, std::vector< std::vector<double> >& matrix);

};

#endif //__H_ROUTER_H__
