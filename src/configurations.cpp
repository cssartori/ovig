#include "../include/configurations.hpp"
#include "../include/constants.hpp"
#include <iostream>
#include <fstream>

typedef struct {
	bool name=false, type=false, size=false, dist=false, clusters=false, density=false, route_time=false, capacity=false, depot=false, time_window=false, service=false, osrm=false, land=false, limits=false, nodes=false;
}checker_t;

bool has_everything(const checker_t& ch, const Configurations& con);

//read keywords from the stream (separated by spaces)
std::string read_keyword(std::istream& f) {
	std::string keyword;
	if (f >> keyword)
		return keyword;
	else
		throw Constants::STR_EOF;
}

//count words separated by a single space
//used to differentiate between full nodes or only locations in the context of the PDPTW
size_t count_words(const std::string text){
	size_t count = 0;
	bool in_spaces = false;
	for(size_t i=0;i<text.size();i++){
		if(text[i] == ' ' && not in_spaces){
			count++;
			in_spaces = true;
		}else{
			in_spaces = false;
		}
	}
	return count+1;	
}

//Reads the node file containing either locations or full nodes of the problem
//TODO: This should probably be changed for other variants of the VRP
FileCode read_node_file(struct Configurations& con, std::istream& f){
	std::string cline;
	std::string keyword;
	bool found_nodes_tag = false;
	bool found_eof = false;
	if(con.verbose >= 2)
		printf("Reading header of %s...\n", Constants::STR_CON_NODE_FILE.c_str());
	
	//ignore comments at the beggining
	while(not f.eof() and not found_nodes_tag){
		keyword = read_keyword(f);
		if(keyword == Constants::STR_EOF){
			found_eof = true;
			break; //end of the file
		}else if(keyword[0] == Constants::CHR_COMMENT){
			//commentary just ignore it
			getline(f,cline);
		}else if(keyword == Constants::STR_NDF_NODES){
			found_nodes_tag = true;
		}else{
			printf("Unkown tag \"%s\" in %s\n", keyword.c_str(), Constants::STR_CON_NODE_FILE.c_str());
			return FileCode::ReadError;
		}
	}
	if(con.verbose >= 2)
		printf("Reading nodes from file...\n");
	
	if(not found_nodes_tag || f.eof() || found_eof){
		printf("Missing tag %s in %s\n", Constants::STR_NDF_NODES.c_str(), Constants::STR_CON_NODE_FILE.c_str());
		return FileCode::ReadError;
	}
	
	std::streampos old_pos = f.tellg(); //get current position
	getline(f,cline); //remove the \n
	getline(f,cline); //get line of node
	size_t words = count_words(cline); //count number of words separated by space
	f.seekg(old_pos);

	if(words != 3 and words != 7){
		printf("Nodes should be define either with only < id lat lon >, or with < id lat lon dem tw_center pick dev >");
		return FileCode::ReadError;
	}
	
	con.full_nodes = words > 3 ? Constants::NUM_NDF_FULL : Constants::NUM_NDF_LOCS;
	int id=0;
	while(not f.eof()){
		std::string bl; //beginning of line
		f >> bl;
		if(bl == Constants::STR_EOF){
			found_eof=true;
			break; //end of the file
		}else if(bl[0] == Constants::CHR_COMMENT){
			//commentary just ignore it
			getline(f,cline);
			continue;
		}
		
		int q=0, tw_center=0, pick=0, dev=0;
		double lat=0, lon=0;
		f >> lat >> lon;
		if(con.full_nodes == Constants::NUM_NDF_FULL)
			f >> q >> tw_center >> pick >> dev;
		NodeType type = NodeType::Depot;
		//TODO: in the file, pickup and delivery should be one after the other!!
		if(dev > 0){
			pick=id+1;
			type = NodeType::Pickup;
		}else if(pick > 0){
			pick = id-1;
			type = NodeType::Delivery;
		}
		
		GeoLocation loc = {lat,lon};
		Node n(id, loc, q, tw_center, tw_center, 0, pick, type);
		con.nodes.push_back(n);
		id++;
	}
	if(con.verbose >= 2)
		printf("DONE: Nodes read.\n");
	if(not found_eof){
		printf("Missing tag %s in %s\n", Constants::STR_EOF.c_str(), Constants::STR_CON_NODE_FILE.c_str());
		return FileCode::ReadError;
	}
	
	return FileCode::ReadOk;
}

//read the distribution type of locations in the space
FileCode read_distribution(struct Configurations& con, std::istream& f){
	std::string dummy;
	f >> dummy;
	if(dummy == Constants::STR_DIST_CLUSTER) con.dist_type = NodeDistribution::Cluster;
	else if(dummy == Constants::STR_DIST_RANDOM) con.dist_type = NodeDistribution::Random;
	else if(dummy == Constants::STR_DIST_CLUST_RAND) con.dist_type = NodeDistribution::ClusterRandom;
	else{
		printf("Unknown distribution type \"%s\"\n", dummy.c_str());
		return FileCode::ReadError;
	}
	return FileCode::ReadOk;
}

//read the limits where to create the locations (only used when GENERATING the locations completely at random)
FileCode read_limits(struct Configurations& con, std::istream& f){
	std::string dummy;
	con.limits = std::vector<double>(4,0);
	int l=0;
	//read 4 sides of the rectangle
	while(l < 4){
		f >> dummy;
		char dir = dummy[0];
		dummy = dummy.substr(2,dummy.length());
		if(dir == 'N') con .limits[LNORTH] = atof(dummy.c_str());
		else if(dir == 'S') con .limits[LSOUTH] = atof(dummy.c_str());
		else if(dir == 'E') con .limits[LEAST] = atof(dummy.c_str());
		else if(dir == 'W') con .limits[LWEST] = atof(dummy.c_str());
		else{ //neither N,S,E,W , then it is an error
			printf("Unknown direction \"%c\"\n", dir);
			return FileCode::ReadError;
		}
		l++;
	}
	return FileCode::ReadOk;
}

//read the type of depot (for now, either central or random position)
FileCode read_depot(struct Configurations& con, std::istream& f){
	std::string dummy;
	FileCode result = FileCode::ReadOk;
	
	f >> dummy;
	if(dummy == Constants::STR_DEPOT_CENTRAL)
		con.depot_type = DepotType::Central;
	else if(dummy == Constants::STR_DEPOT_RANDOM)
		con.depot_type = DepotType::Random;
	else{
		printf("Unknown depot type \"%s\"\n", dummy.c_str());
		result = FileCode::ReadError;
	}
	return result;
}

//read the type of problem to be generated (for now, either PDPTW or CVRP)
FileCode read_type(struct Configurations& con, std::istream& f){
	std::string dummy;
	FileCode result = FileCode::ReadOk;
	
	f >> dummy;
	con.type = string_to_type1(dummy);
	if(con.type != InstanceType::PDPTW and con.type != InstanceType::CVRP and con.type != InstanceType::VRPTW){
		printf("Unknown instance type \"%s\"\n", dummy.c_str());
		result = FileCode::ReadError;
	}

	return result;
}


//read the specification area of the input stream
//check if required fields have been given
FileCode Configurations::read_configurations(struct Configurations& con, std::istream& f){
	checker_t ch = {false};
	con.num_points = 0;
	con.full_nodes = Constants::NUM_NDF_NONE;
	std::string dummy;
	std::string keyword;
	FileCode result;

	do {
		result = FileCode::ReadOk;
		keyword = read_keyword(f);

		if(keyword[0] == Constants::CHR_COMMENT){
			//commentary just ignore it
			getline(f, dummy);
			continue;
		}else if(keyword == Constants::STR_EOF){
			//end of file
			break;
		}else if(keyword[keyword.length()-1] == Constants::CHR_SEPARATOR) {
			keyword = keyword.substr(0,keyword.length()-1);
		}else{
			printf("Missing separator '%c' after tag %s\n", Constants::CHR_SEPARATOR, keyword.c_str());
			result = FileCode::ReadError;
		}
		if(con.verbose >= 2)
			printf("Reading %s\n", keyword.c_str());
		
		if(keyword == Constants::STR_CON_NAME){
			f >> con.iname; ch.name = true;
		}else if(keyword == Constants::STR_CON_TYPE){
			result = read_type(con, f); ch.type = true;
		}else if(keyword == Constants::STR_CON_COMMENT){
			getline(f, con.comment);
		}else if(keyword == Constants::STR_CON_SIZE){
			f >> con.num_points; ch.size = true;
		}else if(keyword == Constants::STR_CON_DIST){
			result = read_distribution(con, f); ch.dist = true;
		}else if(keyword == Constants::STR_CON_DEPOT){
			result = read_depot(con, f); ch.depot = true;
		}else if(keyword == Constants::STR_CON_DENSITY){
			f >> con.density; ch.density = true;
		}else if(keyword == Constants::STR_CON_CLUSTERS){
			f >> con.nclusters; ch.clusters = true;
		}else if(keyword == Constants::STR_CON_TIME_WINDOW){
			f >> con.time_window; ch.time_window = true;
		}else if(keyword == Constants::STR_CON_SERV_TIME){
			f >> con.service_time; ch.service = true;
		}else if(keyword == Constants::STR_CON_CAPACITY){
			f >> con.capacity; ch.capacity = true;
		}else if(keyword == Constants::STR_CON_SEED){
			f >> con.seed;
		}else if(keyword == Constants::STR_CON_OSRM_FILE){
			f >> con.osrm_file; ch.osrm = true;
		}else if(keyword == Constants::STR_CON_LAND_FILE){
			f >> con.land_file; ch.land = true;
		}else if(keyword == Constants::STR_CON_LOCATION){
			f >> con.locname; 
		}else if(keyword == Constants::STR_CON_ROUTE_TIME){
			f >> con.max_route_time; ch.route_time = true;
		}else if(keyword == Constants::STR_CON_NODE_FILE){
			f >> con.node_file; ch.nodes = true;
			std::ifstream nfile(con.node_file);
			if(not nfile.is_open()){
				printf("Could not open file \"%s\"\n", con.node_file.c_str());
				result = FileCode::ReadError;
			}else{
				result = read_node_file(con, nfile);
				nfile.close();
			}
		}else if(keyword == Constants::STR_CON_LIMITS){
			result = read_limits(con, f); ch.limits = true;
		}else {
			printf("Unkown configuration tag \"%s\"\n", keyword.c_str());
			result = FileCode::ReadError;
		}
		if(con.verbose >= 2)
			printf("DONE: Reading %s\n", keyword.c_str());
		
	} while (result == FileCode::ReadOk);

	if(result == FileCode::ReadOk) result = has_everything(ch, con) ? result : FileCode::ReadError;
	
	return result;
}

//checks if everything that is required has been read and warns if something should have been informed
bool has_everything(const checker_t& ch, const Configurations& con){
	std::string misst;
	bool required = false;
	if(not ch.name){
		misst = Constants::STR_CON_NAME;
		required = true;
	}else if(not ch.type){
		misst = Constants::STR_CON_TYPE;
		required = true;
	}else if(not ch.size){
		misst = Constants::STR_CON_SIZE;
		required = true;
	}else if(not ch.dist){
		misst = Constants::STR_CON_DIST;
		required = true;
	}else if((con.dist_type == NodeDistribution::Cluster or con.dist_type == NodeDistribution::ClusterRandom) and not ch.clusters){
		misst = Constants::STR_CON_CLUSTERS;
		required = true;
	}else if((con.dist_type == NodeDistribution::Cluster or con.dist_type == NodeDistribution::ClusterRandom) and not ch.density){
		misst = Constants::STR_CON_DENSITY;
		//not required
	}else if(not ch.route_time){
		if(con.type != InstanceType::CVRP){
			misst = Constants::STR_CON_ROUTE_TIME;
			required = true;
		}
	}else if(not ch.capacity){
		misst = Constants::STR_CON_CAPACITY;
		required = true;
	}else if(not ch.depot){
		misst = Constants::STR_CON_DEPOT;
		required = true;
	}else if(not ch.time_window){
		if(con.type != InstanceType::CVRP){
			misst = Constants::STR_CON_TIME_WINDOW;
			required = true;
		}
	}else if(not ch.service){
		if(con.type != InstanceType::CVRP){
			misst = Constants::STR_CON_SERV_TIME;
			required = true;
		}
	}else if(not ch.osrm){
		misst = Constants::STR_CON_OSRM_FILE;
		required = true;
	}else if(not ch.nodes and not ch.land){
		misst = Constants::STR_CON_LAND_FILE;
		//not required
	}else if(not ch.nodes and not ch.limits){
		misst = Constants::STR_CON_LIMITS;
		required = true;
	}

	if(misst.size() > 0){
		if(required) printf("Missing ");
		else printf("WARNING: You possibly forgot to include ");
		printf("%s in configuration file\n", misst.c_str());
	}

	return not required;
}
