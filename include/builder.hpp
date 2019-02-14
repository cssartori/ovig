#ifndef __H_BUILDER_H__
#define __H_BUILDER_H__

#include "common.hpp"
#include "configurations.hpp"
#include "instance.hpp"
#include "node.hpp"

namespace Builder{
	//Builds an instance of the problem specified in InstanceType (currently supported types are PDPTW and CVRP)
	//This function considers that locations are given and all the information must be generated.
	int build(const Configurations& con, const std::vector<GeoLocation>& locations, std::vector< std::vector< double > >& matrix, Instance& inst);
	//Builds an instance of the problem specified in InstanceType (currently supported type is PDPTW)
	//This function considers that complete Nodes are given and only some of the information must be generated (this is used to create PDPTW New York Taxi instances)
	int build(const Configurations& con, std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix, Instance& inst);


	//defines namespace for each problem
	namespace PDPTW{
		int build(const Configurations& con, const std::vector<GeoLocation>& locations, std::vector< std::vector< double > >& matrix, Instance& inst);
		int build(const Configurations& con, std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix, Instance& inst);
	};

	namespace CVRP{
		//for now, there is only one build method for the CVRP
		int build(const Configurations& con, const std::vector<GeoLocation>& locations, std::vector< std::vector< double > >& matrix, Instance& inst);
	};
}

#endif //__H_BUILDER_H__
