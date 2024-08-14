#include "gps.h"

#ifndef NULL
#define NULL 0x00000000
#endif

const char * gps_cmd_baudrate_4800 = "$PMTK251,4800*14\r\n"; //  to setup output baudrate as 4800 bps
const char * gps_cmd_baudrate_9600 = "$PMTK251,9600*17\r\n"; //  to setup output baudrate as 9600 bps
const char * gps_cmd_baudrate_19200 = "$PMTK251,19200*22\r\n"; //  to setup output baudrate as 19200 bps
const char * gps_cmd_baudrate_38400 = "$PMTK251,38400*27\r\n"; //  to setup output baudrate as 38400 bps
const char * gps_cmd_baudrate_57600 = "$PMTK251,57600*2C\r\n"; //  to setup output baudrate as 57600 bps
const char * gps_cmd_baudrate_115200 = "$PMTK251,115200*1F\r\n"; //  to setup output baudrate as 115200 bps

const char * gps_cmd_update_rate_5 = "$PMTK300,200,0,0,0,0*2F\r\n"; // to configure update rate as 5 Hz
const char * gps_cmd_update_rate_4 = "$PMTK300,250,0,0,0,0*2A\r\n"; // to configure update rate as 4 Hz
const char * gps_cmd_update_rate_1 = "$PMTK300,1000,0,0,0,0*1C\r\n"; //to configure update rate as 1 Hz

int gps_parse(gps_t * gps, char * buffer, unsigned int length) {
	char * dollar;
	char crc;
	int crc_ret;

	gps->status = 0x00;

	dollar = buffer;

	for (; dollar - buffer < length;) {
		// find dollar character
		dollar = gps_strstr(dollar, "$");
		if (dollar == NULL)
			break;

		crc_ret = gps_calc_crc(dollar, &crc);

		if (crc_ret != 0) {
			// search for next valid sentence
			++dollar;
			continue;
		}

		if (!gps_strncmp((char*) (dollar + 1), "GPGGA", 5)) {
			dollar = gps_parse_GPGGA(gps, dollar + 7);
		} else if (!gps_strncmp((char*) (dollar + 1), "GPVTG", 5)) {
			dollar = gps_parse_GPVTG(gps, dollar + 7);
		} else {
			// sentence valid but unknown
			// skip the dollar sign to find a new one
			++dollar;
		}
	}

	return gps->status;
}

char* gps_next_param(char * buffer) {
	int i;
	for (i = 0; i < 20; ++i) {
		if (buffer[i] == ',') {
			return (char*) &buffer[i + 1];
		} else if (buffer[i] == '$' || buffer[i] == '*') {
			return (char*) &buffer[i];
		}
		if (buffer[i] == '\0') {
			return NULL;
		}
	}

	return NULL;
}

char* gps_parse_GPGGA(gps_t * gps, char * buffer) {
	int i;
	int tmp;
	gps->status &= ~(GPS_PARSED_GPGGA);

	for (i = 0; i < 13; ++i) {
		switch (i) {
		// UTC Time
		case 0: {
			gps->time = gps_atoi(buffer, 10, NULL);
			break;
		}
			// Latitude
		case 1: {
			gps->latitude = gps_atof(buffer, NULL);
			gps->latitude /= 100.0f;
			tmp = (int) (gps->latitude);
			gps->latitude = tmp + (gps->latitude - tmp) * (100.0f / 60.0f);
			break;
		}
			// N/S Indicator
		case 2: {
			if (buffer[0] == 'S') {
				gps->latitude *= -1;
			}
			break;
		}
			// Longitude
		case 3: {
			gps->longitude = gps_atof(buffer, NULL);
			gps->longitude /= 100.0f;
			tmp = (int) (gps->longitude);
			gps->longitude = tmp + (gps->longitude - tmp) * (100.0f / 60.0f);
			break;
		}
			// E/W Indicator
		case 4: {
			if (buffer[0] == 'W') {
				gps->longitude *= -1;
			}
			break;
		}
			// Position Fix Indicator
		case 5: {
			gps->fix = gps_atoi(buffer, 10, NULL);
			break;
		}
			// Satellites Used
		case 6: {
			gps->sat_number = gps_atoi(buffer, 10, NULL);
			break;
		}
			// HDOP
		case 7: {
			break;
		}
			// MSL Altitude
		case 8: {
			gps->altitude = gps_atof(buffer, NULL);
			break;
		}
		}
		buffer = gps_next_param(buffer);
	}

	if (buffer) {
		gps->status |= GPS_PARSED_GPGGA;
	}

	return buffer;
}

char* gps_parse_GPVTG(gps_t * gps, char * buffer) {
	int i;
	gps->status &= ~(GPS_PARSED_GPVTG);
	for (i = 0; i < 9; ++i) {
		switch (i) {
		// Course
		case 0: {
			gps->heading = gps_atof(buffer, NULL);
			break;
		}
			// Speed
		case 6: {
			gps->velocity = gps_atof(buffer, NULL);
			break;
		}
		}
		buffer = gps_next_param(buffer);
	}

	if (buffer) {
		gps->status |= GPS_PARSED_GPVTG;
	}

	return buffer;
}

int gps_calc_crc(const char * buffer, char * crc) {
	int i, j;
	int local_crc = 0;
	char * line;

	// find first dollar and set pointer on first
	// character after dollar sign $
	for (i = 0; i < 50; ++i) {
		if (buffer[i] == '$') {
			++i;
			break;
		}
	}
	// move buffer
	line = (char*) buffer + i;
	i = 0;

	// everything between dollar sign $ and asterisk *
	*crc = 0;
	for (i = 0; i < 256; ++i) {
		if (line[i] == '\0') {
			break;
		}
		if (line[i] != '*') {
			*crc ^= line[i];
		} else {
			break;
		}
	}

	// change CRC string to CRC number form line
	for (j = 1; j >= 0; --j) {
		++i;
		if (line[i] >= '0' && line[i] <= '9') {
			local_crc += (line[i] - 48) << (4 * j);
		} else if (line[i] >= 'A' && line[i] <= 'F') {
			local_crc += (line[i] - 65 + 10) << (4 * j);
		} else {
			return -2;
		}
	}

	if (local_crc != *crc)
		return -1;

	return 0;
}
