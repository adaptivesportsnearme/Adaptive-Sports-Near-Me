/* A module to handle parsing google calendar information */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* In the future we will use a shared struct across code.This is just a temporary
	struct while this module is developed */
struct tempinstance {
	char* name,
		zip, city, state, country,
		address_line_1, address_line_2, description;
	short start_time, end_time, month, day, year;
};


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

			int hex = (int)strtol(hexstr, NULL, 16);

			// handles reserved and unreserved percent-encoding characters
			if ((hex >= 0x20 && hex <= 0x40) || (hex >= 0x5B && hex <= 0x60) || (hex >= 0x7B && hex <= 0x7E)) {
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

/* Given a Google calendar link, parses data into the instance. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, will override fields. */


void parseGoogleCalendarLink(struct tempinstance* instance, char* link) {

	if (link == NULL) return;

	// exits if the link is not in google calendar hyperlink form

	// all valid google calendar links must include
		// "action=TEMPLATE" -> this tells the site that the link contains event information
		// "text=" -> for the name of the event
		// "dates=" -> for the time of the event
	if ((strfind(link, "calendar.google.com") == -1 && 
		strfind(link, "google.com/calendar") == -1) || strfind(link, "action=TEMPLATE") == -1) {
		return;
	}

	if (instance == NULL) {
		instance = calloc(1, sizeof(struct tempinstance));
	}

	// START OF TITLE PARSING //
	size_t start_index = strfind(link, "&text="); // param starts at start_index + 6
	size_t end_index;							  // param ends at end_index - 1
												  // param has length end_index - start_index - 6

	if (start_index != -1) {

		end_index = strfindfrom(link, "&", start_index + 5);
		if (end_index == -1) end_index = strlen(link) - 1;

		char * title = calloc(end_index - start_index - 5, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(title, link + start_index + 6, end_index - start_index - 6);
		*(title + end_index - start_index - 6) = '\0';

		parseLinkString(&title);

		// frees space to overwrite instance variable
		if (instance->name == NULL) {
			free(instance->name);
		}

		instance->name = title;
		
	}

	// END OF TITLE PARSING
	// START OF DATE PARSING

	start_index = strfind(link, "&dates="); // param starts at start_index + 7
	        							   // param ends at end_index - 1
										   // param has length end_index - start_index - 7

	if (start_index != -1) {

		end_index = strfindfrom(link, "&", start_index + 6);
		if (end_index == -1) end_index = strlen(link) - 1;

		char * date = calloc(end_index - start_index - 6, sizeof(char));

		// copies the found parameter into a new title string
		memcpy(date, link + start_index + 7, end_index - start_index - 7);
		*(date + end_index - start_index - 7) = '\0';

		// 20260610T173000/20260610T200000
		
	}
										   
}




int main(int argc, char** argv) {

	return 0;
}

