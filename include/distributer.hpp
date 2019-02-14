#ifndef __H_DISTRIBUTER_H__
#define __H_DISTRIBUTER_H__

#include "common.hpp"
#include "node.hpp"
#ifdef __USE_GDAL_LAND_AREA__
#include "../gdal-2.3.0/ogr/ogrsf_frmts/ogrsf_frmts.h"
#endif
#include <vector>

/*
 * Distributer of locations in the space according to pre-defined types.
 * The distirbuter can both generate random geographic locations (lat,lon) in a given area limited by a rectangle <limits>, or
 * simply select geolocations from a pre-defined list/pool provided in the input data (this is the main use so far).
 * When generating locations, one should use the GDAL tool and a shapefile to avoid generating point on water.
 *
 * When selecting locations from a list, one needs to provide a Node file with prefereably a large number of locations to choose from.
 * The more locations in the list, the better the distribution will be.
 */

class Distributer {
private:
#ifdef __USE_GDAL_LAND_AREA__
	const GDALDatasetUniquePtr& land_areas; // data loaded from a file containing the land areas valid to be used
#endif
	
	bool is_valid_location(GeoLocation loc, GenType gen_type=GenType::Rectangle, double cradius=0.0);
	std::vector<GeoLocation> select_random_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used);
	std::vector<GeoLocation> select_semi_clustered_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used, size_t nclusters = 1, double r=1.0);
	std::vector<GeoLocation> select_clustered_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used, size_t nclusters = 1, double r=1.0);
	std::vector<GeoLocation> generate_random_locations(std::vector<double> limits, const size_t num_points);
	std::vector<GeoLocation> generate_clustered_locations(std::vector<double> limits, const size_t num_points, size_t nclusters = 1, double r=1.0);
	std::vector<GeoLocation> generate_semi_clustered_locations(std::vector<double> limits, const size_t num_points, size_t nclusters = 1, double r=1.0);
	
public:
#ifdef __USE_GDAL_LAND_AREA__
	Distributer() : land_areas(nullptr){ }
	Distributer(const GDALDatasetUniquePtr& la) : land_areas(la){ }
#endif
	

	std::vector<GeoLocation> select_locations(const std::vector<GeoLocation>& locs, const size_t num_points, const DepotType dptt, const NodeDistribution distribution, size_t nclusters = 1, double r=1.0);
	std::vector<GeoLocation> generate_locations(const std::vector<double>& limits, const size_t num_points, const DepotType dptt, const NodeDistribution distribution, size_t nclusters = 1, double r=1.0);
	std::vector<Node> select_nodes(const std::vector<Node>& nodes, const size_t num_points, const DepotType dptt, const size_t tw_size, const size_t max_route_time);


	
};

#endif //__H_DISTRIBUTER_H__
