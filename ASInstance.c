/*  The basic backend data container for a unique sports instance. This container
    may be used across the data collection and cleaning process, up until data is 
    pushed into the database.*/

/*  An "Adaptive Sports Instance" is defined as an event with a definite date, time and 
    location that corresponds to an open opportunity to participate in a single, definite 
    kind of adaptive sport for a particular skill level, for a single age range at a single 
    cost.
    
        Example: Every Saturday at 2pm, four friends play a game of adaptive 
        Pickleball at Oz Park, in Chicago, IL. This is not an adaptive sports 
        instance for our purposes, because it is not open --- an outside party 
        is not permitted to join, or at the very least these four friends are 
        not expecting this.
        
        Example: The Rafael Racquet Club (San Anselmo, CA 94960) is hosting 
        an adaptive sports jamboree on 8/19/2027, where people may come for 
        free to try out Power Soccer, Para Badminton, and Adaptive Rock 
        climbing. This is not an adaptive sports instance, since multiple kinds 
        of sports are offered. However, this general event may be split into 
        at least three adaptive sports instances, one for each kind of sport.
        
        Example: On the 31st of this month at noon, the San Geronimo Golf 
        Course will be hosting an Adaptive golf event for $20. Participants 
        will split into two groups -- one group for beginner through 
        intermediate golfers and another for intermediate through advanced 
        golfers.  This is not an adaptive sports instance, as it contains 
        two distinct groups for different skill levels (even though both 
        are open to intermediate golfers!). However, each group represents 
        a single adaptive sports instance.
        
        Example: On 10/31/2026 at 6pm, AS San Francisco is hosting an adaptive yoga 
        event at Dolores Park, which is open to all ages and skill levels. This is an 
        adaptive sports instance!
    
    These are some good examples, but an adaptive sports instance is best defined by the 
    data model below. If some given event would require multiple values for a given field,
    it is probably not an adaptive sports instance! A single adaptive sports instance should 
    correspond to each permutation of the fields that require multiple values.

        Example: If a given program has a morning and evening event at each 
        of their two locations, this description corresponds to four adaptive
        sports events: a morning ASI at location 1, a morning ASI at location
        2, an evening ASI at location 1, and an evening ASI at location 2
    
    The only notable exceptions are the subjective fields: name and description. The way
    you name or describe an adaptive sports instance has no bearing on the actual event,
    barring serious error in description. If a given event has multiple names or descriptions,
    we should only capture the most clear and accurate one, and discard the rest.*/

/*  Alongside the basic structure of the data type, this module will also include 
    various functions to help with handling it.*/


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*  A maximum length for certain strings. Note that because this uses c-style
    strings, the maximum length string stored in a variable is one less then 
    the defined space in bytes.*/ 
#define NAMESIZE 257
#define DESCSIZE 1025
#define SPORTSize 33
#define ORGIDSIZE 17
#define ZIPSIZE 9
#define CITYSIZE 33
#define STATESIZE 17
#define COUNTRYSIZE 9
#define ADDR1SIZE 129
#define AADR2SIZE 129
#define TZSIZE 33
#define REPEATSIZE 17

typedef struct {

    // An 8-digit hexadecimal id corresponding to this unique instance
    int id;

    // String buffers for the title and description of a given event
    char* name, *description;

    // Attributes that are keys in other databases
    char *sport_name, *organization_id;

    // Address Information
	char *zip, *city, *state, *country,
		*address_line_1, *address_line_2;

    // time information
	int start_time, end_time, start_month, start_day, start_year, end_month, end_day, end_year;
	char * timezone, *repeat;

    // booleans
	int UTC_time, allows_beginners, allows_intermediate, allows_advanced;

    // numerical data about the instance
    int age_floor, age_ceil;
    int cost;

} ASInstance;


// Takes a pointer to an ASInstance as input, and frees all dynamically
// allocated memory associated with 
void freeASInstance(ASInstance *inst) {

    if (inst == NULL) return;

    if (inst->name != NULL) free(inst->name);
    if (inst->description != NULL) free(inst->description);
    if (inst->sport_name != NULL) free(inst->organization_id);
    if (inst->zip != NULL) free(inst->zip);
    if (inst->city != NULL) free(inst->city);
    if (inst->state != NULL) free(inst->state);
    if (inst->country != NULL) free(inst->country);
    if (inst->address_line_1 != NULL) free(inst->address_line_1);
    if (inst->address_line_2 != NULL) free(inst->address_line_2);
    if (inst->timezone != NULL) free(inst->timezone);
    if (inst->repeat != NULL) free(inst->repeat);

    free(inst);

}