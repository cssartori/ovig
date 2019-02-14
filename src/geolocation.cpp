#include "../include/geolocation.hpp"
#include <cmath>

// auxiliary math functions
double to_rad_double(double deg){
	// transforms angle in degrees to radians
	return M_PI*deg/180.0;
}

double to_deg_double(double rad){
	// transforms angle in radians to degrees
	return 180.0*rad/M_PI;
}

double sin_sq(double angle){
	// computes the sin^2(angle)
	double s = sin(angle);
	return s*s;
}

GeoLocation GeoLocation::to_rad() const{
	// transforms angle in degrees to radians
	return {to_rad_double(this->lat), to_rad_double(this->lon)};
}

GeoLocation GeoLocation::to_rad(GeoLocation deg){
	// transforms angle in degrees to radians
	return deg.to_rad();
}

GeoLocation GeoLocation::to_deg() const{
	// transforms in radians to degrees
	return {to_deg_double(this->lat), to_deg_double(this->lon)};
}

GeoLocation GeoLocation::to_deg(GeoLocation rad){
	// transforms angle in radians to degrees
	return rad.to_deg();
}

double GeoLocation::haversine_distance_to(GeoLocation dest){
	const double EARTH_RADIUS = 6372.796924; //km
	double t1 = sin_sq((dest.lat-this->lat)/2); //first term
	double t2 = cos(this->lat)*cos(dest.lat)*sin_sq((dest.lon-this->lon)/2); //second term
	double sv = sqrt(t1+t2);
	
	return 2*EARTH_RADIUS*asin(sv);
}

double GeoLocation::haversine_distance(GeoLocation src, GeoLocation dest){
	return src.haversine_distance_to(dest);
}
