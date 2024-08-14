#ifndef GPS_HEADER
#define GPS_HEADER

#include <stdint.h>

// use eprintfs instead of printfs implementation
#ifdef GPS_USE_EPRINTFS

#pragma message("Using eprintfs for GPS functions")

#include "eprintfs.h"

#define gps_strstr(str1, str2) estrstr(str1, str2)
#define gps_strncmp(str1, str2, num) estrncmp(str1, str2, num)
#define gps_atoi(buffer, base, stop) eatoi(buffer, base, stop)
#define gps_atof(buffer, stop) eatof(buffer, stop)

#else

#pragma message("Using stdlib and string for GPS functions")

#include <stdlib.h>
#include <string.h>

#define gps_strstr(str1, str2) strstr(str1, str2)
#define gps_strncmp(str1, str2, num) strncmp(str1, str2, num)
#define gps_atoi(buffer, base, stop) atoi(buffer)
#define gps_atof(buffer, stop) atof(buffer)

#endif

extern const char * gps_cmd_baudrate_4800;
extern const char * gps_cmd_baudrate_9600;
extern const char * gps_cmd_baudrate_19200;
extern const char * gps_cmd_baudrate_38400;
extern const char * gps_cmd_baudrate_57600;
extern const char * gps_cmd_baudrate_115200;

extern const char * gps_cmd_update_rate_5;
extern const char * gps_cmd_update_rate_4;
extern const char * gps_cmd_update_rate_1;

#define GPS_PARSED_GPGGA        (1 << 0)  // 0, time; 1, latitude; 2, N/S lat;
                                          // 3, longitude; 4, E/W lon; 5, fix;
                                          // 6, satellites number; 8, altitude
#define GPS_PARSED_GPVTG        (1 << 1)  // 0, heading; 6, velocity

typedef struct {
	float longitude;		// in degrees, positive N, negative S hemisphere
							// dd.dddd
	float latitude;			// in degrees, positive E, negative W 
							// dd.dddd

	int time;				// encoded as HHMMSS

	float altitude;			// in meters
	float velocity;			// in km/h

	int fix;				// fix, 0 - no fix, 1 - GPS fix, 2 - Differential GPS fix

	int sat_number;			// number of visible satellites

	float heading;			// in degrees, 0 to 360 where 0 - N, 90 - E, 180 - S, 270 - W

	uint32_t status;		// binary encoded status
} gps_t;

/**
 * @brief Parse content of buffer in terms of GPS NMEA
 * 
 * @param gps 		gps structure to fill in
 * @param buffer 	Buffer with sentences
 * @param length 	Length of the buffer
 * @return int 		Binary number indicating which sentences were parsed
 */
int gps_parse(gps_t * gps, char * buffer, unsigned int length);

/**
 * @brief Return pointer to next param
 * 
 * If a comma or asterisk has been found point to next character after that sign.
 * If a dollar sign has been found point to the dollar sign.
 * 
 * @param buffer 
 * @return char* pointer to next param
 */
char *gps_next_param(char *buffer);

char * gps_parse_GPGGA(gps_t * gps, char * buffer);

char * gps_parse_GPVTG(gps_t * gps, char * buffer);

//calculate CRC from buffer
//CRC is stored in CRC
//return
// 0 -- CRC match
//-1 -- CRC do not match
//-2 -- GPS sentence not parsed well
int gps_calc_crc(const char * buffer, char * crc);

#endif
