#include "../include/builder.hpp"
#include <algorithm>

using namespace std;

/*
 * A builder for the VRPTW instances and all related functions that are required!
 */

namespace Builder{
	namespace VRPTW{

		void service_time_nodes(std::vector<Node>& nodes, const size_t st);
		void demand_random_nodes(const Configurations& con, std::vector<Node>& nodes);
		void time_window_random_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes);
		bool check_tw_feasibility(const std::vector<Node>& nodes, const std::vector< std::vector< double > >& matrix);
		void order_nodes(std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix);

		int build(const Configurations& con, const std::vector<GeoLocation>& locations, std::vector< std::vector< double > >& matrix, Instance& inst){
			if(con.verbose >= 2) printf("Grouping nodes...\n");
			vector<Node> nodes;
			for(size_t id = 0; id < locations.size(); id++){
				// for now all of them are pickups. This will change when deciding which is which.
				nodes.push_back(Node(static_cast<int>(id), locations[id], NodeType::Pickup));
			}
			nodes[0].type = NodeType::Depot; // node 0 is always the depot

			if(con.verbose >= 2){
				printf("DONE: Nodes grouped.\n");
				printf("Pairing nodes...\n");
			}

			if(con.verbose >= 2){
				printf("DONE: Nodes paired...\n");
				printf("Setting service times, time windows and demands...\n");
			}

			service_time_nodes(nodes, con.service_time);
			time_window_random_nodes(con, matrix, nodes);
			demand_random_nodes(con, nodes);

			if(con.verbose >= 2){
				printf("DONE: Set service times, time windows and demands.\n");
				printf("Reordering nodes...\n");
			}

            if(con.verbose >= 2){
				printf("DONE: Nodes reordered.\n");
				printf("Checking feasibility of instance...\n");
			}

			if(not check_tw_feasibility(nodes, matrix)){
				printf("WARNING: Instance is not feasible due to Time Window constraints\n");
				return EXIT_FAILURE;
			}

			if(con.verbose >= 2) printf("DONE: Instance is feasible.\n");

			inst.nodes = nodes;
			inst.matrix = matrix;
			return EXIT_SUCCESS;
		}


		bool check_tw_feasibility(const std::vector<Node>& nodes, const std::vector< std::vector< double > >& matrix){
			for(size_t i=1;i<nodes.size();i++){
				double time = matrix[0][i];
				if(time > nodes[i].ltw){
					printf("Time to reach %ld is %.2f which is larger than LTW: %.2f ( path 0 -> %ld )\n", i, time, nodes[i].ltw, i);
					//printf("%d : %f %f | %f\n%d : %f %f\n", i, nodes[i].etw, nodes[i].ltw, matrix[0][i], nodes[i].pair, nodes[nodes[i].pair].etw, nodes[nodes[i].pair].ltw);
					return false;
				}

				time = std::max(nodes[i].etw, time) + nodes[i].stw + matrix[i][0];
				if(time > nodes[0].ltw){
					printf("Time to reach 0 is %.2f which is larger than LTW: %.2f ( path 0 -> %ld -> 0)\n", time, nodes[0].ltw, i);
					//printf("T = %f | %f,%f %f,%f\n", matrix[i][del], nodes[i].loc.lat, nodes[i].loc.lon, nodes[del].loc.lat, nodes[del].loc.lon);//GeoLocation::haversine_distance(nodes[i].loc.to_rad(), nodes[del].loc.to_rad()));
					return false;
				}
			}
			return true;
		}

		void service_time_nodes(std::vector<Node>& nodes, const size_t st){
			nodes[0].stw = 0;
			for(size_t id=1;id<nodes.size();id++){
				nodes[id].stw = st;
			}
		}

		void demand_random_nodes(const Configurations& con, std::vector<Node>& nodes){
			std::uniform_int_distribution<int> rdemand(1,50);
			nodes[0].demand = 0;
			for(size_t id=1;id<nodes.size();id++){
				if(nodes[id].type == NodeType::Pickup){
					nodes[id].demand = rdemand(rng);
				}
			}
		}

		void time_window_random_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes){
			vector<int> ns;
			for(int id=1;id < static_cast<int>(nodes.size()) ; id++)
                ns.push_back(id);

			//shuffle nodes to reorder with minimum bias
			shuffle(ns.begin(), ns.end(), rng);

			double p;
			std::uniform_real_distribution<double> percent;
			std::uniform_int_distribution<size_t> twidth; //width of the time windows

			percent = std::uniform_real_distribution<double>(0.85, 0.95); // selects from 5 to 15% requests that wont have time windows
			p = percent(rng); //percentage of requests that have a time window

			//depot settings
			nodes[0].etw = 0;
			nodes[0].ltw = con.max_route_time;

			double max_route_time = static_cast<double>(con.max_route_time);
			size_t nntw = ceil(p*ns.size());
			std::uniform_real_distribution<double> prob(0.0, 1.0);
			std::uniform_real_distribution<double> radd(-0.4, 0.4); //TODO: this should actually be just a {0,1} generation
			size_t b = 0;
			for(size_t i=0;i<nntw;i++){
				int p = ns[i];
				double t0p = matrix[0][p];
				double tp0 = matrix[p][0];

				std::uniform_int_distribution<size_t> ucenter(t0p, (max_route_time-tp0-nodes[p].stw));
				if(t0p > max_route_time-tp0-nodes[p].stw){
					printf("Bad time window center generator for node %d: UD[ %.2f , %.2f ]\n", p, t0p, (max_route_time-tp0-nodes[p].stw));
					//printf("0 -- %.1f --> %d -- %.1f --> %d -- %.1f --> 0 | %f\n", t0p, p, tpd, d, td0, nodes[p].stw);
					//printf("p: [%f %f]\n", nodes[p].etw, nodes[p].ltw);
					//printf("d: [%f %f]\n", nodes[d].etw, nodes[d].ltw);
					printf("This is usually caused because the maximum route time is not compatible with the rest of the configuration (e.g., too short)\n");
					exit(EXIT_FAILURE);
				}

				double center = ucenter(rng);
				size_t width = con.time_window/2;
				nodes[p].etw = std::max(0.0, center-width);
				nodes[p].ltw = std::min(center + width, max_route_time);

			}

			//for the remaining nodes with no time window
			for(size_t i=nntw;i<ns.size();i++){
				nodes[ns[i]].etw = 0.0;
				nodes[ns[i]].ltw = max_route_time;
			}
		}

	};
};

