#include "../include/instance.hpp"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip> // setprecision
#include <sstream> // stringstream

//TODO: this probably requires more commenting as well

using namespace std;

void write_edge_matrix(const std::vector< std::vector<double> >& matrix, ofstream& file);
void write_pdptw_instance(const Instance& inst, ofstream& file);
void write_vrptw_instance(const Instance& inst, ofstream& file);
void write_cvrp_instance(const Instance& inst, ofstream& file);

// convert double value to string according to precision
static std::string dtos(double value, unsigned int precision=2){
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}

FileCode Instance::write_instance_file(const std::string& filename, const Instance& inst){
	ofstream file(filename);
	if(not file.is_open()) return FileCode::WriteError;

	if(inst.type == InstanceType::PDPTW) write_pdptw_instance(inst, file);
	else if(inst.type == InstanceType::CVRP) write_cvrp_instance(inst, file);
    else if(inst.type == InstanceType::VRPTW) write_vrptw_instance(inst, file);
	
	return FileCode::WriteOk;
}

void write_pdptw_instance(const Instance& inst, ofstream& file){		
	file << "NAME: " << inst.name << endl;
	if(inst.location.size() > 0) file << "LOCATION: " << inst.location << endl;
	file << "COMMENT: " << inst.comment << endl;
	file << "TYPE: " << type_to_string(inst.type) << endl;
	file << "SIZE: " << inst.size << endl;
	file << "DISTRIBUTION: " << type_to_string(inst.dist_type);
	if(inst.dist_type != NodeDistribution::Random)
		file << " (" << inst.nclusters << " / " << inst.density << ")";
	file << endl;
	file << "DEPOT: " << type_to_string(inst.depot) << endl;
	file << "ROUTE-TIME: " << inst.max_route_time << endl;
	file << "TIME-WINDOW: " << inst.time_window << endl;
	file << "CAPACITY: " << inst.max_capacity << endl;
	file << "NODES" << endl;

	for(size_t i=0;i<inst.nodes.size();i++){
		file << i << " ";
		file << dtos(inst.nodes[i].loc.lat, 8) << " " << dtos(inst.nodes[i].loc.lon,8) << " ";
		file << inst.nodes[i].demand << " ";
		file << floor(inst.nodes[i].etw) << " " << floor(inst.nodes[i].ltw) << " " << inst.nodes[i].stw << " ";
		//printf("%ld %d %d %d\n", i, inst.nodes[i].type == NodeType::Pickup, inst.nodes[i].type == NodeType::Delivery, inst.nodes[i].type == NodeType::Depot);
		int p = inst.nodes[i].type == NodeType::Pickup ? 0 : inst.nodes[i].pair;
		int d = inst.nodes[i].type == NodeType::Delivery ? 0 : inst.nodes[i].pair;
		file << p << " " << d << endl;
	}

	write_edge_matrix(inst.matrix, file);

	file << "EOF";
}


void write_vrptw_instance(const Instance& inst, ofstream& file){
	file << "NAME: " << inst.name << endl;
	if(inst.location.size() > 0) file << "LOCATION: " << inst.location << endl;
	file << "COMMENT: " << inst.comment << endl;
	file << "TYPE: " << type_to_string(inst.type) << endl;
	file << "SIZE: " << inst.size << endl;
	file << "DISTRIBUTION: " << type_to_string(inst.dist_type);
	if(inst.dist_type != NodeDistribution::Random)
		file << " (" << inst.nclusters << " / " << inst.density << ")";
	file << endl;
	file << "DEPOT: " << type_to_string(inst.depot) << endl;
	file << "ROUTE-TIME: " << inst.max_route_time << endl;
	file << "TIME-WINDOW: " << inst.time_window << endl;
	file << "CAPACITY: " << inst.max_capacity << endl;
	file << "NODES" << endl;

	for(size_t i=0;i<inst.nodes.size();i++){
		file << i << " ";
		file << dtos(inst.nodes[i].loc.lat, 8) << " " << dtos(inst.nodes[i].loc.lon,8) << " ";
		file << inst.nodes[i].demand << " ";
		file << floor(inst.nodes[i].etw) << " " << floor(inst.nodes[i].ltw) << " " << inst.nodes[i].stw << " ";
		file << endl;
	}

	write_edge_matrix(inst.matrix, file);

	file << "EOF";
}


void write_cvrp_instance(const Instance& inst, ofstream& file){	
	file << "NAME: " << inst.name << endl;
	if(inst.location.size() > 0) file << "LOCATION: " << inst.location << endl;
	file << "COMMENT: " << inst.comment << endl;
	file << "TYPE: " << type_to_string(inst.type) << endl;
	file << "SIZE: " << inst.size << endl;
	file << "DISTRIBUTION: " << type_to_string(inst.dist_type);
	if(inst.dist_type != NodeDistribution::Random)
		file << " (" << inst.nclusters << " / " << inst.density << ")";
	file << endl;
	file << "DEPOT: " << type_to_string(inst.depot) << endl;
	file << "CAPACITY: " << inst.max_capacity << endl;
	file << "NODES" << endl;

	for(size_t i=0;i<inst.nodes.size();i++){
		file << i << " ";
		file << dtos(inst.nodes[i].loc.lat, 8) << " " << dtos(inst.nodes[i].loc.lon,8) << " ";
		file << inst.nodes[i].demand << endl;
	}

	write_edge_matrix(inst.matrix, file);
	
	file << "EOF";
}

void write_edge_matrix(const std::vector< std::vector<double> >& matrix, ofstream& file){
	file << "EDGES" << endl;
	for(size_t i=0;i<matrix.size();i++){
		for(size_t j=0;j<matrix.size();j++){
			file << static_cast<size_t>(matrix[i][j]);
			if(j != matrix.size()-1) file << " ";
		}
		file << endl;
	}
}
