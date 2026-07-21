#ifndef ASINSTANCE_H
#define ASINSTANCE_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


// --------------- Adaptive Sports Instance ---------------- //
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


// --------------- Adaptive Sports Full Time ---------------- //
/* It might often be useful to collect a narrow type of data 
   without bringing in the overhead associated with using a 
   full ASInstance. This is a class for collecting time data to later
   be merged or copied into a full ASInstance. 
*/

/* Note that timezone is stored in both ASFullTime and ASLocation. It clearly relates to time
   but also determines location within a region, so both structs may handle it.
*/
typedef struct {
    int start_time, end_time, start_month, start_day, start_year, end_month, end_day, end_year, UTC_time;
    char * repeat, *timezone;
} ASFullTime;


// --------------- Adaptive Sports Time ---------------- //
/* This is a small class for collecting time data to later
   be merged or copied into a full ASInstance.'s start or
   end time data.
*/
typedef struct {
    int time, month, day, year, UTC_time;
} ASTime;


// --------------- Adaptive Sports Location ---------------- //
// A smaller struct for storing only location information, to later be merged into an ASInstance.
typedef struct {
    char *zip, *city, *state, *country,
		*address_line_1, *address_line_2;
    char * timezone;
} ASLocation;


// -------------------- Print Functions -------------------- //
/* A family of functions for printing the information stored in
   the structs specified earlier. These take up multiple lines
   and are best used for debugging. 
*/ 


// prints all fields of a given ASInstance
void printASInstance(ASInstance* inst);

// Prints all attributes of a given ASFullTime
void printASFullTime(ASFullTime* time);

// Prints all attributes of a given ASTime
void printASTime(ASTime* time);

// prints all fields of a given ASLocation
void printASLocation(ASLocation* location);


// -------------------- Free Functions -------------------- //
/* Functions to free dynamically allocated memory stored in a given
   instance. Very helpful to use before freeing an instance.
*/ 

/* Takes a pointer to an ASInstance as input, and frees all dynamically
   allocated memory associated within the instance, including the instance
*/ 
void freeASInstance(ASInstance *inst);

/* Takes a pointer to an ASFullTime as input, and frees all dynamically
   allocated memory associated within the ASFullTime, including the ASFullTime
*/ 
void freeASFullTime(ASFullTime* time);

/* Takes a pointer to an ASTime as input, and frees the ASTime
*/ 
void freeASTime(ASTime* time);

/* Takes a pointer to an ASLocation as input, and frees all dynamically
   allocated memory associated within the ASLocation, including the ASLocation
*/
void freeASLocation(ASLocation* location);


// -------------------- Clear Functions -------------------- //
/* Functions to reset an instance's fields back to their default values,
   freeing dynamically allocated memory.
*/ 

/* Takes a pointer to an ASInstance as input, and resets the ASInstance back
   to its default values, freeing any dynamically allocated memory.
*/ 
void clearASInstance(ASInstance *inst);

/* Takes a pointer to an ASFullTime as input, and resets the ASFullTime back
   to its default values, freeing any dynamically allocated memory.
*/ 
void clearASFullTime(ASFullTime* time);

/* Takes a pointer to an ASTime as input, and resets the ASTime back
   to its default values.
*/ 
void clearASTime(ASTime* time);

/* Takes a pointer to an ASLocation as input, and resets the ASLocation back
   to its default values, freeing any dynamically allocated memory.
*/ 
void clearASLocation(ASLocation* location);


// ---------------- Naive Data Cleaning -------------------- //

/* Formats all dynamic fields to within their specified memory limits,
   and removes any excess space at the end of strings
*/ 
void ASInstanceFormatMemory(ASInstance* inst);


// -------------------- Merge Functions -------------------- //
/* Functions to move data from one object to another. Merging is
   less rigid than copying because it never overwrites data.
*/ 

/*  Takes two ASInstances as input. 

    If src has a value in any field where dest has a default value, 
    dest will be given the value of src's field. Note that this 
    gives dest priority when both instances have non-default values, 
    so use caution! Dynamic values are copied into a new memory block 
     */
void mergeASInstance(ASInstance *dest, ASInstance *src);

/* Note that there are no functions to merge one ASFullTime to another, or
   one ASLocation to another. If you'd like to do:

        mergeASFullTimeToASFullTime(time1, time2);
        mergeASFullTime(inst, time1);

    for instance, you can instead do:

        mergeASFullTime(inst, time1);
        mergeASFullTime(inst, time2);

    If you'd instead like to copy the merged times, simply do:

        copyASFullTime(inst, time1);
        mergeASFullTime(inst, time1);

*/

/* Modifies inst such that for any time field where the ASInstance has
   a default value and the ASFullTime has a non-default value, inst will
   copy ASFullTime's value for that field.
*/
void mergeASFullTime(ASInstance * inst, ASFullTime * time);

/* Modifies inst such that for any starting time field where the ASInstance has
   a default value and the ASTime has a non-default value, inst will
   be given ASFullTime's value for that starting field.
*/
void mergeASTimeStart(ASInstance * inst, ASTime * time);

/* Modifies inst such that for any ending time field where the ASInstance has
   a default value and the ASTime has a non-default value, inst will
   be given ASFullTime's value for that ending field.
*/
void mergeASTimeEnd(ASInstance * inst, ASTime * time);

/* Modifies inst such that for any time field where the ASInstance has
   a default value and the ASFullTime has a non-default value, inst will
   be given ASFullTime's value for that field.
*/
void mergeASLocation(ASInstance * inst, ASLocation * location);


// -------------------- Push Functions -------------------- //
/*  Functions to move data from one object to another. Pushing is
    similar to merging, but pushing overwrites data in the target
    object, except where the source object has the default value.
*/ 

/*  Takes two ASInstances as input. 

    For any field in src with a non-default value, 
    dest will be given the value of src's field. Note that this 
    gives src priority when both instances have non-default values, 
    so use caution! Dynamic values are copied into a new memory block.
*/
void pushASInstance(ASInstance *dest, ASInstance *src);


/* Modifies inst such that for any time field where ASFullTime has a non-default value, 
   inst willcopy ASFullTime's value for that field. Dynamic values are copied into a new memory block.
*/
void pushASFullTime(ASInstance * inst, ASFullTime * time);

/* Modifies inst such that for any starting time field where ASTime has a 
   non-default value, inst will copy ASFullTime's value for that starting field.
   Dynamic values are copied into a new memory block.
*/
void pushASTimeStart(ASInstance * inst, ASTime * time);

/* Modifies inst such that for any ending time field where ASTime has a 
   non-default value, inst will copy ASFullTime's value for that ending field.
   Dynamic values are copied into a new memory block.
*/
void pushASTimeEnd(ASInstance * inst, ASTime * time);

/* Modifies inst such that for any time field where the ASLocation 
   has a non-default value, inst will copy ASFullTime's value for that field.
   Dynamic values are copied into a new memory block.
*/
void pushASLocation(ASInstance * inst, ASLocation * location);


// -------------------- Copy Functions -------------------- //

/* Copies all data from src into dest, deleting any data stored in 
   dest
*/
void copyASInstance(ASInstance* dest, ASInstance* src);

/* Copies all fields in the ASFullTime to their respective fields in inst, overwriting
   all time fields in inst. Any dynamically allocated time variables in inst will be freed and 
   overwritten.
*/
void copyASFullTime(ASInstance * inst, ASFullTime * time);

/* Copies all fields in the ASTime to their respective starting fields in inst, overwriting
   all time fields in inst. Any dynamically allocated time variables in inst will be freed and 
   overwritten.

   (time->month ==> inst->start_month, time->year ==> inst->start_month, etc.)
*/
void copyASTimeStart(ASInstance * inst, ASTime * time);

/* Copies all fields in the ASTime to their respective ending fields in inst, overwriting
   all time fields in inst. Any dynamically allocated time variables in inst will be freed and 
   overwritten.

   (time->month ==> inst->end_month, time->year ==> inst->end_month, etc.)
*/
void copyASTimeEnd(ASInstance * inst, ASTime * time);

/* Copies all fields in the ASLocation to their respective fields in inst, overwriting
   all location fields in inst. Any dynamically allocated location variables in inst will be freed and 
   overwritten.
*/
void copyASLocation(ASInstance * inst, ASLocation * location);


#endif