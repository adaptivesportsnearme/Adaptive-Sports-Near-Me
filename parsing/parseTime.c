#include "parseTime.h"
#include "parseUtils.h"

// Author: Clara James - claramacjames7@gmail.com

/*
        This module is an incomplete version of a module
        that would parse time information from a provided
        string and retrieve our desired fields. Those fields
        are:
         - Starting and ending year
         - Starting and ending month
         - Starting and ending day
         - Starting and ending time of day (7:00 PM or 19:00, for example)
         - Timezone name, per the tz database
         - Whether or not the provided time is in UTC or local time

        Time Parsing Requirements and heuristics

        All valid fields must either begin the string or have a space before.
        E.G. BJanuary 18 2005 is not a valid date
        Date formats:   (M)M/(D)D/YYYY -> common in US
                        (M)M-(D)D-YYYY
                        Month Day
                        Month Day(th/st/nd)
                        If year not present, next instance of the date
        Time formats:   HH:MM
                        HH:MM:SS
                        HH:MM PM/AM
                        H:MM PM/AM
                        H PM/AM

        Time Parsing Retrieval Methods

        We divide an input string along defined boundaries (spaces and commas
        for example) and try to categorize each segment into one of the fields
        we are looking for. If we see that any type of information is repeated twice,
        we interpret the first two instances as the starting and ending times. We'll
        assign which one is the starting time
        
        If any field appears more than three times we ignore the third appearance 
        onwards, but in general we should not be passing these kinds of strings. 
        We would like to be filtering these kinds of strings out in whatever module 
        is calling this one. Finally, if we see that there is partial information 
        provided, we will want to assume the missing info in most cases. For instance, 
        if today is 8/11/2026, and I give you one of the following:

            - August 14th
            - The 14th
            - Friday (with no other date information)
        
        You should be assuming these refer to 8/14/2026, as this is the next day that
        satisfies the provided information. However, don't worry as much about the 
        last two cases for now, and just focus on inferring year, as that is what is
        most likely to come up. If timezone is missing, we'll assume local time always, 
        and we'll never assume UTC time unless it is explicitly specified. If not provided, 
        we'll find timezone later, from location data.

        We'll also make sure we are making a number of obvious validity checks, such as making
        sure that all dates and times will happen with the Gregorian calendar. In the future, it
        might also be nice to do some work around daylight savings, and standardize behavior there,
        but the time change occurs at 2AM, so we can safely ignore this for now.

        Finally, we'll need to figure out whether we are using 24-hour or 12-hour time. In general,
        since we're working in the US and Canada for now, we'll assume 12-hour time for the US, and
        24-hour time for Canada, but we'll need to figure out how to use location information
        if we parse time before location.  However you decide to represent this ambiguity,
        please let me know about it so we can agree on something that causes as few errors as possible.

        This whole explanation should be a relatively complete explanation of the intent of this
        module, and honestly, it's probably not necessary to refer to the below functions, as they're
        incomplete and working with c-strings. If you want a better understanding of how we're splitting
        and parsing information, parseLocation.c will have a more complete suite of matching, getting,
        and parsing functions. Thank you for reading this description in full. Let me know you did and i'll 
        buy you a candy bar or something. In general, like with the entirety of the project, please try 
        to take a modular approach, and divide key operations into functions, so that we can have more readable
        and more easily fixable code.



*/

// ----------- MATCHING FUNCTIONS ------------- //

// If the string at *(arr + index) matches a numerical date format
// we return the number of array segments the matched date is comprised of
// The accepted format here is (M)M/(D)D/(YY)YY or (M)M-(D)D-(YY)YY or (M)M/(D)D or (M)M-(D)D
int matchNumDate(char** arr, int index, int size) {

    // THIS FUNCTION IS UNFINISHED

    // this one only ever matches single segments, so
    // we'll make a simpler reference to our one segment
    char * str = *(arr + index);

    // our position in the string
    int strpos = 0;

    if (strlen(str) < 3) return 0; // smallest valid date is M/D

    if (!isdigit(*(str + strpos))) return 0; // technically the following if-else block makes this line
                                             // unnecessary, but it's a nice early exit for non-matching strings

    if (*(str + strpos) == '0') {
        strpos++;
        if (!isdigit(*(str + strpos)) || *(str + strpos) == '0') {
            return 0; // 0 is not a valid month
        }
    } else if (*(str + strpos) == '1') { //only 1, 10, 11, and 12 allowed,
        if (isdigit(*(str + strpos))) {
            if(*(str + strpos + 1) > '2') return 0;
            else strpos++;
        } 
    }

    // at this point we start with a valid month

    strpos++;

    if (!(*(str + strpos) == '-' || *(str + strpos) == '/')) {
        return 0;
    }

    if (strpos >= strlen(str) - 1) // we need at least one character after this.

    // at this point we have a valid month-day connecting symbol

    strpos++;

    if (!isdigit(*(str + strpos))) return 0; // first character after connector must be a digit

    if (*(str + strpos) == '0') {
        strpos++;

        if (strpos > strlen(str) - 1) return 0; // exit if we're at the end of the string

        if (!isdigit(*(str + strpos)) || *(str + strpos) == '0') {
            return 0; // 0 is not a valid day
        }
    } else if (*(str + strpos) == '3') { //only 1, 10, 11, and 12 allowed,

        if (strpos + 1 > strlen(str) - 1) return 0;

        if(*(str + strpos + 1) >= '1') return 0;
        else strpos++;
    } else if (*(str + strpos) == '1' || *(str + strpos) == '2') {
        if (strpos + 1 < strlen - 1) {
            if (isdigit(*(str + strpos + 1))) {
                strpos++;
            }
        }
    }

    // at this point we have a valid day field and may exit if nothing is after
    if (strpos > strlen(str) - 1) return 1; // (M)M[/ or -](D)D

    else if (strpos >= strlen(str) - 3) return 0; // we need at least 3 more characters to retrieve a year

    strpos++;

    if (!(*(str + strpos) == '-' || *(str + strpos) == '/')) {
        return 0;
    }

    // at this point we have a valid day-year connector

    strpos++;

    if (strpos == strlen(str) - 4) { // only 4 or 2 digit years are allowed
        for (int i = 0; i < 4; i++) {
            if (!isdigit(*(str + strpos + i))) return 0; // even though we have month and day, a year fragment is not allowed
        }

        // we have 4 digits in a row. We choose to be agnostic about their value, so we return.

        return 1;

    } else if (strpos == strlen(str) - 2) { // 2 digit year
        for (int i = 0; i < 2; i++) {
            if (!isdigit(*(str + strpos + i))) return 0; // even though we have month and day, a year fragment is not allowed
        }

        // we have 2 digits in a row. We choose to be agnostic about their value, so we return.

        return 1;
    }


    return 0; // any arrival here is a bad match, so we return

}

// --------------- GETTING FUNCTIONS ------------------ //

/*  For some input such that matchNumDate(arr, index, size) == 1
    parses its information into the provided ASTime container
*/
void parseNumDate(ASTime * astime, char ** arr, int index) {

    // THIS FUNCTION IS UNFINISHED

    time_t seconds = time(NULL);
    struct tm* currenttime = localtime(seconds);

    int current_year = currenttime->tm_year; // gets the current year at time of execution




    char * str = *(arr + index);

    int strpos = 0;

    int month = atoi(str + strpos);

    // advances to day field
    strpos += 2;
    if (!isdigit(str + strpos)) strpos++; 
    
    int day = atoi(str + strpos);

    if (strpos == strlen(str) - 1) { // if input has no year, we exit here

        astime->day = day;
        astime->month = month;
        return;

    }

    int year = -1;

    // advances to year field
    strpos += 2;
    if (!isdigit(str + strpos)) strpos++; 

    if (strpos == strlen(str) - 2) { // 2 digit year
        year = atoi(str + strpos); // because of the length restriction, 0 <= year <= 99
        // assumes current century -- will start to break around 2096
        year = (current_year / 100) * 100 + year;
    } else if (strpos == strlen(str) - 4) { // 4 digit year
        year = atoi(str + strpos);
    }

    astime->day = day;
    astime->month = month;
    astime->year = year;

    return;

}



void parseTimeASTime(ASTime * time, char * str) {

    // THIS FUNCTION IS UNFINISHED

    char ** arr;
    int count = splitString(&arr, str);
   

}