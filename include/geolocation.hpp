#ifndef __H_GEOLOCATION_H__
#define __H_GEOLOCATION_H__

/*
 * Data strucutre to hold a geographic location in (lat,lon) coordinates.
 * It contains some associated functions as well.
 *
 */

struct GeoLocation {
	double lat;
	double lon;

	GeoLocation to_rad() const;
	GeoLocation to_deg() const;
	double haversine_distance_to(GeoLocation dest);
	
	static GeoLocation to_rad(GeoLocation deg);
	static GeoLocation to_deg(GeoLocation rad);
	static double haversine_distance(GeoLocation src, GeoLocation dest);
	
};


#endif //__H_LOCATION_H__
