#include "../include/builder.hpp"
#include <algorithm>

using namespace std;

namespace Builder{

	//Builds an instance of the problem specified in InstanceType (currently supported types are PDPTW and CVRP)
	//This function considers that locations are given and all the information must be generated.
	int build(const Configurations& con, const std::vector<GeoLocation>& locations, std::vector< std::vector< double > >& matrix, Instance& inst){
		int res = EXIT_FAILURE;
		if(con.type == InstanceType::PDPTW)
			res = PDPTW::build(con, locations, matrix, inst);
		else if(con.type == InstanceType::CVRP)
			res = CVRP::build(con, locations, matrix, inst);
		
		return res;
	}

	//Builds an instance of the problem specified in InstanceType (currently supported type is PDPTW)
	//This function considers that complete Nodes are given and only some of the information must be generated (this is used to create PDPTW New York Taxi instances)
	int build(const Configurations& con, std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix, Instance& inst){
		int res = EXIT_FAILURE;

		res = PDPTW::build(con, nodes, matrix, inst);
		
		return res;
	}

};
