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
#include <string.h>

/*  A maximum length for certain strings. Note that because this uses c-style
    strings, the maximum length string stored in a variable is one less then 
    the defined space in bytes.*/ 
#define NAMESIZE 257
#define DESCSIZE 1025
#define SPORTSIZE 33
#define ORGIDSIZE 17
#define ZIPSIZE 9
#define CITYSIZE 33
#define STATESIZE 17
#define COUNTRYSIZE 9
#define ADDR1SIZE 129
#define ADDR2SIZE 129
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

// Note that timezone is stored in both of the below structs. It clearly relates to time
// but also determines location within a region, so both structs may handle it.

// A smaller struct for storing only time information, to later be merged into an ASInstance.
typedef struct {
    int start_time, end_time, start_month, start_day, start_year, end_month, end_day, end_year, UTC_time;
    char * repeat, *timezone;
} ASTime;


// A smaller struct for storing only location information, to later be merged into an ASInstance.
typedef struct {
    char *zip, *city, *state, *country,
		*address_line_1, *address_line_2;
    char * timezone;
} ASLocation;



// prints all fields of a given ASInstance
void printASInstance(ASInstance* inst) {
    if (inst == NULL) return;

    printf("id = %d\nname = %s\ndescription = %s\n", inst->id, inst->name, inst->description);
    printf("sport name = %s\norganization_id = %s\n", inst->sport_name, inst->organization_id);
    printf("zip = %s\ncity = %s\nstate = %s\ncountry = %s\n", inst->zip, inst->city, inst->state, inst->country);
    printf("address line 1 = %s\naddress line 2 = %s\n", inst->address_line_1, inst->address_line_2);
    printf("start year = %d\nend year = %d\nstart month = %d\nend month = %d\nstart day = %d\nend day = %d\nstart time = %d\nend time = %d\n", inst->start_year, inst->end_year, inst->start_month, inst->end_month, inst->start_day, inst->end_day, inst->start_time, inst->end_time);
    printf("timezone = %s\nrepeat = %s\n", inst->timezone, inst->repeat);
    printf("UTC time = %d\nallows beginners = %d\nallows intermediate = %d\nallows advanced = %d\n", inst->UTC_time, inst->allows_beginners, inst->allows_intermediate, inst->allows_advanced);
    printf("age floor = %d\nage ceiling = %d\ncost = %d\n", inst->age_floor, inst->age_ceil, inst->cost);

}


// Prints all attributes of a given ASTime
void printASTime(ASTime* time) {
    if (time == NULL) return;
    
    printf("start year = %d\nend year = %d\nstart month = %d\nend month = %d\nstart day = %d\nend day = %d\nstart time = %d\nend time = %d\n", time->start_year, time->end_year, time->start_month, time->end_month, time->start_day, time->end_day, time->start_time, time->end_time);
    printf("UTC time = %d\n", time->UTC_time);
    printf("timezone = %s\nrepeat = %s\n", time->timezone, time->repeat);

}


// prints all fields of a given ASLocation
void printASLocation(ASLocation* location) {
    if (location == NULL) return;

    printf("zip = %s\ncity = %s\nstate = %s\ncountry = %s\n", location->zip, location->city, location->state, location->country);
    printf("address line 1 = %s\naddress line 2 = %s\n", location->address_line_1, location->address_line_2);
    printf("timezone = %s\n", location->timezone);

}



// Takes a pointer to an ASInstance as input, and frees all dynamically
// allocated memory associated within the instance
void clearASInstance(ASInstance *inst) {

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

}


// Takes a pointer to an ASTime as input, and frees all dynamically
// allocated memory associated within the AST
void clearASTime(ASTime* time) {

    if (time == NULL) return;

    if (time->repeat != NULL) free(time->repeat);
    if (time->timezone != NULL) free(time->timezone);
}


// Takes a pointer to an ASLocation as input, and frees all dynamically
// allocated memory associated within the ASLoc
void clearASLocation(ASLocation* location) {

    if (location == NULL) return;

    if (location->zip != NULL) free(location->zip);
    if (location->city != NULL) free(location->city);
    if (location->state != NULL) free(location->state);
    if (location->country != NULL) free(location->country);
    if (location->address_line_1 != NULL) free(location->address_line_1);
    if (location->address_line_2 != NULL) free(location->address_line_2);
    if (location->timezone != NULL) free(location->timezone);
}



/* A function that takes a string and length as input.
   If the string is longer than the specified length minus one it is
   trimmed within this range. Otherwise, it is trimmed after
   the null character. 
*/
void ASInstanceTrimStringTo(char** string, size_t length) {
    size_t slen = strlen(*string);

    if (slen > length - 1) {
        *string = realloc(*string, length * sizeof(char));
        *(*string + length - 1) = '\0';
    } else {
        *string = realloc(*string, (slen + 1) * sizeof(char));
    }
}

// Returns a reference to a new character buffer with the same content as the input string
char* ASInstanceCopyString(char* str) {
    char * retstr = calloc(strlen(str) + 1, sizeof(char));

    memcpy(retstr, str, strlen(str));
    return retstr;
}

// Formats all dynamic fields to within their specified memory limits
void ASInstanceFormatMemory(ASInstance* inst) {

    printf("%p\n", inst);

    if (inst == NULL) {
        printf("bad\n");
        return;
    };

    if (inst->name != NULL) ASInstanceTrimStringTo(&inst->name, NAMESIZE);
    if (inst->description != NULL) ASInstanceTrimStringTo(&inst->description, DESCSIZE);
    if (inst->sport_name != NULL) ASInstanceTrimStringTo(&inst->sport_name, SPORTSIZE);
    if (inst->organization_id != NULL) ASInstanceTrimStringTo(&inst->organization_id, ORGIDSIZE);
    if (inst->zip != NULL) ASInstanceTrimStringTo(&inst->zip, ZIPSIZE);
    if (inst->city != NULL) ASInstanceTrimStringTo(&inst->city, CITYSIZE);
    if (inst->state != NULL) ASInstanceTrimStringTo(&inst->state, STATESIZE);
    if (inst->country != NULL) ASInstanceTrimStringTo(&inst->country, COUNTRYSIZE);
    if (inst->address_line_1 != NULL) ASInstanceTrimStringTo(&inst->address_line_1, ADDR1SIZE);
    if (inst->address_line_2 != NULL) ASInstanceTrimStringTo(&inst->address_line_2, ADDR2SIZE);
    if (inst->timezone != NULL) ASInstanceTrimStringTo(&inst->timezone, TZSIZE);
    if (inst->repeat != NULL) ASInstanceTrimStringTo(&inst->repeat, REPEATSIZE);
}



/*  Takes two ASInstances as input. If inst2 has a value in any field
    where inst1 has a default value, inst1 will be given the value
    of inst2's field. Note that this gives inst1 priority when
    both instances have non-default values, so use caution! Dynamic 
    values are copied into a new memory block to avoid unintentional 
    frees. */
void mergeASInstance(ASInstance *inst1, ASInstance *inst2) {
    if (inst1->name == NULL && inst2->name != NULL) inst1->name = ASInstanceCopyString(inst2->name);
    if (inst1->description == NULL && inst2->description != NULL) inst1->description = ASInstanceCopyString(inst2->description);
    if (inst1->sport_name == NULL && inst2->sport_name != NULL) inst1->sport_name = ASInstanceCopyString(inst2->sport_name);
    if (inst1->organization_id == NULL && inst2->organization_id != NULL) inst1->organization_id = ASInstanceCopyString(inst2->organization_id);
    if (inst1->zip == NULL && inst2->zip != NULL) inst1->zip = ASInstanceCopyString(inst2->zip);
    if (inst1->city == NULL && inst2->city != NULL) inst1->city = ASInstanceCopyString(inst2->city);
    if (inst1->state == NULL && inst2->state != NULL) inst1->state = ASInstanceCopyString(inst2->state);
    if (inst1->country == NULL && inst2->country != NULL) inst1->country = ASInstanceCopyString(inst2->country);
    if (inst1->address_line_1 == NULL && inst2->address_line_1 != NULL) inst1->address_line_1 = ASInstanceCopyString(inst2->address_line_1);
    if (inst1->address_line_2 == NULL && inst2->address_line_2 != NULL) inst1->address_line_2 = ASInstanceCopyString(inst2->address_line_2);
    if (inst1->repeat == NULL && inst2->repeat != NULL) inst1->repeat = ASInstanceCopyString(inst2->repeat);
    if (inst1->timezone == NULL && inst2->timezone != NULL) inst1->timezone = ASInstanceCopyString(inst2->timezone);

    if (inst1->start_day == 0 && inst2->start_day != 0) inst1->start_day = inst2->start_day;
    if (inst1->start_month == 0 && inst2->start_month != 0) inst1->start_month = inst2->start_month;
    if (inst1->start_year == 0 && inst2->start_year != 0) inst1->start_year = inst2->start_year;
    if (inst1->start_time == 0 && inst2->start_time != 0) inst1->start_time = inst2->start_time;

    if (inst1->end_day == 0 && inst2->end_day != 0) inst1->end_day = inst2->end_day;
    if (inst1->end_month == 0 && inst2->end_month != 0) inst1->end_month = inst2->end_month;
    if (inst1->end_year == 0 && inst2->end_year != 0) inst1->end_year = inst2->end_year;
    if (inst1->end_time == 0 && inst2->end_time != 0) inst1->end_time = inst2->end_time;

    if (inst1->UTC_time == 0 && inst2->UTC_time != 0) inst1->UTC_time = inst2->UTC_time;
    if (inst1->allows_advanced == 0 && inst2->allows_advanced != 0) inst1->allows_advanced = inst2->allows_advanced;
    if (inst1->allows_beginners == 0 && inst2->allows_beginners != 0) inst1->allows_beginners = inst2->allows_beginners;
    if (inst1->allows_intermediate == 0 && inst2->allows_intermediate != 0) inst1->allows_intermediate = inst2->allows_intermediate;
    if (inst1->cost == 0 && inst2->cost != 0) inst1->cost = inst2->cost;
    if (inst1->age_ceil == 0 && inst2->age_ceil != 0) inst1->age_ceil = inst2->age_ceil;
    if (inst1->age_floor == 0 && inst2->age_floor != 0) inst1->age_floor = inst2->age_floor;
}

// Modifies inst such that for any time field where the ASInstance has
// a default value and the ASTime has a non-default value, inst will
// copy ASTime's value for that field.
void mergeASTime(ASInstance * inst, ASTime * time) {
    if (inst->start_day == 0 && time->start_day != 0) inst->start_day = time->start_day;
    if (inst->start_month == 0 && time->start_month != 0) inst->start_month = time->start_month;
    if (inst->start_year == 0 && time->start_year != 0) inst->start_year = time->start_year;
    if (inst->start_time == 0 && time->start_time != 0) inst->start_time = time->start_time;

    if (inst->end_day == 0 && time->end_day != 0) inst->end_day = time->end_day;
    if (inst->end_month == 0 && time->end_month != 0) inst->end_month = time->end_month;
    if (inst->end_year == 0 && time->end_year != 0) inst->end_year = time->end_year;
    if (inst->end_time == 0 && time->end_time != 0) inst->end_time = time->end_time;

    if (inst->UTC_time == 0 && time->UTC_time != 0) inst->UTC_time = time->UTC_time;
    if (inst->repeat == NULL && time->repeat != NULL) inst->repeat = ASInstanceCopyString(time->repeat);
    if (inst->timezone == NULL && time->timezone != NULL) inst->timezone = ASInstanceCopyString(time->timezone);
}


// Modifies inst such that for any time field where the ASInstance has
// a default value and the ASTime has a non-default value, inst will
// copy ASTime's value for that field.
void mergeASLocation(ASInstance * inst, ASLocation * location) {
    if (inst->zip == NULL && location->zip != NULL) inst->zip = ASInstanceCopyString(location->zip);
    if (inst->city == NULL && location->city != NULL) inst->city = ASInstanceCopyString(location->city);
    if (inst->state == NULL && location->state != NULL) inst->state = ASInstanceCopyString(location->state);
    if (inst->country == NULL && location->country != NULL) inst->country = ASInstanceCopyString(location->country);
    if (inst->address_line_1 == NULL && location->address_line_1 != NULL) inst->address_line_1 = ASInstanceCopyString(location->address_line_1);
    if (inst->address_line_2 == NULL && location->address_line_2 != NULL) inst->address_line_2 = ASInstanceCopyString(location->address_line_2);
    if (inst->timezone == NULL && location->timezone != NULL) inst->timezone = ASInstanceCopyString(location->timezone);
}



// Gives all time fields stored in inst the values of the ASTime provided.
// This function is distinct from merge in that the ASTime will override 
// all time values in inst Any dynamically allocated time variables in inst
// will be freed
void copyASTime(ASInstance * inst, ASTime * time) {
    inst->start_day = time->start_day;
    inst->start_month = time->start_month;
    inst->start_year = time->start_year;
    inst->start_time = time->start_time;

    inst->end_day = time->end_day;
    inst->end_month = time->end_month;
    inst->end_year = time->end_year;
    inst->end_time = time->end_time;

    inst->UTC_time = time->UTC_time;
    if (inst->repeat != NULL) free(inst->repeat);
    if (time->repeat != NULL) inst->repeat = ASInstanceCopyString(time->repeat);
    if (inst->timezone != NULL) free(inst->timezone);
    if (time->timezone != NULL) inst->timezone = ASInstanceCopyString(time->timezone);
}


// Gives all location fields stored in inst the values of the ASLocation provided.
// This function is distinct from merge in that the ASLocation will override 
// all location values in inst. Any dynamically allocated location variables in inst
// will be freed if they have a value
void copyASLocation(ASInstance * inst, ASLocation * location) {

    if (inst->zip != NULL) free(inst->zip);
    if (location->zip != NULL) inst->zip = ASInstanceCopyString(location->zip);
    if (inst->city != NULL) free(inst->city);
    if (location->city != NULL) inst->city = ASInstanceCopyString(location->city);
    if (inst->state != NULL) free(inst->state);
    if (location->state != NULL) inst->state = ASInstanceCopyString(location->state);
    if (inst->country != NULL) free(inst->country);
    if (location->country != NULL) inst->country = ASInstanceCopyString(location->country);
    if (inst->address_line_1 != NULL) free(inst->address_line_1);
    if (location->address_line_1 != NULL) inst->address_line_1 = ASInstanceCopyString(location->address_line_1);
    if (inst->address_line_2 != NULL) free(inst->address_line_2);
    if (location->address_line_2 != NULL) inst->address_line_2 = ASInstanceCopyString(location->address_line_2);
    if (inst->timezone != NULL) free(inst->timezone);
    if (location->timezone != NULL) inst->timezone = ASInstanceCopyString(location->timezone);
}

int main(int argc, char ** argv) {

    return 0;
}