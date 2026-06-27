/* A module to handle parsing google calendar information */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "ASInstance.h"


 /* Given a Google calendar link, parses data into the instance. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. 
    
    If the link mixes UTC and local when storing start and end date this function will set UTC_time
    based off the start date*/
void parseGoogleCalendarLink(ASInstance* instance, char* link);


/* Given a Google calendar link, parses data into the ASTime. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. 
    
    This is a version of parseGoogleCalendarLink modified specifically for ASTime
    
    If the link mixes UTC and local when storing start and end date this function will set UTC_time
    based off the start date*/
void parseGoogleCalendarLinkAST(ASTime* time, char* link);


/* Given a Google calendar link, parses data into the ASLocation. Fields that are not found are
	populated as NULL in the object. If the link is not a Google calendar link, then instance
	will remain untouched. By default, the function will override fields if a new value is found. */

void parseGoogleCalendarLinkASLoc(ASLocation* location, char* link);