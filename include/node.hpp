#ifndef __H_NODE_H__
#define __H_NODE_H__

#include "common.hpp"

//strucutre that defines a node in VRP problems
//For CVRP, VRPTW and PDPTW it has everything that is needed
//for other problems it may require some additional features
//or even a refactoring.

struct Node {
	int id; // its ID
	GeoLocation loc; // geospatial location
	int demand; // its demand: d<0 is delivery ; d > 0 is pickup
	double etw; // start of time window
	double ltw; // end of time window
	double stw; // service time
	int pair; // its pair index: pickup or delivery
	NodeType type; // its type

	Node(const int _id, const GeoLocation _loc){
		id = _id;
		loc = _loc;
		type = NodeType::Pickup; //default TODO: maybe add an unknown?
		pair = NO_NODE;
		demand = 0;	
	}
	
	Node(const int _id, const GeoLocation _loc, const NodeType _type){
		id = _id;
		loc = _loc;
		type = _type;
		pair = NO_NODE;
		demand = 0;	
	}

	Node(const int _id, const GeoLocation _loc, const int _dem, const double _etw, const double _ltw, const double _stw, const int _pair, const NodeType _type){
		id = _id;
		loc = _loc;
		demand = _dem;
		etw = _etw;
		ltw = _ltw;
		stw = _stw;
		pair = _pair;
		type = _type;		
	}
};

#endif //__H_NODE_H__

