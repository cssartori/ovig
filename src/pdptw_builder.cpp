#include "../include/builder.hpp"
#include <algorithm>

using namespace std;

/*
 * A builder for the PDPTW instances and all related functions that are required!
 * TODO: more comments
 */

namespace Builder{
	namespace PDPTW{
		void clusterize(std::vector<Node>& nodes, const size_t k);
		void pair_random_nodes(std::vector<Node>& nodes);
		void service_time_nodes(std::vector<Node>& nodes, const size_t st);
		void demand_random_nodes(const Configurations& con, std::vector<Node>& nodes);
		void time_window_random_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes);
		void time_window_full_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes);
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
			if(con.dist_type == NodeDistribution::Cluster)
				clusterize(nodes, con.nclusters);
			else
				pair_random_nodes(nodes);

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
			order_nodes(nodes, matrix);
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


		int build(const Configurations& con, std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix, Instance& inst){
			if(con.verbose >= 2) printf("Setting depot information...\n");
	
			nodes[0].id = 0;
			nodes[0].etw = 0; nodes[0].ltw = con.max_route_time;
			nodes[0].stw = 0; nodes[0].demand = 0; nodes[0].pair = 0;

			// for(size_t i =0;i<nodes.size();i++){
			// 	matrix[0][i] = 0;
			// 	matrix[i][0] = 0;
			// }

			if(con.verbose >= 2) printf("Setting service times, time windows and reordering nodes...\n");
			service_time_nodes(nodes, con.service_time);
			time_window_full_nodes(con, matrix, nodes);
			order_nodes(nodes, matrix);
			if(con.verbose >= 2){
				printf("DONE: Set service times, time windows and reordered nodes.\n");
				printf("Checking feasibility of instance...\n");
			}
			if(not check_tw_feasibility(nodes, matrix)){
				printf("WARNING: Instance is not feasible due to Time Window constraints\n");
				return EXIT_FAILURE;
			}
			if(con.verbose >= 2)
				printf("DONE: Instance is feasible.\n");
	
			inst.nodes = nodes;
			inst.matrix = matrix;
			return EXIT_SUCCESS;
		}

		bool check_tw_feasibility(const std::vector<Node>& nodes, const std::vector< std::vector< double > >& matrix){
			for(size_t i=1;i<nodes.size();i++){
				if(nodes[i].type == NodeType::Delivery) continue;
				int del = nodes[i].pair;
				double time = matrix[0][i];
				if(time > nodes[i].ltw){
					printf("Time to reach %ld is %.2f which is larger than LTW: %.2f ( path 0 -> %ld )\n", i, time, nodes[i].ltw, i);
					//printf("%d : %f %f | %f\n%d : %f %f\n", i, nodes[i].etw, nodes[i].ltw, matrix[0][i], nodes[i].pair, nodes[nodes[i].pair].etw, nodes[nodes[i].pair].ltw);
					return false;
				}
				time = std::max(nodes[i].etw, time) + nodes[i].stw + matrix[i][del];
				if(time > nodes[del].ltw){
					printf("Time to reach %d is %.2f which is larger than LTW: %.2f ( path 0 -> %ld -> %d )\n", del, time, nodes[del].ltw, i, del);
					//printf("T = %f | %f,%f %f,%f\n", matrix[i][del], nodes[i].loc.lat, nodes[i].loc.lon, nodes[del].loc.lat, nodes[del].loc.lon);//GeoLocation::haversine_distance(nodes[i].loc.to_rad(), nodes[del].loc.to_rad()));
					return false;
				}
				time = std::max(nodes[del].etw, time) + nodes[del].stw + matrix[del][0];
				if(time > nodes[0].ltw){
					printf("Time to reach 0 is %.2f which is larger than LTW: %.2f ( path 0 -> %ld -> %d -> 0)\n", time, nodes[0].ltw, i, del);
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
			std::uniform_int_distribution<int> rdemand(10,0.6*con.capacity);
			nodes[0].demand = 0;
			for(size_t id=1;id<nodes.size();id++){
				if(nodes[id].type == NodeType::Pickup){
					nodes[id].demand = rdemand(rng);
					nodes[nodes[id].pair].demand = -nodes[id].demand;
				}
			}
		}

		void time_window_full_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes){
			double width = static_cast<double>(con.time_window)/2;
			for(size_t i=1;i<nodes.size();i++){
				double t0i = matrix[0][i];
				Node& nd = nodes[i];
				double center = nd.etw;
				if(t0i > center-width){
					center = t0i+width;
					nodes[nd.pair].etw += (center-nd.etw);
				}
				nd.etw = std::max(0.0, center-width);
				nd.ltw = std::min(static_cast<double>(con.max_route_time), center+width);
			}
		}

		void time_window_random_nodes(const Configurations& con, const std::vector< std::vector< double > >& matrix, std::vector<Node>& nodes){
			vector<int> ns;
			for(int id=1;id < static_cast<int>(nodes.size()) ; id++)
				if(nodes[id].type == NodeType::Pickup)
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
				int d = nodes[p].pair;
				double t0p = matrix[0][p];
				double tpd = matrix[p][d];
				double td0 = matrix[d][0];
		
				std::uniform_int_distribution<size_t> ucenter(t0p, (max_route_time-tpd-td0-2*nodes[p].stw));
				if(t0p > max_route_time-tpd-td0-2*nodes[p].stw){
					printf("Bad time window center generator for node %d: UD[ %.2f , %.2f ]\n", p, t0p, (max_route_time-tpd-td0-2*nodes[p].stw-con.time_window/2));
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
		
				if(prob(rng) >= 0.9 and nodes[p].ltw < (max_route_time-tpd-td0-2*nodes[p].stw-con.time_window)){
					//time windows without overlap
					b++;
					nodes[d].etw =  nodes[p].ltw + tpd + nodes[p].stw; 
					nodes[d].ltw = std::min(nodes[d].etw + con.time_window, max_route_time-td0);
				}else{
					//time window with overlap
					nodes[d].etw =  nodes[p].etw + tpd + nodes[p].stw + std::max(0.0, tpd*radd(rng)); 
					nodes[d].ltw = std::min(nodes[d].etw + con.time_window, max_route_time-td0);			
				}
				
			}

			//for the remaining nodes with no time window
			for(size_t i=nntw;i<ns.size();i++){
				nodes[ns[i]].etw = 0.0;
				nodes[ns[i]].ltw = max_route_time;
				nodes[nodes[ns[i]].pair].etw = 0.0;
				nodes[nodes[ns[i]].pair].ltw = max_route_time;
			}

			// double leastt = max_route_time;
			// size_t a=0;
			// //for the remaining nodes with no time window
			// for(size_t i=0;i<nodes.size();i++){
			// 	if(nodes[i].ltw - nodes[i].etw < leastt) leastt = nodes[i].ltw - nodes[i].etw;
			// 	if(nodes[i].ltw - nodes[i].etw < con.time_window) a++;
			// }
			// printf("least tw: %f | a = %ld | b = %ld\n", leastt, a, b);
	
		}


		void pair_random_nodes(std::vector<Node>& nodes){
			vector<int> ns; // list of node ids

			for(int id=1;id < static_cast<int>(nodes.size()) ; id++) ns.push_back(id);
	
			while(ns.size() > 0){ //assuming |nodes|-1 is even
				int pi = ns[0]; // index of the pickup
				int k = (rng()%static_cast<int>(ns.size()-1))+1;
				int di = ns[k]; // index of the delivery
	
				nodes[pi].pair = di; nodes[pi].type = NodeType::Pickup;
				nodes[di].pair = pi; nodes[di].type = NodeType::Delivery;
		
				ns.erase(ns.begin()+k);
				ns.erase(ns.begin()+0);
			}

			nodes[0].pair = 0;
		}

		void clusterize(std::vector<Node>& nodes, const size_t k){
			std::vector<GeoLocation> means;
			for(size_t i=0;i<k;i++) means.push_back(nodes[i+1].loc);
	
			std::vector< size_t > clusters(nodes.size(), k+1);
			for(size_t c=1;c<=k;c++) clusters[c] = c-1;
	
			std::vector<size_t> sizes(k,0);	
			bool improved = true;
			int iters = 0;
			while(improved){
				iters++;
				improved = false;
				for(size_t c=0;c<k;c++) sizes[c] = 0; //reset clusters sizes
		
				for(size_t i=1;i<nodes.size();i++){//the depot does not count
					size_t cl = 0;
					double minhd = GeoLocation::haversine_distance(means[0].to_rad(), nodes[i].loc.to_rad());
					for(size_t c=1;c<k;c++){
						double hd = GeoLocation::haversine_distance(means[c].to_rad(), nodes[i].loc.to_rad());
						if(minhd > hd){
							minhd = hd; cl = c;
						}
					}
					if(clusters[i] != cl) improved=true;
			
					clusters[i] = cl;
					sizes[cl]++;
				}
		
				//compute new means
				for(size_t i=0;i<k;i++) means[i] = {0.0,0.0}; //reset means
				//take the averages of all locations in each cluster
				for(size_t i=1;i<nodes.size();i++){
					size_t c = clusters[i];
					means[c].lat += nodes[i].loc.lat/sizes[c];
					means[c].lon += nodes[i].loc.lon/sizes[c];
				}

				//for(size_t i=0;i<k;i++) means[c] = {means[c].lat/sizes[i,0.0};
			}
	
			bool all_even;//we need all clusters with an even number of nodes for pairing
	
			//here we use (for now?) a greedy strategy to guarantee that
			do{
				all_even = true;
				size_t min_size_i=k+1;
				for(size_t i=0;i<k;i++){
					if(sizes[i]%2 == 1){
						all_even = false; //there is at least one (actually two!) with odd number of nodes
						min_size_i = (min_size_i == k+1 || sizes[min_size_i] > sizes[i]) ? i : min_size_i;
					}
				}

				if(not all_even){
					//get closest odd number cluster
					size_t min_dist_i = k+1;
					for(size_t i=0;i<k;i++){
						if(i != min_size_i && sizes[i]%2 == 1){
							if(min_dist_i == k+1) min_dist_i = i;
							else if(GeoLocation::haversine_distance(means[min_size_i].to_rad(), means[min_dist_i].to_rad()) >
									GeoLocation::haversine_distance(means[min_size_i].to_rad(), means[i].to_rad()))
								min_dist_i = i;
						}
					}

					//get the location in min_dist_i cluster that is closest to the cluster in min_size_i
					size_t close_loc = nodes.size()+1;
					for(size_t n=1;n<nodes.size();n++){
						if(clusters[n] == min_dist_i){
							if(close_loc == nodes.size()+1) close_loc=n;
							else if(GeoLocation::haversine_distance(means[min_size_i].to_rad(), nodes[close_loc].loc.to_rad()) >
									GeoLocation::haversine_distance(means[min_size_i].to_rad(), nodes[n].loc.to_rad()))
								close_loc = n; 
						}
					}

					sizes[min_dist_i]--;
					sizes[min_size_i]++;
					clusters[close_loc] = min_size_i;
				}	
			}while(not all_even);

			/*for(size_t c=0;c<k;c++){
			  printf("Cluster %ld: %ld | mean: %f , %f\n", c, sizes[c], means[c].lat, means[c].lon);
			  }*/
	
			//printf("Setting pairs within clusters\n");
			for(size_t c=0;c<k;c++){
				//printf("Cluster %d\n", c);
				vector<size_t> clvec;
				for(size_t i=1;i<nodes.size();i++){
					if(clusters[i] == c){
						clvec.push_back(i);
					}
				}
				//printf("shuffling nodes (%ld)\n", clvec.size());
				//shuffle nodes to reorder with minimum bias
				shuffle(clvec.begin(), clvec.end(), rng);
				size_t count = 0;
				size_t r = 0;
				while(count < clvec.size()){
					// size_t s = rng()%(clvec.size()-r)+r;
					// if(r == s || clvec[r] == clvec.size()+1 || clvec[s] == clvec.size()+1){
					// 	r = (r+1)%clvec.size();
					// 	continue;
					// }
					//printf("r %ld | s %ld\n", r, s);
					// nodes[clvec[r]].pair = clvec[s];
					// nodes[clvec[r]].type = NodeType::Pickup;
					// nodes[clvec[s]].pair = clvec[r];
					// nodes[clvec[s]].type = NodeType::Delivery;
					// count+=2;
					// clvec[r] = clvec.size()+1;
					// clvec[s] = clvec.size()+1;
					// r = (r+1)%clvec.size();
					size_t s = r+1;
					nodes[clvec[r]].pair = clvec[s];
					nodes[clvec[r]].type = NodeType::Pickup;
					nodes[clvec[s]].pair = clvec[r];
					nodes[clvec[s]].type = NodeType::Delivery;
					count+=2;
					r+=2;
				}
			}
			//printf("Done pairing...\n");
		}

		//order nodes so that pickup location i has delivery n+i
		void order_nodes(std::vector<Node>& nodes, std::vector< std::vector< double > >& matrix){
			for(size_t i=1;i<nodes.size();i++){
				if(nodes[i].demand < 0) nodes[i].type = NodeType::Delivery;
				else if(nodes[i].demand > 0) nodes[i].type = NodeType::Pickup;
				else nodes[i].type = NodeType::Depot;
			}
			vector<Node> ntemp = nodes;
			vector<int> moves(nodes.size(), -1);

			int idx = 1; //index of the current pickup (0 is always the depot)
			int jt = 1;
			int n = (nodes.size()-1)/2;
			while(idx < n+1){
				while(ntemp[jt].type != NodeType::Pickup) jt++;
				nodes[idx] = ntemp[jt];
				moves[jt] = idx;
				nodes[idx].pair = idx+n;
				nodes[idx+n] = ntemp[ntemp[jt].pair];
				moves[ntemp[jt].pair] = idx+n;
				nodes[idx+n].pair = idx;
				nodes[idx].id = idx;
				nodes[idx+n].id = idx+n;
				idx++;
				jt++;
			}
			// size_t found = 0;
			// vector<bool> tested(nodes.size(), false);
			// for(size_t i=0;i<nodes.size();i++){
			// 	for(size_t j=0;j<nodes.size();j++){
			// 		if(not tested[j] && nodes[i].loc.lat == ntemp[j].loc.lat
			// 		   && nodes[i].loc.lat == ntemp[j].loc.lat && nodes[i].demand == ntemp[j].demand){
			// 			found++;
			// 			tested[j] = true;
			// 			break;
			// 		}
			// 	}
			// }
			//printf("found = %ld | %ld\n", found, nodes.size());
			// for(size_t i=0;i<nodes.size();i++){
			// 	printf("%li : %i\n", i, moves[i]);
			// }
	
			int old_index = 1;
			int new_index = moves[old_index];
			size_t orders = 0;
			vector<double> row = matrix[old_index];
			vector<double> col(nodes.size(), 0.0);
			vector<double> row2;
			vector<double> col2(nodes.size(), 0.0);
			for(size_t i=0;i<nodes.size();i++) col[i] = matrix[i][old_index];

			vector<double> nto_dist(nodes.size(), 0.0); //new to old
			vector<double> otn_dist(nodes.size(), 0.0); //old to new

			for(size_t i=1;i<nodes.size();i++){
				nto_dist[moves[i]] = matrix[moves[i]][i];
				otn_dist[i] = matrix[i][moves[i]];
			}
	
			do{
				if(old_index == new_index || moves[old_index] == -1){
					if(moves[old_index] > 0){
						moves[old_index] = -1;
						orders++;
					}
					old_index = (old_index+1) % nodes.size();
					new_index = moves[old_index];
					row = matrix[old_index];
					for(size_t i=0;i<nodes.size();i++) col[i] = matrix[i][old_index];
				}else{
					row2 = matrix[new_index];
					for(size_t i=0;i<nodes.size();i++) col2[i] = matrix[i][new_index];
			
					row2[new_index] = nto_dist[new_index];
					col2[new_index] = otn_dist[old_index];
			
					matrix[new_index] = row;
					for(size_t i=0;i<nodes.size();i++) matrix[i][new_index] = col[i];
					matrix[new_index][new_index] = 0;

					moves[old_index] = -1;
					old_index = new_index;
					new_index = moves[old_index];
					row = row2;
					col = col2;
					orders++;
				}

			}while(orders < nodes.size()-1);

			nodes[0].pair = 0;
		}

	};
};

