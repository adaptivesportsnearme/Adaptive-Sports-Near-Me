/* A module to handle parsing google calendar information */

// more information on Google Calendar formatting can be found here:
// https://developers.google.com/workspace/calendar/api/concepts/inviting-attendees-to-events#link-user

// although for whatever reason it omits that &ctz= also stores the timezone as of 20260616T172200 ;)

// This module has been tested for memory leaks, even in the presence of broken and adversarial links
// Its accuracy has also been vetted.

#include "calendarparser.h"

//-------------------- Helper Functions ---------------------- //

/* Given a string and a substring, returns the index of the first character of the substring upon a match,
	or -1 if the substring is not found or either string is null. If there are multiple substring matches,
	then strfind will return the first occurrence of the substring. */

size_t strfind(char* str, char* substr) {

	// handles case of NULL parameters
	if (str == NULL || substr == NULL) {
		return -1;
	}

	if (strlen(str) < strlen(substr)) return -1;

	// performs an O(mn - n^2) search for the substring, where m = strlen(str), and n = strlen(substr)
	for (int i = 0; i < (strlen(str) - strlen(substr)) + 1; i++) {
		int j;
		for (j = 0; j < strlen(substr); j++) {
			if (*(str + i + j) != *(substr + j)) {
				break;
			}
		}
		if (j == strlen(substr)) {
			return i;
		}
	}

	return -1;
}

/* Obeys the same requirements as strfind, but only begins searching for matches at index start. 

	For example the call
		strfind("happy! so happy", "happy", 6);
						   ^
	will return 10, which is the first occurrence of substring "happy" after index 6
	
	returns -1 */

size_t strfindfrom(char* str, char* substr, int start) {

	if (str == NULL || substr == NULL) {
		return -1;
	}


	if (strlen(str) < strlen(substr) || start >= strlen(str)) {
		return -1;
	}


	for (int i = start; i < (strlen(str) - strlen(substr)) + 1; i++) {
		int j = -1;
		for (j = 0; j < strlen(substr); j++) {
			if (*(str + i + j) != *(substr + j)) {
				break;
			}
		}
		if (j == strlen(substr)) {
			return i;
		}
	}

	return -1;
}

//-------------------- Parsing Functions ---------------------- //


// finds the date information from a string in the ISO 8601 basic date and time format
// and passes that information into the provided arguments. If any information is in
// an improper format, the default value of 0 will be passed.

// https://en.wikipedia.org/wiki/ISO_8601

int parseGoogleTime(char* str, int* year, int* mon, int* day, int* time) {

	if (strlen(str) < 8) return 0;

	// a placeholder variable to prevent bad information from being passed
	int temp;

	char * parsebuffer = calloc(5, sizeof(char));
	*(parsebuffer + 4) = '\0';

	memcpy(parsebuffer, str, 4);
	temp = atoi(parsebuffer);
	if (temp >= 0000 && temp <= 9999) { // verifies that year is in the ISO 8601 format
		*year = temp;
	}
	
	*(parsebuffer + 2) = '\0';
	memcpy(parsebuffer, str + 4, 2);
	temp = atoi(parsebuffer);
	if (temp >= 1 && temp <= 12) {
		*mon = temp;
	}
	
	memcpy(parsebuffer, str + 6, 2);
	temp = atoi(parsebuffer);
	if (temp >= 1 && temp <= 31) {
		*day = temp;
	}
	

	int hrs = 0;
	int min = 0;
	int absolute = 0;

	if (strlen(str) >= 15) { // YYYYMMDDTHHMISS is length 8 + 1 + 6 = 15
		if (*(str + 8) == 'T') {
			memcpy(parsebuffer, str + 9, 2);
			hrs = atoi(parsebuffer);
			if (hrs >= 0 && hrs <= 24) {
				memcpy(parsebuffer, str + 11, 2);
				min = atoi(parsebuffer);
				if (min >= 0 && min <= 59 && !(hrs == 24 & min != 0)) {
					*time = 60 * hrs + min;
				}
			}
	
			
		}

		if (strlen(str) >= 16) {
			if (*(str + 16) == 'Z' || *(str + 16) == 'z') {
				absolute = 1;
			}
		}
	}

	free(parsebuffer);
	
	return absolute;
}


/*  Google Calendar uses URL encoding to store strings in URLs. This function takes a 
	URL-encoded string and decodes it, modifying the original string and adjusting
	its space in memory to accomodate. */

// https://en.wikipedia.org/wiki/Percent-encoding

// takes a pointer to the string as input, so that it may be modified in memory

void parseLinkString(char** str) {
	// we are changing the string in-place, but, for instance, the sequence "%20" resolves to ' '.
	// this offset accounts for the difference in length these kinds of change create
	int offset = 0;
	int length = strlen(*str);

	for (int i  = 0; i + offset < length; i++) {
		// '+' is resolved to ' '
		if (*(*str + offset + i) == '+') {
			*(*str + i) = ' ';
		} else if (*(*str + offset + i) == '%' && (offset + i + 2 <= length)) {
			// handles percent-encoding
			
			char hexstr[3] = {*(*str + offset + i + 1), *(*str + offset + i + 2), '\0'};

			int hex = (int)strtol(hexstr, NULL, 16);

			// Google calendar seemingly allows any ASCII value to be passed as string data in a link
			// so the decoding reflects this.

			// We don't have to check hex's range here, because we are converting from a 2-character
			// hexadecimal string, which already protects from extreme values. We only check for 
			// non-negativity. Typically, strtol would use errno to convey that 0 is the converted
			// value. But we ABSOLUTELY don't want users putting string-terminating characters
			// in our strings, so we just ignore this case.
			if (hex >= 0) {
				*(*str + i) = (char)hex;
				offset += 2;
			} else {
				*(*str + i) = *(*str + i + offset);
			}

			// 
		} else {
			*(*str + i) = *(*str + i + offset);
		}
	}

	// at the end of this process, we may have a buffer that is shorter than what we began with
	// we will perform a realloc to keep string length minimal.

	*str = realloc(*str, length + 1 - offset);
	*(*str + length - offset) = '\0';
	
}

 /* Given a Google calendar link, parses data into the instance. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. */

// If the link mixes UTC and local when storing start and end date this function will set UTC_time
// based off the start date

void parseGoogleCalendarLink(ASInstance* instance, char* link) {

	if (link == NULL) return;

	// all valid google calendar links must include
		// "action=TEMPLATE" -> this tells the site that the link contains event information
		// "text=" -> for the name of the event
		// "dates=" -> for the time of the event

	// if the link is invalid we will return immediately
	if ((strfind(link, "calendar.google.com") == -1 && 
		strfind(link, "google.com/calendar") == -1) || strfind(link, "action=TEMPLATE") == -1
		|| strfind(link, "&text=") == -1 || strfind(link, "&dates=") == -1) {
		return;
	}

	if (instance == NULL) {
		instance = calloc(1, sizeof(ASInstance));
	}

	// START OF TITLE PARSING //

	// We know we will find an index in the string, as "&text=" is required in the link, and 
	// we already verified that this will not return -1
	size_t start_index = strfind(link, "&text="); // param starts at start_index + 6
	size_t end_index;							  // param ends at end_index - 1
												  // param has length end_index - start_index - 6

	end_index = strfindfrom(link, "&", start_index + 6);
	if (end_index == -1) {
		end_index = strlen(link);
	}

	if (end_index - start_index - 6 >= 0) { // only collects data if it is possible

		char * title = calloc(end_index - start_index - 5, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(title, link + start_index + 6, end_index - start_index - 6);
		*(title + end_index - start_index - 6) = '\0';

		parseLinkString(&title);

		// frees space to overwrite instance variable
		if (instance->name != NULL) {
			free(instance->name);
		}

		instance->name = title;
	}

	// END OF TITLE PARSING

	// START OF DATE PARSING

	// We know we will find an index in the string, as "&dates=" is required in the link, and 
	// we already verified that this will not return -1
	start_index = strfind(link, "&dates="); // param starts at start_index + 7
	        							    // param ends at end_index - 1
										    // param has length end_index - start_index - 7
	end_index = strfindfrom(link, "&", start_index + 6);
	if (end_index == -1) {
		end_index = strlen(link);
	}

	int datelen = end_index - start_index - 7;

	if (datelen >= 17) { // 17 is the minimum time length allowed

		char * date = calloc(datelen + 1, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(date, link + start_index + 7, datelen);
		*(date + datelen) = '\0';

		// gets the divider between the dates.
		size_t split = strfind(date, "/");

		if (split < end_index) {
		
			char * startdate = calloc(split + 1, sizeof(char));
			char * enddate = calloc(datelen - split + 1, sizeof(char));

			memcpy(startdate, date, split);
			*(startdate + split) = '\0';
			
			memcpy(enddate, date + split + 1, datelen - split);
			*(date + datelen - split) = '\0';

			free(date);

			// we are assuming that we are not mixing date formatting, although this technically results in a valid
			// google calendar link
			instance->UTC_time = parseGoogleTime(startdate, &(instance->start_year), &(instance->start_month), &(instance->start_day), &(instance->start_time));
			parseGoogleTime(enddate, &(instance->end_year), &(instance->end_month), &(instance->end_day), &(instance->end_time));

			free(startdate);
			free(enddate);
		} 
		// it might be worth returning if we can't find a "/" here, as that necessarily invalidates the link.
		// However, even if the date format is corrupted, it still might be worth trying to retrieve other data.
	}

	// END OF DATE PARSING //

	// START OF DESCRIPTION PARSING //
	
	start_index = strfind(link, "&details=");

	if (start_index != -1) {
		end_index = strfindfrom(link, "&", start_index + 8); // desc starts at start_index + 9
		if (end_index == -1) end_index = strlen(link);	 // desc ends at end_index - 1
															 // desc has length end_index - start_index - 9

		if (end_index - start_index - 9 >= 0) {
			char * details = calloc(end_index - start_index - 9, sizeof(char));
			
			memcpy(details, link + start_index + 9, end_index - start_index - 9);
			*(details + end_index - start_index - 9) = '\0';

			parseLinkString(&details);

			if (instance->description != NULL) free(instance->description);
			instance->description = details;
		}

	}

	// END OF DESCRIPTION PARSING //

	// START OF LOCATION PARSING //

	start_index = strfind(link, "&location=");

	if (start_index != -1) {
		end_index = strfindfrom(link, "&", start_index + 9); 
		if (end_index == -1) end_index = strlen(link);

		if (end_index - start_index - 10 >= 0) {
			char* locationstring = calloc(end_index - start_index - 9, sizeof(char));

			memcpy(locationstring, link + start_index + 10, end_index - start_index - 10);
			*(locationstring + end_index - start_index - 10) = '\0';

			parseLinkString(&locationstring);

			// PLACEHOLDER WARNING UNTIL STRING TO LOCATION PARSING IS POSSIBLE
			// printf("WARNING! Due to a lack of a string to location parser, this link is ignoring useful information!\nPlease fix. Information lost: Location: %s\n", locationstring);
			free(locationstring);
		}
	}

	// END OF LOCATION PARSING //

	// START OF TIMEZONE PARSING //

	// time zone information may be stored either in a single time zone under &ctz=
	// or as a start and end time zone under &stz= and &etz= respectively.
	// We will prioritize the start time zone, as few sporting events should cross time zone lines
	// and even fewer will pass that information effectively in a calendar link.

	size_t ctz_index = strfind(link, "&ctz=");
	size_t stz_index = strfind(link, "&stz=");

	if (ctz_index != -1 || stz_index != -1) {
		if (ctz_index != -1) {
			start_index = ctz_index; 
		} else {
			start_index = stz_index;
		}

		end_index = strfindfrom(link, "&", start_index + 4); 
		if (end_index == -1) end_index = strlen(link);

		if (end_index - start_index - 5 >= 0) {

			char* tzstring = calloc(end_index - start_index - 4, sizeof(char));

			memcpy(tzstring, link + start_index + 5, end_index - start_index - 5);
			*(tzstring + end_index - start_index - 5) = '\0';

			if (instance->timezone != NULL) free(instance->timezone);

			instance->timezone = tzstring;

		}
	}

	// END OF TIMEZONE PARSING //

	ASInstanceFormatMemory(instance);

}


/* Given a Google calendar link, parses data into the ASTime. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. */

// This is a version of parseGoogleCalendarLink modified specifically for ASTime

// If the link mixes UTC and local when storing start and end date this function will set UTC_time
// based off the start date

void parseGoogleCalendarLinkAST(ASTime* time, char* link) {

	if (link == NULL) return;

	// all valid google calendar links must include
		// "action=TEMPLATE" -> this tells the site that the link contains event information
		// "text=" -> for the name of the event
		// "dates=" -> for the time of the event

	// if the link is invalid we will return immediately
	if ((strfind(link, "calendar.google.com") == -1 && 
		strfind(link, "google.com/calendar") == -1) || strfind(link, "action=TEMPLATE") == -1
		|| strfind(link, "&text=") == -1 || strfind(link, "&dates=") == -1) {
		return;
	}

	if (time == NULL) {
		time = calloc(1, sizeof(ASTime));
	}

	// START OF DATE PARSING

	// We know we will find an index in the string, as "&dates=" is required in the link, and 
	// we already verified that this will not return -1
	size_t start_index = strfind(link, "&dates="); // param starts at start_index + 7
	        							    // param ends at end_index - 1
										    // param has length end_index - start_index - 7
	size_t end_index = strfindfrom(link, "&", start_index + 6);
	if (end_index == -1) {
		end_index = strlen(link);
	}

	int datelen = end_index - start_index - 7;

	if (datelen >= 17) { // 17 is the minimum time length allowed

		char * date = calloc(datelen + 1, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(date, link + start_index + 7, datelen);
		*(date + datelen) = '\0';

		// gets the divider between the dates.
		size_t split = strfind(date, "/");

		if (split < end_index) {
		
			char * startdate = calloc(split + 1, sizeof(char));
			char * enddate = calloc(datelen - split + 1, sizeof(char));

			memcpy(startdate, date, split);
			*(startdate + split) = '\0';
			
			memcpy(enddate, date + split + 1, datelen - split);
			*(date + datelen - split) = '\0';

			free(date);

			// we are assuming that we are not mixing date formatting, although this technically results in a valid
			// google calendar link
			time->UTC_time = parseGoogleTime(startdate, &(time->start_year), &(time->start_month), &(time->start_day), &(time->start_time));
			parseGoogleTime(enddate, &(time->end_year), &(time->end_month), &(time->end_day), &(time->end_time));

			free(startdate);
			free(enddate);
		} 
		// it might be worth returning if we can't find a "/" here, as that necessarily invalidates the link.
		// However, even if the date format is corrupted, it still might be worth trying to retrieve other data.
	}

	// END OF DATE PARSING //

	// START OF TIMEZONE PARSING //

	// time zone information may be stored either in a single time zone under &ctz=
	// or as a start and end time zone under &stz= and &etz= respectively.
	// We will prioritize the start time zone, as few sporting events should cross time zone lines
	// and even fewer will pass that information effectively in a calendar link.

	size_t ctz_index = strfind(link, "&ctz=");
	size_t stz_index = strfind(link, "&stz=");

	if (ctz_index != -1 || stz_index != -1) {
		if (ctz_index != -1) {
			start_index = ctz_index; 
		} else {
			start_index = stz_index;
		}

		end_index = strfindfrom(link, "&", start_index + 4); 
		if (end_index == -1) end_index = strlen(link);

		if (end_index - start_index - 5 >= 0) {

			char* tzstring = calloc(end_index - start_index - 4, sizeof(char));

			memcpy(tzstring, link + start_index + 5, end_index - start_index - 5);
			*(tzstring + end_index - start_index - 5) = '\0';

			if (time->timezone != NULL) free(time->timezone);

			time->timezone = tzstring;

		}
	}

}

/* Given a Google calendar link, parses data into the ASLocation. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. */

void parseGoogleCalendarLinkASLoc(ASLocation* location, char* link) {

	if (link == NULL) return;

	// all valid google calendar links must include
		// "action=TEMPLATE" -> this tells the site that the link contains event information
		// "text=" -> for the name of the event
		// "dates=" -> for the time of the event

	// if the link is invalid we will return immediately
	if ((strfind(link, "calendar.google.com") == -1 && 
		strfind(link, "google.com/calendar") == -1) || strfind(link, "action=TEMPLATE") == -1
		|| strfind(link, "&text=") == -1 || strfind(link, "&dates=") == -1) {
		return;
	}

	if (location == NULL) {
		location = calloc(1, sizeof(ASLocation));
	}

	// START OF LOCATION PARSING //

	size_t start_index = strfind(link, "&location=");
	size_t end_index;

	if (start_index != -1) {
		end_index = strfindfrom(link, "&", start_index + 9); 
		if (end_index == -1) end_index = strlen(link);

		if (end_index - start_index - 10 >= 0) {
			char* locationstring = calloc(end_index - start_index - 9, sizeof(char));

			memcpy(locationstring, link + start_index + 10, end_index - start_index - 10);
			*(locationstring + end_index - start_index - 10) = '\0';

			parseLinkString(&locationstring);

			// PLACEHOLDER WARNING UNTIL STRING TO LOCATION PARSING IS POSSIBLE
			// printf("WARNING! Due to a lack of a string to location parser, this link is ignoring useful information!\nPlease fix. Information lost: Location: %s\n", locationstring);
			free(locationstring);
		}
	}

	// END OF LOCATION PARSING //

	// START OF TIMEZONE PARSING //

	// time zone information may be stored either in a single time zone under &ctz=
	// or as a start and end time zone under &stz= and &etz= respectively.
	// We will prioritize the start time zone, as few sporting events should cross time zone lines
	// and even fewer will pass that information effectively in a calendar link.

	size_t ctz_index = strfind(link, "&ctz=");
	size_t stz_index = strfind(link, "&stz=");

	if (ctz_index != -1 || stz_index != -1) {
		if (ctz_index != -1) {
			start_index = ctz_index; 
		} else {
			start_index = stz_index;
		}

		end_index = strfindfrom(link, "&", start_index + 4); 
		if (end_index == -1) end_index = strlen(link);

		if (end_index - start_index - 5 >= 0) {

			char* tzstring = calloc(end_index - start_index - 4, sizeof(char));

			memcpy(tzstring, link + start_index + 5, end_index - start_index - 5);
			*(tzstring + end_index - start_index - 5) = '\0';

			if (location->timezone != NULL) free(location->timezone);

			location->timezone = tzstring;

		}
	}

}