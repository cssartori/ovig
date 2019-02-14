#include "../include/distributer.hpp"
#include <random>
#include <algorithm>

//TODO: should comment more this part of the code.

void map_to_rad_location(GeoLocation& loc){
	loc = loc.to_rad();
}

void map_to_rad_double(double& deg){
	deg = deg*M_PI/180.0;
}

bool Distributer::is_valid_location(GeoLocation loc, GenType gen_type, double cradius){
#ifdef __USE_GDAL_LAND_AREA__
	if(land_areas == NULL)
		return true;
	
	OGRLayer* layer = land_areas->GetLayer(0); // get first layer (usually there is only one). TODO: extend for more than one layer
	OGRSpatialReference* layer_ref = layer->GetSpatialRef();
	
	OGRSpatialReference wgs;
	wgs.importFromEPSG(4326);
	OGRCoordinateTransformation* coordTrans = OGRCreateCoordinateTransformation(&wgs, layer_ref);

	coordTrans->Transform(1, &loc.lon, &loc.lat);
	OGRPoint point(loc.lon, loc.lat);
	layer->SetSpatialFilter(&point);
	
	if(gen_type == GenType::Circle){
		// check radius with haversine formula. Reject sampling.
	}

	// the land area is valid if it has at least one feature (according to Geofabrik's shapefile defition)
	return (layer->GetFeatureCount() > 0);
#else
	return true;
#endif
	
}

// rectangle limits are in radians
GeoLocation random_location_rectangle(const std::vector<double>& limits){
	static std::uniform_real_distribution<double> rd01(0, 1);
	double r1 = rd01(rng), r2 = rd01(rng);
	
	GeoLocation loc;
	loc.lat = asin(r1*(sin(limits[LNORTH])-sin(limits[LSOUTH])) + sin(limits[LSOUTH]));
	double width = limits[LEAST] - limits[LWEST];
	width = width < 0 ? width + 2*M_PI : width;
	loc.lon = limits[LWEST] + width*r2;
	if(loc.lon < -M_PI) loc.lon += 2*M_PI;
	else if(loc.lon > M_PI) loc.lon -= 2*M_PI;

	return loc;
}

// computes the chance of point p being placed
// r is the clusterization proportion, the greater, the more clusterized
double cluster_prob(const std::vector<GeoLocation>& clusters, GeoLocation p, double r=1.0){
	double prb = 0.0;

	for(size_t s=0;s<clusters.size();s++){
		prb += exp(-(GeoLocation::haversine_distance(p,clusters[s]))*r); //TODO: check this
	}
	return prb;
}

//TODO: check whether this depot location is valid - if it is not do something!
GeoLocation generate_central_depot(std::vector<double> limits){
	return {(limits[LNORTH]+limits[LSOUTH])/2.0 , (limits[LWEST]+limits[LEAST])/2.0};
}

//TODO: check whether this depot location is valid - if it is not do something!
GeoLocation generate_random_depot(std::vector<double> limits){
	return random_location_rectangle(limits);
}

//selects the given locations that is closest to the exact middle of the locations previously selected (used)
GeoLocation select_central_depot(const std::vector<GeoLocation>& locs, std::vector<bool>& used){
	double max_lat=-180, min_lat=180, max_lon = -180, min_lon = 180;
	for(size_t i=0;i<locs.size();i++){
		if(not used[i]) continue;
		GeoLocation l = locs[i];
		max_lat = std::max(l.lat, max_lat);
		min_lat = std::min(l.lat, min_lat);
		max_lon = std::max(l.lon, max_lon);
		min_lon = std::min(l.lon, min_lon);
	}

	std::vector<double> limits(4);
	limits[LNORTH] = max_lat; limits[LSOUTH] = min_lat; limits[LEAST] = max_lon; limits[LWEST] = min_lon;
	GeoLocation c = generate_central_depot(limits);
	double mind = std::numeric_limits<double>::max(); //GeoLocation::haversine_distance(c.to_rad(), locs[0].to_rad()); //minimum distance to c
	size_t mli = 0; //location index of minimum distance to c
	for(size_t i=1;i<locs.size();i++){
		if(used[i]) continue;
		double d = GeoLocation::haversine_distance(c.to_rad(), locs[i].to_rad());
		if(d < mind){
			mind = d;
			mli = i;
		}
	}

	used[mli] = true;
	return locs[mli];
}

//selects a random depot location
GeoLocation select_random_depot(const std::vector<GeoLocation>& locs, std::vector<bool>& used){
	size_t r = rng()%locs.size();
	while(used[r]) r = rng()%locs.size();
	
	used[r] = true;
	return locs[r];
}

GeoLocation select_random_depot(const std::vector<Node>& nodes, std::vector<bool>& used){
	std::vector<GeoLocation> locs;
	for(Node n : nodes){
		locs.push_back(n.loc);
	}
	return select_random_depot(locs, used);
}

GeoLocation select_central_depot(const std::vector<Node>& nodes, std::vector<bool>& used){
	std::vector<GeoLocation> locs;
	for(Node n : nodes){
		locs.push_back(n.loc);
	}
	return select_central_depot(locs, used);
}

std::vector<GeoLocation> Distributer::select_locations(const std::vector<GeoLocation>& locs, const size_t num_points, const DepotType dptt, const NodeDistribution distribution, size_t nclusters, double r){
	std::vector<bool> used(locs.size(), false);
	std::vector<GeoLocation> selected;
	if(distribution == NodeDistribution::Cluster)
		selected = select_clustered_locations(locs, num_points-1, used, nclusters, r);
	else if(distribution == NodeDistribution::Random)
		selected = select_random_locations(locs, num_points-1, used);
	else
		selected = select_semi_clustered_locations(locs, num_points-1, used, nclusters, r);

	GeoLocation depot;
	if(dptt == DepotType::Central)
		depot = select_central_depot(locs, used);
	else
		depot = select_random_depot(locs, used);
	
	selected.insert(selected.begin(), depot);
	
	return selected;
}

std::vector<GeoLocation> Distributer::select_random_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used){
	std::vector<GeoLocation> selected;

	while(selected.size() < num_points){
		size_t s = rng()%locs.size();
		if(not used[s]){
			selected.push_back(locs[s]);
			used[s] = true;
		}
	}

	return selected;
}

std::vector<GeoLocation> Distributer::select_clustered_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used, size_t nclusters, double r){
	static std::uniform_real_distribution<double> prob(0, 1);
	//static std::uniform_int_distribution<size_t> seed_cluster(3, 10); //should this be dependent on the number of points?

	size_t num_clusters = nclusters;
	//num_clusters = num_clusters >= num_points ? num_points : num_clusters; //in case there are too few points

	std::vector<GeoLocation> clusters = select_random_locations(locs, num_clusters, used); // get seed locations
	std::vector<GeoLocation> selected = clusters;
	
	for_each(clusters.begin(), clusters.end(), map_to_rad_location);
		
	while(selected.size() < num_points){
		size_t s = rng()%locs.size();
		if(not used[s]){
			GeoLocation final = locs[s];
			double prf = cluster_prob(clusters, final.to_rad(), r);
			if(prob(rng) < prf){
				selected.push_back(final);
				used[s] = true;
			}
		}
	}
	
	return selected;
}

std::vector<GeoLocation> Distributer::select_semi_clustered_locations(const std::vector<GeoLocation> locs, const size_t num_points, std::vector<bool>& used, size_t nclusters, double r){
	static std::uniform_real_distribution<double> proportion(0.4, 0.6);
	const double cp = proportion(rng); //proportion of points that are clustered
	
	const size_t num_clustered_locations = std::ceil(cp*num_points);
	//printf("ceil(%.2f * %li) = %li\n", cp, num_points, ncluster);
	std::vector<GeoLocation> clusters = select_clustered_locations(locs, num_clustered_locations, used, nclusters, r);
	std::vector<GeoLocation> randoms = select_random_locations(locs, num_points-num_clustered_locations, used);
	
	std::vector<GeoLocation> locations = clusters;
	locations.insert(locations.end(), randoms.begin(), randoms.end());
	
	return locations;
}

std::vector<GeoLocation> Distributer::generate_clustered_locations(std::vector<double> limits, const size_t num_points, size_t nclusters, double r){
	static std::uniform_real_distribution<double> prob(0, 1);
	//static std::uniform_int_distribution<size_t> seed_cluster(3, 10); //should this be dependent on the number of points?
	size_t num_clusters = nclusters;
	//num_clusters = num_clusters >= num_points ? num_clusters*0.5 : num_clusters; //in case there are too few points
	std::vector<GeoLocation> clusters = generate_random_locations(limits, num_clusters); // creates seed locations

	std::vector<GeoLocation> locations = clusters;

	//transform each location in its equivalent in radiansx
	for_each(clusters.begin(), clusters.end(), map_to_rad_location);
	for_each(limits.begin(), limits.end(), map_to_rad_double);

	//generate one location at a time
	while(locations.size() < num_points){
		//generates a random location in the limited rectangle
		GeoLocation final = random_location_rectangle(limits);

		//check whether this location should be inserted - with a given probability
		//and if it is a valid location in the OSM file given
		double prf = cluster_prob(clusters, final, r);
		if(is_valid_location(final.to_deg()) && prob(rng) < prf){
			locations.push_back(final.to_deg());
		}
	}
	
	return locations;
	
}

std::vector<GeoLocation> Distributer::generate_random_locations(std::vector<double> limits, const size_t num_points){
	for_each(limits.begin(), limits.end(), map_to_rad_double);
	
	std::vector<GeoLocation> locations;
	while(locations.size() < num_points){
		GeoLocation final = random_location_rectangle(limits).to_deg();
		
		if(is_valid_location(final))
			locations.push_back(final);
	}
	
	return locations;
}

std::vector<GeoLocation> Distributer::generate_semi_clustered_locations(std::vector<double> limits, const size_t num_points, size_t nclusters, double r){
	static std::uniform_real_distribution<double> proportion(0.4, 0.6);
	const double cp = proportion(rng); //proportion of points that are clustered
	
	const size_t ncluster = std::ceil(cp*num_points);
	std::vector<GeoLocation> clusters = generate_clustered_locations(limits, ncluster, r);
	std::vector<GeoLocation> randoms = generate_random_locations(limits, num_points-ncluster);
	
	std::vector<GeoLocation> locations = clusters;
	locations.insert(locations.end(), randoms.begin(), randoms.end());
	
	return locations;	
}

std::vector<GeoLocation> Distributer::generate_locations(const std::vector<double>& limits, const size_t num_points, const DepotType dptt, const NodeDistribution distribution, size_t nclusters, double r){
	
	std::vector<GeoLocation> generated;
	if(distribution == NodeDistribution::Cluster)
		generated = generate_clustered_locations(limits, num_points-1, r);
	else if(distribution == NodeDistribution::Random)
		generated = generate_random_locations(limits, num_points-1);
	else
		generated = generate_semi_clustered_locations(limits, num_points-1, r);
	GeoLocation depot;
	if(dptt == DepotType::Central)
		depot = generate_central_depot(limits);
	else
		depot = generate_random_depot(limits);
	generated.insert(generated.begin(), depot);
	
	return generated;
}

//Randomly selects nodes from the list given - should it be different?
std::vector<Node> Distributer::select_nodes(const std::vector<Node>& nodes, const size_t num_points, const DepotType dptt, const size_t tw_size, const size_t max_route_time){
	//estimating time with fixed speed of 20 km/h (highly conservative)
	//TODO: should check this
	const double SPEED = 1.0/3.0; //km/min
	std::vector<bool> used(nodes.size(), false);
	std::vector<Node> selected;
	size_t ai = rng()%nodes.size(); //attractor's index
	Node attractor = nodes[ai]; //an attractor is required to avoid infeasible time windows
	used[ai] = true;
	used[attractor.pair] = true;
	size_t marked = 2;
	double width = static_cast<double>(tw_size)/2;
	double horizon = static_cast<double>(max_route_time);
	while(selected.size() < num_points-1){
		size_t s = rng()%(nodes.size()-1);
		
		if(not used[s]){
			s = nodes[s].type == NodeType::Pickup ? s : nodes[s].pair;
			//printf("s = %ld | %ld | %ld | %ld | %ld\n", s, nodes[s].pair, selected.size(), marked, nodes.size());
			if(nodes[s].etw + width < horizon and nodes[nodes[s].pair].etw + width <= horizon){			
				double tas = GeoLocation::haversine_distance(attractor.loc.to_rad(), nodes[s].loc.to_rad())/SPEED;
				double tsd = GeoLocation::haversine_distance(nodes[s].loc.to_rad(), nodes[nodes[s].pair].loc.to_rad())/SPEED;
				double tda = GeoLocation::haversine_distance(nodes[nodes[s].pair].loc.to_rad(), attractor.loc.to_rad())/SPEED;
				double center = nodes[s].etw;
				double dcenter = nodes[nodes[s].pair].etw;
				if(tas > center-width){
					center = tas+width;
					dcenter += (center-nodes[s].etw);
				}

				tas = std::max(tas, center);
				tsd = std::max(tas+nodes[s].stw+tsd, dcenter-width);
				tsd += nodes[s].stw + tda;
				if(tsd < horizon){
					size_t sp = selected.size();
					selected.push_back(nodes[s]);
					//selected[sp].etw = center;
					selected.push_back(nodes[nodes[s].pair]);
					//selected[sp+1].etw = dcenter;
					//make pairs be one index more due to the depot that will come in the next steps at the begining (so we do not have to go through all list again and change)
					selected[sp].pair = sp+2;
					selected[sp+1].pair = sp+1;
					selected[sp].id = sp+1;
					selected[sp+1].id = sp+2;
				}
			}
			//even if we do not use, mark as used to avoid reselcting
			used[s] = true;
			used[nodes[s].pair] = true;
			marked+=2;
		}
	}
	used[ai] = false;
	bool valid_depot = false;
	GeoLocation loc_depot;
	size_t tries = 0;
	do{
		if(dptt == DepotType::Central){
			if(tries > 100) //if every 100 tries no valid central depot was found, just take the attractor (which should be +- centralized anyway)
				loc_depot = attractor.loc;
			else
				loc_depot = select_central_depot(nodes, used);
		}else{
			if(tries > 10000) //avoid "infinite" loop
				loc_depot = attractor.loc;
			else
				loc_depot = select_random_depot(nodes, used);
		}

		valid_depot = true;
		for(size_t p=0;p<selected.size();p+=2){
			double t0p = GeoLocation::haversine_distance(loc_depot.to_rad(), selected[p].loc.to_rad())/SPEED;
			double tpd = GeoLocation::haversine_distance(selected[p].loc.to_rad(), selected[p+1].loc.to_rad())/SPEED;
			double td0 = GeoLocation::haversine_distance(selected[p+1].loc.to_rad(), loc_depot.to_rad())/SPEED;
			double center = selected[p].etw;
			double dcenter = selected[p+1].etw;
			if(t0p > center-width){
				center = t0p+width;
				dcenter += (center-selected[p].etw);
			}

			tpd = std::max(t0p+selected[p].stw+tpd, dcenter-width);
			tpd += selected[p].stw + td0;
			if(floor(tpd) > horizon){
				//printf("Invalid for %d | %f\n", p, floor(tpd));
				valid_depot = false;
				break;
			}		
		}
		//printf("%f %f valid: %i\n", loc_depot.lat, loc_depot.lon, valid_depot);
		tries++;
	}while(not valid_depot);
	
	Node depot(0, loc_depot, NodeType::Depot);
	//Node depot = attractor;
	depot.etw = 0;
	depot.ltw = horizon;
	selected.insert(selected.begin(), depot);
	return selected;
}
