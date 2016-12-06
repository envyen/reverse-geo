/*
 * reverse-geocode.c
 * 
 * Retrieve Road address (etc) from OSM Reverse Geocode 
 * Uses Parson http://kgabis.github.com/parson/ for json parsing.
 * 
 * Copyright 2016 Naveen <naveen.karuthedath@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
#include <stdio.h>
#include "parson.h"

/* Refer OpenStreetMap data access API
 * http://wiki.openstreetmap.org/wiki/Nominatim_usage_policy
 * http://wiki.openstreetmap.org/wiki/Nominatim#Reverse_Geocoding
 */
#define CURL_USERAGENT "\"Custom GPS App\""
#define OSM_REVGEOCODE "curl -s -A "  CURL_USERAGENT \
		"\"http://nominatim.openstreetmap.org/reverse?format=json&" \
		"lat=%f&lon=%f&zoom=18&addressdetails=1\" > %s"

static struct pos {
	double lat;
	double lon;
	char road_name[250];
	double speed;
};

/* 
 * name: nominatim_rev
 * @param pos: pointer to struct pos with latitude & longitude.
 *             function returned with road_name (string) filled
 * @return void 
 */

void nominatim_rev(struct pos *p) 
{
	char curl_command[512];
	char cleanup_command[256];
	char output_filename[] = "/tmp/geo-loc.json";

	JSON_Value *root_value;
	JSON_Object *root_obj;
	
	sprintf(curl_command, OSM_REVGEOCODE, p->lat, p->lon, output_filename);
	sprintf(cleanup_command, "rm -f %s", output_filename);
	system(curl_command);

	/* parsing json */
	root_value = json_parse_file(output_filename);
	if(root_value != NULL) {
		root_obj = json_value_get_object(root_value);
		sprintf(p->road_name, "%s", json_object_dotget_string(root_obj, "address.road"));
	}
	
	/* cleanup code */
	json_value_free(root_value);
	system(cleanup_command);

	/* No heavy API uses or you will get denied responses :) */
	sleep(2);	
}

/*
 * name: main()
 * @param none
 * @return int
 */

int main(void)
{
	int i=0;
	struct pos gps; 
	
	while(1){	
		/* TODO Get this from gpsd */
		gps.lat=10.0134461;
		gps.lon=76.3311157;
		nominatim_rev(&gps);
		printf("[%d] %s\n", i++, gps.road);
	}
	
	return 0;
}


/* Sample JSON Output
 * 
 * {
  "place_id": "2398394",
  "licence": "Data © OpenStreetMap contributors, ODbL 1.0. http:\/\/www.openstreetmap.org\/copyright",
  "osm_type": "node",
  "osm_id": "343709643",
  "lat": "10.01359",
  "lon": "76.33076",
  "display_name": "Reliance Store, Kakkanad, Palarivattom - Kakkanad Road, Palarivattom, Ernakulam, Kerala, 682021, India",
  "address": {
    "supermarket": "Reliance Store, Kakkanad",
    "road": "Palarivattom - Kakkanad Road",
    "suburb": "Palarivattom",
    "city": "Ernakulam",
    "state_district": "Ernakulam",
    "state": "Kerala",
    "postcode": "682021",
    "country": "India",
    "country_code": "in"
  },
  "boundingbox": [
    "10.01349",
    "10.01369",
    "76.33066",
    "76.33086"
  ]
}
*/
