#include "../include/instance_creator.hpp"
#include "../include/builder.hpp"
#include "../include/router.hpp"
#include "../include/constants.hpp"
#include "../include/distributer.hpp"

//TODO: deserves more commenting

int holds_triangular(const std::vector< std::vector<double> >& mat);
int is_assymmetric(const std::vector< std::vector<double> >& mat);

namespace InstanceCreator{
	//TODO: modify code for cases CVRP, VRPTW and PDP
	int create(const Configurations& con, Instance& inst){
#ifdef __USE_GDAL_LAND_AREA__
		GDALDatasetUniquePtr land_areas = nullptr;
		if(con.land_file.size() > 0){
			// initialize GDAL drivers and land areas file
			GDALAllRegister();
			if(con.verbose >= 2) printf("Opening land areas file %s...\n", con.land_file.c_str());
			land_areas = GDALDatasetUniquePtr(GDALDataset::Open( con.land_file.c_str(), GDAL_OF_VECTOR));
			if( land_areas == nullptr ) {
				printf( "Could not open land data file \"%s\"\n", con.land_file.c_str());
				return EXIT_FAILURE;
			}
			if(con.verbose >= 2) printf("DONE: Land areas file opened.\n");
		}
#endif
		std::vector<GeoLocation> locations;
		std::vector<Node> nodes;
		if(con.full_nodes == 0){ //no previous locations informed
			if(con.verbose >= 2) printf("Generating %ld locations...\n", con.num_points);
#ifdef __USE_GDAL_LAND_AREA__
			Distributer dit(land_areas);
#else
			Distributer dit;
#endif
			locations = dit.generate_locations(con.limits, con.num_points, con.depot_type, con.dist_type, con.nclusters, con.density);
			if(con.verbose >= 2) printf("DONE: All %ld locations generated.\n", con.num_points);
		}else if(con.full_nodes == 1){
			if(con.verbose >= 2) printf("Selecting %ld locations from configuration...\n", con.num_points);
			Distributer dit;
			std::vector<GeoLocation> locs;
			for(Node nd : con.nodes) locs.push_back(nd.loc);
			locations = dit.select_locations(locs, con.num_points, con.depot_type, con.dist_type, con.nclusters, con.density);
			if(con.verbose >= 2) printf("DONE: All %ld locations selected.\n", con.num_points);
		}else{ //all locations exaclty informed
			if(con.verbose >= 2) printf("Selecting %ld complete nodes from configuration...\n", con.num_points);
			Distributer dit;
			nodes = dit.select_nodes(con.nodes, con.num_points, con.depot_type, con.time_window, con.max_route_time);
			for(Node nd : nodes) locations.push_back(nd.loc);
			if(con.verbose >= 1) printf("DONE: All %ld complete nodes selected.\n", con.num_points);
		}

		if(con.verbose >= 3){
			printf("ALL LOCATIONS %ld:\n", locations.size());
			for(GeoLocation loc : locations) printf("%f , %f\n", loc.lat, loc.lon);
		}
		//exit(1);
		
		if(con.verbose >= 2) printf("Routing time matrix...\n");
		//TODO: change this name to 'times'
		std::vector< std::vector<double> > distances(locations.size(), std::vector<double>(locations.size(), 0));
		Router router(con.osrm_file);
		router.route(locations, distances);
		
		if(con.verbose >= 2){
			if(con.verbose >= 3){
				printf("Matrix triangular: %s\n", (holds_triangular(distances) == 0) ? "true" : "false");
				int assm = is_assymmetric(distances);
				printf("Matrix assymmetric: %f (total = %i)\n", assm*100.0/(0.5*distances.size()*distances.size()-distances.size()), assm);
			}
			printf("DONE: Time matrix routed.\n");
		}
		
		if(con.verbose >= 4){
			std::cout << "FULL TIME MATRIX: " << std::endl;
			for(size_t i=0;i<locations.size();i++){
				for(size_t j=0;j<locations.size();j++){
					printf("%2.0f\t", distances[i][j]);
				}
				std::cout << "\n";
			}
		}

		if(con.verbose >= 2) printf("Building instance...\n");
		int ret;
		if(con.full_nodes != Constants::NUM_NDF_FULL) ret = Builder::build(con, locations, distances, inst);
		else ret = Builder::build(con, nodes, distances, inst);

		inst.name = con.iname;
		inst.type = con.type;
		inst.dist_type = con.dist_type;
		inst.size = con.num_points;
		inst.comment = con.comment;
		inst.location = con.locname;
		inst.max_capacity = con.capacity;
		inst.max_route_time = con.max_route_time;
		inst.depot = con.depot_type;
		inst.nclusters = con.nclusters;
		inst.density = con.density;
		inst.time_window = con.time_window;
		
		if(con.verbose >= 2 and ret == EXIT_SUCCESS) printf("DONE: instance built.\n");
		else if(con.verbose >= 2) printf("WARNING: some errors ocurred building the instance.\n");
		return ret;
	}

};


int holds_triangular(const std::vector< std::vector<double> >& mat){
	int nerr = 0;
	for(size_t i=1;i<mat.size();i++){
		for(size_t j=1;j<mat.size();j++){
			for(size_t k=1;k<mat.size();k++){
				if(mat[i][k] - (mat[i][j] + mat[j][k]) > EPSILON){
					nerr++;
				}
			}
		}
	}
	
	return nerr;
}

int is_assymmetric(const std::vector< std::vector<double> >& mat){
	int nerr = 0;
	for(size_t i=0;i<mat.size();i++){
		for(size_t j=i+1;j<mat.size();j++){
			if(mat[i][j] != mat[j][i]){
				nerr++;
			}
		}
	}
	
	return nerr;
}
