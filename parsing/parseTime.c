#include "parseTime.h"
#include "parseUtils.h"

/*
    How to parse time
        All valid fields must either begin the string or have a space before.
        E.G. BJanuary 18 2005 is not a valid date
        Date formats:   (M)M/(D)D/YYYY -> common in US
                        (M)M-(D)D-YYYY
                        Month Day
                        Month Day(th/st/nd)
                        If year not present, next instance of the date
                        Use time.h
        Time formats:   HH:MM
                        HH:MM:SS
                        HH:MM PM/AM
                        H:MM PM/AM
                        H PM/AM

*/

// ----------- MATCHING FUNCTIONS ------------- //

// If the string at *(arr + index) matches a numerical date format
// we return the number of array segments the matched date is comprised of
// The accepted format here is (M)M/(D)D/(YY)YY or (M)M-(D)D-(YY)YY or (M)M/(D)D or (M)M-(D)D
int matchNumDate(char** arr, int index, int size) {

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

    char ** arr;
    int count = splitString(&arr, str);
   

}