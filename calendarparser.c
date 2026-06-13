/* A module to handle parsing google calendar information */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// DESCSIZE is the maximum allowed description length, with DESCSIZE - 1 characters and a null terminator.
#define DESCSIZE 1024
#define NAMESIZE 256


/* In the future we will use a shared struct across code.This is just a temporary
	struct while this module is developed */
struct tempinstance {
	char* name,
		*zip, *city, *state, *country,
		*address_line_1, *address_line_2, *description;
	int start_time, end_time, start_month, start_day, start_year, end_month, end_day, end_year;
	int UTC_time;
};

// incomplete!!! do not use outside this module and delete soon!!!
void freeinstance(struct tempinstance * instance) {
	if (instance->name != NULL) free(instance->name);
	if (instance->description != NULL) free(instance->description);
	free(instance);
}


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

// finds the date information from a link in the format YYYYMMDD
// after YYYYMMDD there may be a time included, which is in form THHMMSS or THHMMSSZ

// If the Z is present, then the time is being conveyed in UTC time
// and we will return a 1 to indicate that we need to convert to relative time

int parseGoogleTime(char* str, int* year, int* mon, int* day, int* time) {

	if (strlen(str) < 8) return 0;

	char * parsebuffer = calloc(5, sizeof(char));
	*(parsebuffer + 4) = '\0';

	memcpy(parsebuffer, str, 4);
	*year = atoi(parsebuffer);
	*(parsebuffer + 2) = '\0';

	memcpy(parsebuffer, str + 4, 2);
	*mon = atoi(parsebuffer);
	
	memcpy(parsebuffer, str + 6, 2);
	*day = atoi(parsebuffer);

	int temptime = 0;
	int absolute = 0;

	if (strlen(str) >= 17) {
		if (*(str + 8) == 'T') {
			memcpy(parsebuffer, str + 9, 2);
			temptime = 60 * atoi(parsebuffer);

			memcpy(parsebuffer, str + 11, 2);
			*time = temptime + atoi(parsebuffer);
		}

		if (strlen(str) >= 18) {
			if (*(str + 17) == 'Z' || *(str + 17) == 'z') {
				absolute = 1;
			}
		}
	}
	
	return absolute;
}


/* Information carried by links uses a particular format, avoiding spaces in particular.
	This function modifies the input string to convert to a more friendly format*/

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

			errno = 0;

			int hex = (int)strtol(hexstr, NULL, 16);

			// Google calendar seemingly allows any ASCII value to be passed as string data in a link
			// so the decoding reflects this.

			// If strtol fails it will set the errno to a number other than 0
			// Otherwise it will return the value of the string as a hex value.
			if (hex != 0 || errno == 0) {
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

 // https://www.google.com/calendar/event?action=TEMPLATE&dates=20260610T173000/20260610T200000&text=Community%20Climbing%20Meet%20Up%20%7C%20Golden%2C%20CO&details=Our+local+programs+provide+multiple+opportunities+for+indoor+and+outdoor+climbing+throughout+Colorado.+We+offer+our+bi-monthly+climbing+meet+ups%2C+focused+on+building+community%2C+staying+connected+and+having+fun.+Our+monthly+meet+ups+are+designed+for+all+abilities.If+you+have+questions+about+the+event%2C+please+contact+our+National+Program+Manager%2C+Sam+Sala+at%C2%A0%3Ca+href%3D%22mailto%3Asam%40paradoxsports.org%22+target%3D%22_blank%22+rel%3D%22noopener%22+data-cke-saved-href%3D%22mailto%3Asam%40paradoxsports.org%22%3Esam%40paradoxsports.org%3C%2Fa%3E.Location%3A+Golden%2C+CO+%28exact+location+will+be+determined+by+group+accessibility+needs+and+details+will+be+emailed+prior+to+event%29.%C2%A0+%2AThis+is+an+outdoor+climbing+event.An+optional+happy+hour+in+Golden+will+take+place+following+climbing.Price%3A+Free%21%3Cstrong%3E%3Cem%3E%2ARegistration+space+is+limited.%C2%A0+Tickets+are+available+on+a+first-come%2C+first-serve+basis.%C2%A0%3C%2Fem%3E%3C%2Fstrong%3E%5Bfusion_button+link%3D%22https%3A%2F%2Fsecure.qgiv.com%2Ffor%2FMRLAK29HQWDN9CM7EBT1%2Fevent%2Fgoldenjune10%2F%22+title%3D%22%22+%28View+Full+event+Description+Here%3A+https%3A%2F%2Fwww.paradoxsports.org%2Fcalendar%2Fcommunity-climbing-meet-up-golden-co-3%2F%29&trp=false&ctz=America/Denver&sprop=website:https://www.paradoxsports.org

 // http://www.google.com/calendar/event?action=TEMPLATE&text=Cycling+Center+Thursday+Open+Hours&dates=20260604T190000Z/20260605T010000Z&details=Come+Ride+With+Us%21++%0D%0APlease+call+or+email+to+make+an+appointment%3A+%0D%0A510-848-2930+or+cycling%40borp.org&location=Aquatic+Park%2C80+Bolivar+Drive%2CBerkeley%2CCA%2CUnited+States+of+America+94710
/* Given a Google calendar link, parses data into the instance. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, will override fields. */

// If the link mixes UTC and local when storing start and end date this function will return 


void parseGoogleCalendarLink(struct tempinstance* instance, char* link) {

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
		instance = calloc(1, sizeof(struct tempinstance));
	}

	// START OF TITLE PARSING //

	// We know we will find an index in the string, as "&text=" is required in the link, and 
	// we already verified that this will not return -1
	size_t start_index = strfind(link, "&text="); // param starts at start_index + 6
	size_t end_index;							  // param ends at end_index - 1
												  // param has length end_index - start_index - 6

	end_index = strfindfrom(link, "&", start_index + 6);
	if (end_index == -1) {
		end_index = strlen(link) - 1;
	}

	if (end_index - start_index - 6 >= 0) { // only collects data if it is possible

		char * title = calloc(end_index - start_index - 5, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(title, link + start_index + 6, end_index - start_index - 6);
		*(title + end_index - start_index - 6) = '\0';

		parseLinkString(&title);

		if (strlen(title) >= NAMESIZE - 1) {
			title = realloc(title, NAMESIZE);
			*(title + NAMESIZE) = '\0';
		}
		// frees space to overwrite instance variable
		if (instance->name == NULL) {
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
		end_index = strlen(link) - 1;
	}

	int datelen = end_index - start_index - 7;

	if (datelen >= 17) {

		char * date = calloc(datelen + 1, sizeof(char));

		printf("datelen: %d, end_index: %ld, start_index: %ld\n", datelen, end_index, start_index);

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
		} 
		// it might be worth returning if we can't find a "/" here, as that necessarily invalidates the link.
		// However, even if the date format is corrupted, it still might be worth trying to retrieve other data.
	}

	// END OF DATE PARSING //

	// START OF DESCRIPTION PARSING //
	
	start_index = strfind(link, "&details=");

	if (start_index != -1) {
		end_index = strfindfrom(link, "&", start_index + 8); // desc starts at start_index + 9
		if (end_index == -1) end_index = strlen(link) - 1;	 // desc ends at end_index - 1
															 // desc has length end_index - start_index - 9

		if (end_index - start_index - 9 >= 0) {
			int tempsize = end_index - start_index - 9;
			/* our maximum description size (at time of writing) is 1023 (2^10 - 1). If half the characters in the 
			description are percent-encoded, then that size 1023 will map to 2046 in the undecoded string. If the 
			description in the link is larger, then it must be truncated as a necessary casualty of database storage 
			being limited. Otherwise, if the decoded version would be comprised of half non-alphanumeric values, I 
			will assume it is mostly garbage and fine to truncate. However, if space permits, you can always set the 
			max value of tempsize to 3 times the maximum description size to allow such junk inputs. */ 

			// also I am truncating here and not in title because titles will usually have shorter lengths,
			// while I imagine descriptions could get much much longer.
			if (tempsize > (2 * DESCSIZE - 2)) tempsize = 2 * DESCSIZE - 2;

			char * details = calloc(tempsize + 1, sizeof(char));
			
			memcpy(details, link + start_index + 9, tempsize);
			*(details + tempsize) = '\0';

			parseLinkString(&details);

			if(strlen(details) >= DESCSIZE - 1) {
				details = realloc(details, DESCSIZE);
				*(details + DESCSIZE - 1) = '\0';
			}

			if (instance->description != NULL) free(instance->description);
			instance->description = details;
		}

	}

	// END OF DESCRIPTION PARSING //

	// START OF LOCATION PARSING //

	start_index = strfind(link, "&location=");

	if (start_index != -1) {
		end_index = strfindfrom(link, "&", start_index + 9); 
		if (end_index == -1) end_index = strlen(link) - 1;

		if (end_index - start_index - 10 >= 0) {
			char* locationstring = calloc(end_index - start_index - 9, sizeof(char));

			memcpy(locationstring, link + start_index + 10, end_index - start_index - 10);
			*(locationstring + end_index - start_index - 10) = '\0';

			parseLinkString(&locationstring);

			// PLACEHOLDER WARNING UNTIL STRING TO LOCATION PARSING IS POSSIBLE
			printf("WARNING! Due to a lack of a string to location parser, this link is ignoring useful information!\nPlease fix. Information lost: Location: %s\n", locationstring);
			free(locationstring);
		}
	}
}




int main(int argc, char** argv) {

	struct tempinstance * instance = calloc(1, sizeof(struct tempinstance));

	parseGoogleCalendarLink(instance, "google.com/calendar/event?action=TEMPLATE&text=&dates="); // bad link!!

	printf("name = %s, start month = %d, end month = %d, start year = %d, end year = %d, start day = %d, end day = %d, start time = %d, end time = %d, description = %s\n\n", instance->name, instance->start_month, instance->end_month, instance->start_year, instance->end_year, instance->start_day, instance->end_day, instance->start_time, instance->end_time, instance->description);
	
	parseGoogleCalendarLink(instance, "http://www.google.com/calendar/event?action=TEMPLATE&text=Cycling+Center+Thursday+Open+Hours&dates=20260604T190000Z/20260605T010000Z&details=Come+Ride+With+Us\%21++\%0D\%0APlease+call+or+email+to+make+an+appointment\%3A+\%0D\%0A510-848-2930+or+cycling\%40borp.org&location=Aquatic+Park\%2C80+Bolivar+Drive\%2CBerkeley\%2CCA\%2CUnited+States+of+America+94710");

	printf("name = %s, start month = %d, end month = %d, start year = %d, end year = %d, start day = %d, end day = %d, start time = %d, end time = %d, description = %s\n\n", instance->name, instance->start_month, instance->end_month, instance->start_year, instance->end_year, instance->start_day, instance->end_day, instance->start_time, instance->end_time, instance->description);

	// attempt to reparse

	parseGoogleCalendarLink(instance, "https://www.google.com/calendar/event?action=TEMPLATE&dates=20260610T173000/20260610T200000&text=Community\%20Climbing\%20Meet\%20Up\%20\%7C\%20Golden\%2C\%20CO&details=Our+local+programs+provide+multiple+opportunities+for+indoor+and+outdoor+climbing+throughout+Colorado.+We+offer+our+bi-monthly+climbing+meet+ups\%2C+focused+on+building+community\%2C+staying+connected+and+having+fun.+Our+monthly+meet+ups+are+designed+for+all+abilities.If+you+have+questions+about+the+event\%2C+please+contact+our+National+Program+Manager\%2C+Sam+Sala+at\%C2\%A0\%3Ca+href\%3D\%22mailto\%3Asam\%40paradoxsports.org\%22+target\%3D\%22_blank\%22+rel\%3D\%22noopener\%22+data-cke-saved-href\%3D\%22mailto\%3Asam\%40paradoxsports.org\%22\%3Esam\%40paradoxsports.org\%3C\%2Fa\%3E.Location\%3A+Golden\%2C+CO+\%28exact+location+will+be+determined+by+group+accessibility+needs+and+details+will+be+emailed+prior+to+event\%29.\%C2\%A0+\%2AThis+is+an+outdoor+climbing+event.An+optional+happy+hour+in+Golden+will+take+place+following+climbing.Price\%3A+Free\%21\%3Cstrong\%3E\%3Cem\%3E\%2ARegistration+space+is+limited.\%C2\%A0+Tickets+are+available+on+a+first-come\%2C+first-serve+basis.\%C2\%A0\%3C\%2Fem\%3E\%3C\%2Fstrong\%3E\%5Bfusion_button+link\%3D\%22https\%3A\%2F\%2Fsecure.qgiv.com\%2Ffor\%2FMRLAK29HQWDN9CM7EBT1\%2Fevent\%2Fgoldenjune10\%2F\%22+title\%3D\%22\%22+\%28View+Full+event+Description+Here\%3A+https\%3A\%2F\%2Fwww.paradoxsports.org\%2Fcalendar\%2Fcommunity-climbing-meet-up-golden-co-3\%2F\%29&trp=false&ctz=America/Denver&sprop=website:https://www.paradoxsports.org");

	printf("name = %s, start month = %d, end month = %d, start year = %d, end year = %d, start day = %d, end day = %d, start time = %d, end time = %d, description = %s\n\n", instance->name, instance->start_month, instance->end_month, instance->start_year, instance->end_year, instance->start_day, instance->end_day, instance->start_time, instance->end_time, instance->description);

	freeinstance(instance);

	return 0;
}

