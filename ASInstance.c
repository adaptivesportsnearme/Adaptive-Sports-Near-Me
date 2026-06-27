/*  The basic backend data container for a unique sports instance. This container
    may be used across the data collection and cleaning process, up until data is 
    pushed into the database.*/

/*  An "Adaptive Sports Instance" is defined as an event with a definite date, time and 
    location that corresponds to an open opportunity to participate in a single, definite 
    kind of adaptive sport for a particular skill level, for a single age range at a single 
    cost.
    
        --- Not an ASI ---
        Example: Every Saturday at 2pm, four friends play a game of adaptive 
        pickleball at Oz Park, in Chicago, IL. This is NOT an adaptive sports 
        instance for our purposes, because it is not open --- an outside party 
        is not permitted to join, or at the very least these four friends are 
        not expecting this.
        
        --- Not an ASI ---
        Example: The Rafael Racquet Club (San Anselmo, CA 94960) is hosting 
        an adaptive sports jamboree on 8/19/2027, where people may come for 
        free to try out power soccer, para badminton, and adaptive rock 
        climbing. This is NOT an adaptive sports instance, since multiple kinds 
        of sports are offered. However, this general event may be split into 
        at least three adaptive sports instances, one for each kind of sport.
        
        --- Not an ASI ---
        Example: On the 31st of this month at noon, the San Geronimo Golf 
        Course will be hosting an adaptive golf event for $20. Participants 
        will split into two groups -- one group for beginner through 
        intermediate golfers and another for intermediate through advanced 
        golfers.  This is NOT an adaptive sports instance, as it contains 
        two distinct groups for different skill levels (even though both 
        are open to intermediate golfers!). However, each group represents 
        a single adaptive sports instance.
        
        --- A valid ASI ---
        Example: On 10/31/2026 at 6pm, AS San Francisco is hosting an adaptive yoga 
        event at Dolores Park, which is open to all ages and skill levels. This IS an 
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


/*  A maximum length for certain strings. Note that because this uses c-style
    strings, the maximum length string stored in a variable is one less then 
    the defined space in bytes. These values will likely be redecided later, once
    we get an idea for a typical ASI. */ 

// Remember that c-style strings require a terminating character,
// so these should be one larger than the corresponding attribute 
// size in the database.
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

#include "ASInstance.h"


// -------------------- Print Functions -------------------- //
// A family of functions for printing the information stored in
// the structs specified earlier. These take up multiple lines
// and are best used for debugging. 


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



// -------------------- Clear Functions -------------------- //
// Functions to clear dynamically allocated memory stored in a given
// instance. Very helpful to use before freeing an instance.

// Takes a pointer to an ASInstance as input, and frees all dynamically
// allocated memory associated within the instance
void clearASInstance(ASInstance *inst) {

    if (inst == NULL) return;

    if (inst->name != NULL) free(inst->name);
    if (inst->description != NULL) free(inst->description);
    if (inst->sport_name != NULL) free(inst->sport_name);
    if (inst->organization_id != NULL) free(inst->organization_id);
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



// -------------------- Helper Functions -------------------- //
// Functions for internal use within this module. 

/* A function that takes a pointer to a string and length as input.
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

    memcpy(retstr, str, strlen(str)); // calloc guarantees a '\0'
    return retstr;
}


// ---------------- Naive Data Cleaning -------------------- //
// Ensures that dynamically allocated data stored in an ASI is
// within the specified space limits.

// Formats all dynamic fields to within their specified memory limits
void ASInstanceFormatMemory(ASInstance* inst) {

    if (inst == NULL) {
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



// -------------------- Merge Functions -------------------- //
// Functions to move data from one object to another. Merging is
// less rigid than copying because it never overwrites data.

/*  Takes two ASInstances as input. If src has a value in any field
    where dest has a default value, dest will be given the value
    of src's field. Note that this gives dest priority when
    both instances have non-default values, so use caution! Dynamic 
    values are copied into a new memory block to avoid unintentional 
    frees. */
void mergeASInstance(ASInstance *dest, ASInstance *src) {
    if (dest->name == NULL && src->name != NULL) dest->name = ASInstanceCopyString(src->name);
    if (dest->description == NULL && src->description != NULL) dest->description = ASInstanceCopyString(src->description);
    if (dest->sport_name == NULL && src->sport_name != NULL) dest->sport_name = ASInstanceCopyString(src->sport_name);
    if (dest->organization_id == NULL && src->organization_id != NULL) dest->organization_id = ASInstanceCopyString(src->organization_id);
    if (dest->zip == NULL && src->zip != NULL) dest->zip = ASInstanceCopyString(src->zip);
    if (dest->city == NULL && src->city != NULL) dest->city = ASInstanceCopyString(src->city);
    if (dest->state == NULL && src->state != NULL) dest->state = ASInstanceCopyString(src->state);
    if (dest->country == NULL && src->country != NULL) dest->country = ASInstanceCopyString(src->country);
    if (dest->address_line_1 == NULL && src->address_line_1 != NULL) dest->address_line_1 = ASInstanceCopyString(src->address_line_1);
    if (dest->address_line_2 == NULL && src->address_line_2 != NULL) dest->address_line_2 = ASInstanceCopyString(src->address_line_2);
    if (dest->repeat == NULL && src->repeat != NULL) dest->repeat = ASInstanceCopyString(src->repeat);
    if (dest->timezone == NULL && src->timezone != NULL) dest->timezone = ASInstanceCopyString(src->timezone);

    if (dest->start_day == 0 && src->start_day != 0) dest->start_day = src->start_day;
    if (dest->start_month == 0 && src->start_month != 0) dest->start_month = src->start_month;
    if (dest->start_year == 0 && src->start_year != 0) dest->start_year = src->start_year;
    if (dest->start_time == 0 && src->start_time != 0) dest->start_time = src->start_time;

    if (dest->end_day == 0 && src->end_day != 0) dest->end_day = src->end_day;
    if (dest->end_month == 0 && src->end_month != 0) dest->end_month = src->end_month;
    if (dest->end_year == 0 && src->end_year != 0) dest->end_year = src->end_year;
    if (dest->end_time == 0 && src->end_time != 0) dest->end_time = src->end_time;

    if (dest->UTC_time == 0 && src->UTC_time != 0) dest->UTC_time = src->UTC_time;
    if (dest->allows_advanced == 0 && src->allows_advanced != 0) dest->allows_advanced = src->allows_advanced;
    if (dest->allows_beginners == 0 && src->allows_beginners != 0) dest->allows_beginners = src->allows_beginners;
    if (dest->allows_intermediate == 0 && src->allows_intermediate != 0) dest->allows_intermediate = src->allows_intermediate;
    if (dest->cost == 0 && src->cost != 0) dest->cost = src->cost;
    if (dest->age_ceil == 0 && src->age_ceil != 0) dest->age_ceil = src->age_ceil;
    if (dest->age_floor == 0 && src->age_floor != 0) dest->age_floor = src->age_floor;
}


// Note that there are no functions to merge one ASTime to another, or
// one ASLocation to another. If you'd like to do:

/*      mergeASTimeToASTime(time1, time2);
        mergeASTime(inst, time1);

    You can instead

        mergeASTime(inst, time1);
        mergeASTime(inst, time2);

    If you'd instead like to copy the merged times, simply do

        copyASTime(inst, time1);
        mergeASTime(inst, time1);

*/


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


// -------------------- Copy Functions -------------------- //

// Copies all data from src into dest, deleting any data stored in 
// dest
void copyASInstance(ASInstance* dest, ASInstance* src) {
    clearASInstance(dest);

    dest->id = src->id;

    if (src->name != NULL) dest->name = ASInstanceCopyString(src->name);
    if (src->description != NULL) dest->description = ASInstanceCopyString(src->description);
    if (src->sport_name != NULL) dest->sport_name = ASInstanceCopyString(src->sport_name);
    if (src->organization_id != NULL) dest->organization_id = ASInstanceCopyString(src->organization_id);
    if (src->zip != NULL) dest->zip = ASInstanceCopyString(src->zip);
    if (src->city != NULL) dest->city = ASInstanceCopyString(src->city);
    if (src->state != NULL) dest->state = ASInstanceCopyString(src->state);
    if (src->country != NULL) dest->country = ASInstanceCopyString(src->country);
    if (src->address_line_1 != NULL) dest->address_line_1 = ASInstanceCopyString(src->address_line_1);
    if (src->address_line_2 != NULL) dest->address_line_2 = ASInstanceCopyString(src->address_line_2);
    if (src->timezone != NULL) dest->timezone = ASInstanceCopyString(src->timezone);
    if (src->repeat != NULL) dest->repeat = ASInstanceCopyString(src->repeat);

    dest->start_day = src->start_day;
    dest->start_month = src->start_month;
    dest->start_year = src->start_year;
    dest->start_time = src->start_time;

    dest->end_day = src->end_day;
    dest->end_month = src->end_month;
    dest->end_year = src->end_year;
    dest->end_time = src->end_time;

    dest->UTC_time = src->UTC_time;
    dest->allows_beginners = src->allows_beginners;
    dest->allows_intermediate = src->allows_intermediate;
    dest->allows_advanced = src->allows_advanced;

    dest->cost = src->cost;
    dest->age_floor = src->age_floor;
    dest->age_ceil = src->age_ceil;
    

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
// will be freed if they have a value.
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