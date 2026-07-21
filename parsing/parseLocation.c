/*
    This is an address parsing module, designed to extract location fields
    from unformatted strings.

    When a human is given the input

        732849 California Ct.
        Washington, NJ 732848
        USA

    It is fairly easy to parse that information into these fields:

        House number: 732849
        Street: California Ct (Court)
        City: Washington
        State: New Jersey
        ZIP: 732848
        Country: USA

    But that relies a lot on human brain power. For instance, the house number and
    zip code are nearly the same, and California, Ct, Washington, and NJ could all
    refer to states. This module aims to replicate how the human brain leverages information
    position and standard formatting to parse this information
*/

/* 
    In multiple places within this code, the term multistate (or multicity) is used.
    A multistate denotes a state, territory, or province and contains spaces

    For instance "New Jersey", "District of Columbia", and "Northwest Territories" 
    are all multistates, but "Alabama", "DC", and "NT" are not, even though
    "DC" and "NT" refer to the above states that use spaces. The reason for this
    discrepancy is that this is a programming-related label that will not be used outside
    of this module's internal implementation.
*/


#include "parseLocation.h"
#include "locationReference.h"


// ------------- BASIC HELPER FUNCTIONS ---------------------- //


// returns true if the strings are equal -- case insensitive
// and false otherwise
int strEquals(char * str1, char * str2) {
    if (strlen(str1) != strlen(str2)) return 0;

    for (int i = 0; i < strlen(str1); i++) {
        if (toupper(*(str1 + i)) != toupper(*(str2 + i))) return 0;
    }

    return 1;
}

// Taking the given sequence of num words starting at index in arr,
// returns a string pointer that concatenates the words with a joining space.
// NO SAFEGUARDS FOR MEMORY BUFFER INVALID READ -- do a check before passing
char * strCombineSpace(char ** arr, int index, int num) {
    int len = 0;

    // count space needed
    for (int i = 0; i < num; i++) {
        len += strlen(*(arr + index + i));
    }

    // len characters + (num - 1 spaces) + null character
    char* ret = calloc(len + num, sizeof(char));

    len = 0;
    for (int i = 0; i < num; i++) {
        memcpy(ret + len, *(arr + index + i), strlen(*(arr + index + i)));
        len += strlen(*(arr + index + i)) + 1;
        if (i != num - 1) {
            *(ret + len - 1) = ' ';
        }
    }

    return ret;
}

// returns 1 if str is an entry in arr, and 0 otherwise
int strInArr(char * str, char** arr, int array_size) {

    for (int i = 0; i < array_size; i++) {
        if (strEquals(str, (char *)(*(arr + i)))) return 1;
    }

    return 0;
}

// finds the index of str in arr, or returns -1 if str is not in arr
int strFindInArr(char * str, char** arr, int size) {
    for (int i = 0; i < size; i++) {
        if (strEquals(str, *(arr + i))) return i;
    }

    return -1;
}

// returns 1 if a string is only digits, and 0 otherwise
int strIsDigit(char * str) {
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(*(str + i))) return 0;
    }
    return 1;
}

// Given an array and a string, returns the substring on str that
// corresponds to a given series of (len) segments in splitString(str)
// which start at start_index
char * strBuildFromArr(char* str, int start_index, int len) {
    
    if (str == NULL) return NULL;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return NULL; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;


    while (index < start_index) {
        start++;
        if (start >= strlen(str)) return NULL; // return if out of range
        if (isspace(*(str + start)) || *(str + start) == ',') {
            lastwasspace = 1;
        } else {
            if (lastwasspace == 1) {
                index++;
            }
            lastwasspace = 0;
        }
    }

    // start is now at the start of our desired substring, and index = start_index
    // we now find the end of our desired substring

    // the end of our substring
    int end = start;

    // note that index here will change on the shift to whitespace instead of the
    // reverse shift. This is so we don't include unnecessary spaces or commas

    while (index < start_index + len) {
        end++;
        if (end >= strlen(str)) return NULL;
        if (isspace(*(str + end)) || *(str + end) == ',') {
            if (lastwasspace == 0) {
                index++;
            }
            lastwasspace = 1;
        } else {
            lastwasspace = 0;
        }
    }



    // start and end are now at the right places.
    // start is at the first included index
    // end is at the first excluded index

    char * ret = calloc(end - start + 1, sizeof(char));
    memcpy(ret, str + start, end - start);


    return ret;
    
}

// Given a splitString array and its corresponding string, returns the index that is the
// first after a newline or comma. An example of output depending on input index is given below
/*        2   2   4    4     6   6      8      8
        "This is, a string, with a \nnewline break",
*/
int getNextBreakInArr(char* str, int start_index, int size) {
    
    if (str == NULL) return -1;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int letter = 0;

    // advances start to the index of the first non-space character in the string.
    while (letter < strlen(str) && (isspace(*(str + letter)) || *(str + letter) == ',')) {
        letter++;
    }

    if (letter == strlen(str)) return -1; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;

    while (index < start_index) {
        letter++;
        if (letter >= strlen(str)) return -1; // return if out of range
        if (isspace(*(str + letter)) || *(str + letter) == ',') {
            lastwasspace = 1;
        } else {
            if (lastwasspace == 1) {
                index++;
            }
            lastwasspace = 0;
        }
    }

    // letter is now at the start of our desired substring, and index = letter
    // we now find the next index after a newline or comma

    // note that index here will change on the shift to whitespace instead of the
    // reverse shift. This is so we don't include unnecessary spaces or commas

    while (index < size) {
        letter++;
        if (letter >= strlen(str)) {
            if (index == size - 1) return size;
            return -1;
        }
        if (isspace(*(str + letter)) || *(str + letter) == ',') {
            if (lastwasspace == 0) {
                index++;
            }
            if (*(str + letter) == ',' || *(str + letter) == '\n') { // return index if we see a comma or newline
                return index;
            }
            lastwasspace = 1;
        } else {
            lastwasspace = 0;
        }
    }

    return index;
    
}



// Given an array of strings, a starting index, and the size of the array,
// If the index is the first of n entries that comprise a US/CA postal code,
// then the function will return n. Otherwise it will return 0
int matchZip(char ** arr, int index, int size) {
    // matches 12345 and 12345 6789
    if (strlen(*(arr + index)) == 5 && strIsDigit(*(arr + index))) {
        if (index < size - 1) {
            // input is ZIP+4, separated by a space
            if (strlen(*(arr + index + 1)) == 4 && strIsDigit(*(arr + index + 1))) return 2;
        }
        return 1;
    }
        // matches 12345-6789
    if (strlen(*(arr + index)) == 10) {
        if (*(*(arr + index) + 5) != '-') return 0;
        for (int i = 0; i < 5; i++) if (!isdigit(*(*(arr + index) + i))) return 0;
        for (int i = 6; i < 10; i++) if (!isdigit(*(*(arr + index) + i))) return 0;
        return 1; // input is ZIP+4, separated by a hyphen
    }
        // matches Canadian postal code -> [ABCEGHJ-NPRSTV-Y]\d[ABCEGHJ-NPRSTV-Z]\d[ABCEGHJ-NPRSTV-Z]\d
    if (strlen(*(arr + index)) == 6) {
        if (!isdigit(*(*(arr + index) + 1)) || !isdigit(*(*(arr + index) + 3)) || !isdigit(*(*(arr + index) + 5))) return 0;
        if (!isalpha(**(arr + index)) || tolower(**(arr + index)) == 'd' || tolower(**(arr + index)) == 'f'
            || tolower(**(arr + index)) == 'i' || tolower(**(arr + index)) == 'o' || tolower(**(arr + index)) == 'q'
            || tolower(**(arr + index)) == 'u' || tolower(**(arr + index)) == 'z') return 0;
        if (!isalpha(*(*(arr + index) + 2)) || tolower(*(*(arr + index) + 2)) == 'd' || tolower(*(*(arr + index) + 2)) == 'f'
            || tolower(*(*(arr + index) + 2)) == 'i' || tolower(*(*(arr + index) + 2)) == 'o' || tolower(*(*(arr + index) + 2)) == 'q'
            || tolower(*(*(arr + index) + 2)) == 'u') return 0;
        if (!isalpha(*(*(arr + index) + 4)) || tolower(*(*(arr + index) + 4)) == 'd' || tolower(*(*(arr + index) + 4)) == 'f'
            || tolower(*(*(arr + index) + 4)) == 'i' || tolower(*(*(arr + index) + 4)) == 'o' || tolower(*(*(arr + index) + 4)) == 'q'
            || tolower(*(*(arr + index) + 4)) == 'u') return 0;
        return 1; // input is CA postal code
    }
        // matches Canadian postal code -> [ABCEGHJ-NPRSTV-Y]\d[ABCEGHJ-NPRSTV-Z][ ]\d[ABCEGHJ-NPRSTV-Z]\d
    if (strlen(*(arr + index)) == 3) {
        if (!isdigit(*(*(arr + index) + 1))) return 0;
        if (!isalpha(**(arr + index)) || tolower(**(arr + index)) == 'd' || tolower(**(arr + index)) == 'f'
            || tolower(**(arr + index)) == 'i' || tolower(**(arr + index)) == 'o' || tolower(**(arr + index)) == 'q'
            || tolower(**(arr + index)) == 'u' || tolower(**(arr + index)) == 'z') return 0;
        if (!isalpha(*(*(arr + index) + 2)) || tolower(*(*(arr + index) + 2)) == 'd' || tolower(*(*(arr + index) + 2)) == 'f'
            || tolower(*(*(arr + index) + 2)) == 'i' || tolower(*(*(arr + index) + 2)) == 'o' || tolower(*(*(arr + index) + 2)) == 'q'
            || tolower(*(*(arr + index) + 2)) == 'u') return 0;
        if (index < size - 1) {
            if (strlen(*(arr + index + 1)) == 3) {
                if (!isdigit(**(arr + index + 1)) || !isdigit(*(*(arr + index + 1) + 2))) return 0;
                if (!isalpha(*(*(arr + index + 1) + 1)) || tolower(*(*(arr + index + 1) + 1)) == 'd' || tolower(*(*(arr + index + 1) + 1)) == 'f'
                    || tolower(*(*(arr + index + 1) + 1)) == 'i' || tolower(*(*(arr + index + 1) + 1)) == 'o' || tolower(*(*(arr + index + 1) + 1)) == 'q'
                    || tolower(*(*(arr + index + 1) + 1)) == 'u') return 0;
                return 2;
            }
            return 0;
            
        }     
        return 0; 
    }

    return 0;
}


// Given an array of strings, a starting index, and the size of the array,
// if the string at the index is the first of n entries that comprise a US state or 
// Canadian territory or province, then the function will return n. 
// Otherwise, it will return 0.

// this only matches spellings identical to common names.
int matchStateStrong(char** arr, int index, int size) {
    // filters out obvious non-matches
    if (strlen(*(arr + index)) < 2) return 0;
    if (strlen(*(arr + index)) == 3 && !strEquals(*(arr + index), "NEW")) return 0;

    // If input is 2-letter state code (CT, NV, DC, NL)
    if (strlen(*(arr + index)) == 2) {
        for (int i = 0; i < (2 * PARLOCNUMSTATES); i += 2) { // only scans even indices -- the only ones that are 2 letters
            if (strEquals(*(arr + index), (char*)*(states + i))) return 1;
        }
        return 0;
    }

    // At this point the only possible input is a full state name

    // index in the multistateCue array
    int msInd = strFindInArr(*(arr + index), (char**)multistateCue, PARLOCNUMMULTISTATES); 

    if (msInd != -1 && (*(multistateCueLength + msInd) + index - 1 < size)) {
        char * multistate = strCombineSpace(arr, index, *(multistateCueLength + msInd)); // ALLOCATES MEMORY
        for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(multistate, (char*)*(states + i))) {
                free(multistate);
                return *(multistateCueLength + msInd);
            }
        }
        free(multistate);
        return 0;
    }

    for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(*(arr + index), (char*)*(states + i))) return 1; // only scans unabbreviated state names
    }
    return 0;

}


// Given an array of strings, a starting index, and the size of the array,
// If the index is the first of some number of entries that comprise a US/CA postal code,
// then the function will return 1. 
// Otherwise it will return 0
int matchAddress(char ** arr, int index, int size) {

    // All valid addresses must begin with a number. This number may include 
    // fractions or decimals and may be multiple segments.
    // The allowed characters for this first field are 0-9, "-", "/", ".", and " "
    // And a single* letter character may indicate unit
    // slashes can only occur after a hyphen or space
    // a digit must begin the first segment

    // * there could probably be multiple letters, but we'd risk false positives
    // for a minor edge case.

    // there cannot be a 1-length address.
    if (index == size - 1) return 0;

    // the number of segments the address number takes up
    int houseNumSegs = 0; 

    // a flag to check if a slash is a violation
    int slashesAllowed = 0;
    // a flag to check if an address number could be in two parts
    int multiNum = 1;

    // if input does not begin with digit we return false
    if (strlen(*(arr + index)) > 0) {
        if (!isdigit(**(arr + index))) return 0;
    }

    for (int i = 1; i < strlen(*(arr + index)); i++) {
        if (!isdigit(*(*(arr + index) + i))) {
            multiNum = 0; // if the first segment has any non-digit characters, it cannot be
                          // part of a multiple-segment address number -- at least under our model
            if (*(*(arr + index) + i) == '-') slashesAllowed = 1;
            else if (*(*(arr + index) + i) == '/') {
                if (!slashesAllowed) return 0;
            }
            else if (*(*(arr + index) + i) != '.') {
                if (isalpha(*(*(arr + index) + i))) {
                    if (i != strlen(*(arr + index)) - 1) return 0;
                }
            }
        }
    }

    houseNumSegs = 1;
    if (multiNum = 1 && index < size - 2) {

        /*  Note that here we don't check if the first character is a digit,
            because addresses like 221 B baker street are fine.

            But what about ambiguous addresses like 404 E Hastings Blvd?
            Here E could be a directional or a unit. 
            
            It turns out this doesn't actually matter. 
            Our algorithm will mark E as a unit number, but
            this marking never reaches the user. Our algorithm will then determine
            that Hastings is not a directional, and we will still mark the
            correct amount of space, despite the potential mis-marking of E.

            If you have something like 221 B E Hastings Blvd., that's very annoying to read
            but we will still interpret B as a unit number and then E as a directional, so no 
            miscounting ever occurs. Cool!
        */

        for (int i = 0; i < strlen(*(arr + index + 1)); i++) {
            if (!isdigit(*(*(arr + index + 1) + i)) && *(*(arr + index + 1) + i) != '-' 
                && *(*(arr + index + 1) + i) != '/' && *(*(arr + index + 1) + i) != '.'
                && !(isalpha(*(*(arr + index + 1) + i)) && i == strlen(*(arr + index + 1)) - 1)) {
                    multiNum = 0; // mark format as invalid
                    break;
            }
        }
        if (multiNum == 1) houseNumSegs = 2;
    }



    // As long as there is a field after the house number, this counts as a match.
    // This is a very weak matching function, so bear that in mind.
    // I have deleted sections from my initial implementation, but I have
    // left some utilities in the locationReference header if you need to
    // parse the street address more precisely.

    if (index + houseNumSegs < size) {
        for (int i = 0; i < strlen(*(arr + index + houseNumSegs)); i++) {
            if (isalpha(*(*(arr + index + houseNumSegs) + i))) return 1; // street name must have at least one alphabetical character
        }
    }

    return 0;
}

// if there is a series of segments that are a major US/CA city, we return
// the size of the largest series starting at index in arr. If the segment
// does not match, we return 0.
// because some cities like Buffalo are contained in other cities like Wood Buffalo
// scanning from R->L can return an early result, so use caution and prefer L-> scans
int matchCommonCity(char ** arr, int index, int size) {
    char * multi;
    if (index <= size - 3) {
        if (strInArr(*(arr + index), (char**)multiCityThreeCues, PARLOCMULTTHRCITYCUES)) {
            multi = strCombineSpace(arr, index, 3);
            if (strInArr(multi, (char**)commonCities, PARLOCCITIES)) {
                free(multi);
                return 3;
            }
        }
    }
    if (index <= size - 2) {
        if (strInArr(*(arr + index), (char**)multiCityThreeCues, PARLOCMULTWOCITYCUES)) {
            multi = strCombineSpace(arr, index, 2);
            if (strInArr(multi, (char**)commonCities, PARLOCCITIES)) {
                free(multi);
                return 2;
            }
        }
    }

    if (strInArr(*(arr + index), (char**)commonCities, PARLOCCITIES)) {
        return 1;
    }
}

// if there is a series of segments in a valid city name format, we return
// the size of the largest series starting at index in arr. If the first segment
// does not match the format, we return 0
int matchCity(char ** arr, int index, int size) {

    int count = 0;

    // longest us city name in terms of word count is Kinney and Gourlays Improved City Plat
    // barring official names like "La Villa Real de la Santa Fe de San Francisco de Asis" -- which is Santa Fe
    while (count < size && count < 6) {
        for (int i = 0; i < strlen(*(arr + index + count)); i++) {
            if (!isalpha(*(*(arr + index + count) + i)) && *(*(arr + index + count) + i) != '-'
                && *(*(arr + index + count) + i) != '.') {
                    return count;
            }
        }

        count++;
    }
}

int matchCountry(char ** arr, int index, int size) {
    // filters out obvious non-matches
    if (strlen(*(arr + index)) != 2 && strlen(*(arr + index)) != 3 && strlen(*(arr + index)) != 6
                && strlen(*(arr + index)) != 7 && strlen(*(arr + index)) != 13
                && strlen(*(arr + index)) != 24) return 0;

    char * country = NULL;
    int len = 1;

    if (strEquals(*(arr + index), "UNITED")) {

        if (index + 1 >= size) return 0;
        else {
            if (index + 3 < size) {
                if (strEquals(*(arr + index + 2), "OF") && strEquals(*(arr + index + 3), "AMERICA")) {
                    country = strCombineSpace(arr, index, 4);
                    len = 4;
                }
            } 
            if (country == NULL) {
                country = strCombineSpace(arr, index, 2);
                len = 2;
            }
        }
    } else {
        country = calloc(strlen(*(arr + index)) + 1, sizeof(char));
        memcpy(country, *(arr + index), strlen(*(arr + index)));
    }

    if (strInArr(country, (char **  )countries, PARLOCCOUNTRIES)) {
        free(country);
        return len;
    } else {
        free(country);
        return 0;
    }
}


// Returns the canonical form of a given state in new memory
// INPUT MUST BE A MATCHING ZIP, or in other words,
// matchZip(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur.
char * getCanonicalZip(char** arr, int index, int num) {
    if (num == 1) {
        char * ret = calloc(strlen(*(arr + index)) + 1, sizeof(char));
        memcpy(ret, *(arr + index), strlen(*(arr + index)));
        return ret;
    } else {
        if (num == 2 && strlen(*(arr + index)) == 3) { // if input is CA postal code in XXX XXX format
            char * ret = calloc(7, sizeof(char));
            memcpy(ret, *(arr + index), 3);
            memcpy(ret + 3, *(arr + index + 1), 3);
            return ret;
        }
        if (num == 2 && strlen(*(arr + index)) == 5) {
            char * ret = calloc(11, sizeof(char));
            memcpy(ret, *(arr + index), 5);
            *(ret + 5) = '-';
            memcpy(ret + 6, *(arr + index + 1), 4);
            return ret;
        }
        return NULL; // YOU SHOULD NEVER REACH THIS LINE OF CODE
    }

}

// Returns the canonical form of a given state in new memory
// INPUT MUST BE A MATCHING STATE, or in other words,
// matchStateStrong(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur:
    // If the input is already two characters, then it will return those two 
    // characters in new memory REGARDLESS OF IF THEY REPRESENT A STATE.
    // Otherwise NULL will be returned if we do not reach out of bounds
char * getCanonicalState(char ** arr, int index, int num) {

    // If the input is two characters, a copy will be returned in new memory
    if (strlen(*(arr + index)) == 2) {
        char * ret = calloc(3, sizeof(char));
        memcpy(ret, *(arr + index), 2);
        if (islower(*ret)) *ret = toupper(*ret);
        if (islower(*(ret + 1))) *(ret + 1) = toupper(*(ret + 1));
        return ret;
    }

    // handles multistates
    if (num > 1) {
        char * multistate = strCombineSpace(arr, index, num); // ALLOCATES MEMORY
        for (int i = 1; i < (2 * PARLOCNUMSTATES); i += 2) {
            if (strEquals(multistate, (char*)*(states + i))) {
                char * ret = calloc(3, sizeof(char));
                memcpy(ret, (char*)*(states + i - 1), 2);
                free(multistate);
                return ret;
            }
        }
        free(multistate);
        return NULL;
    }
}

// Returns the canonical form of a given country in new memory
// INPUT MUST BE A MATCHING COUNTRY, or in other words,
// matchCountry(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur:
char * getCanonicalCountry(char ** arr, int index, int num) {
    char * combined = strCombineSpace(arr, index, num);
    int posInCntry = strFindInArr(combined, (char**)countries, PARLOCCOUNTRIES);
    char * ret;
    if (posInCntry == 4) {
        ret = calloc(3, sizeof(char));
        sprintf(ret, "CA");
    } else {
        ret = calloc(4, sizeof(char));
        sprintf(ret, "USA");
    }

    return ret;
}


/*  A function that takes a given valid c-string str and splits it along
    whitespaces and commas for easier containment, putting it into dst. 
    Returns the number of pieces in the array, or 0 if the string
    has no non-space characters or is NULL*/
int splitString(char*** dst, char * str) {

    if (str == NULL) return 0;

    // The array of strings we will return
    char ** pieces;

    // the number of splits we will make. This excludes whitespace and commas at 
    // the start and end of a string
    int spaces = 0;
    
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return 0; // returns if passed a string with only spaces and commas

    // counts the number of segments needed to split the string
    int lastwasspace = 0;
    for (int i = start; i < strlen(str); i++) {
        if (isspace(*(str + i))|| *(str + i) == ',') {
            if (lastwasspace == 0) {
                spaces++; 
            }
            lastwasspace = 1;
        } else lastwasspace = 0;
    }
    if (lastwasspace == 1) spaces--; // if the string ends with a space we don't count it

    pieces = calloc(spaces + 1, sizeof(char*));

    // the segment in the string we are currently copying
    int currpiece = 0;

    // an index to store where we are in the given string
    int sind = start;
    // an index to find the end of a given segment
    int eind = sind;

    // splits the string into pieces
    while (currpiece < spaces + 1) {

        // places eind at the first space after *(str + sind), or at the null character if it comes first
        while (*(str + eind) != ',' && !isspace(*(str + eind)) && eind < strlen(str)) {
            eind++;
        }

        // copies the ith string segment into *(pieces + i)
        *(pieces + currpiece) = calloc(eind - sind + 1, sizeof(char));
        memcpy(*(pieces + currpiece), str + sind, eind - sind);
        *(*(pieces + currpiece) + eind - sind) = '\0';

        // readies 
        while ((isspace(*(str + eind)) || *(str + eind) == ',') && eind < strlen(str)) {
            eind++;
        }
        sind = eind;

        currpiece++;
    }

    *dst = pieces;
    return spaces + 1;

}


/*
    Formatting strength & info

        ZIP - must be formatted correctly or information lost --------- done!!
            - usually at the end of an address or by the city or state
            - typically determines state
            - format determines country
        State - finite correctly spelled possibilities, but high possibility for misread ------ done!
              - Determines country
              - usually at end of address and by country
              - country will almost always be after
        Street number - number, ideally in front of street name  -- done
                      - no letters unless there's a letter at the end
                      - usually first field in address
                      - usually not mistakeable for other fields except potentially zip
                      - always before street info
        street prefix - very fixed, has to be direction (can be abbreviated or in spanish though) -- N/A
                      - smallest number of possibilities, but can be a multi-cue for a state
                      - Right after street number if present and right before street
        Street name - Loosest field -- N/A
                    - usually towards start
                    - after street prefix or street number
                    - before street suffix if suffix is present
        Street suffix - wide array of possibilities but still limited -- N/A
                      - can be spanish or english
                      - after street name always
                      - can be omitted
        City - technically limited but often from limited pool
                -> Chicago easily recognizable as city but maybe not Adaven
             - usually before state and after street info
             - technically often obeys certain naming practices
        Country - often omitted
                - in current model, determined by state
                - also determined by zip code
                - if present, likely at the end

    Other formatting heuristics:
        - No segment of text can provide information for two fields other than by determination
        - Information usually goes specific to unspecific
        - information is often clumped
            - City, State, (Country), ZIP usually together at end
            - Street info usually together at start
            - City and State usually clumped in middle
        - if multiple segments could be the same field, the segment with
          the least amount of confusion is likely that field
        - No field is represented more than once in general
        - Address information is usually all in a row 
            -> very rarely will there be irrelevant information in between
               the address information
        - A single field is usually not split by a newline

*/




/*
    Improvements -> always assumes city starts after suffix
    Improve city match with common cities list or common prefixes (San, Saint, Fort)
    Do better way of finding address maybe
    Use line breaks for field splitting -> as other boundary
*/



// taking a string as input, parses a location into the appropriate fields
// and passes that information into location. If there is an error or
// the string does not have enough information, we will return, leaving 
// location unchanged
void parseLocationASLoc(ASLocation * location, char * str) {

    // Some fields to store data before we push it to location
            // The address
            char * addr;
            // the index the house number starts at in the string, 
            // and the length of the substring corresponding to the house number
            int addr_index = -1, addr_len = 0;

            int suffix_index = -1; // suffix length always 1

            char * city;
            int city_index, city_len = 0;

            char * state;
            int state_index, state_len = 0;

            // the four digit zip code extension if provided.
            char * zip;
            int zip_index, zip_len = 0;

            char * country;
            int country_index, country_len = 0;

    
    char ** arr;
    
    int size = splitString(&arr, str);

    if (size < 3) return;

    // a binary buffer to store which fields we have already parsed
    short * occupied = calloc(size, sizeof(short));

    // find zip code, state and country first, moving R -> L and block it off
    for (int i = size - 1; i >= 0; i--) {
        if (zip_len == 0) {
            int zipRes = matchZip(arr, i, size);
            if (zipRes) {
                zip_index = i;
                zip_len = zipRes;
                for (int j = 0; j < zipRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (country_len != 0 && state_len != 0) break;
            }
        }
        if (country_len == 0) {
            int countryRes = matchCountry(arr, i, size);
            if (countryRes) {
                country_index = i;
                country_len = countryRes;
                for (int j = 0; j < countryRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (zip_len != 0 && state_len != 0) break;
            }
        }
        if (state_len == 0) {
            int stateRes = matchStateStrong(arr, i, size);
            if (stateRes) {
                state_index = i;
                state_len = stateRes;
                for (int j = 0; j < stateRes; j++) {
                    *(occupied + i + j) = 1;
                }
                if (country_len != 0 && zip_len != 0) break;
            }
        }
    }

    int nextocc = 0; // storing the next occupied segment, so we can avoid unnecessary checking and overlaps.
    while (*(occupied + nextocc) == 0 && nextocc < size - 1) nextocc++;

    for (int i = 0; i < size; i++) {
        if (*(occupied + i) == 1) continue; // segment already accounted for
        if (i > nextocc) {
            while (*(occupied + nextocc) == 0 && nextocc < size - 1) nextocc++;
        }
        if (addr_index == -1) {
            if (matchAddress(arr, i, nextocc)) {
                addr_index = i;
                char * suff;
                // iterate backwards from the last possible street segment
                for (int j = nextocc - 1; j > i; j--) {
                    if (*(*(arr + j) + strlen(*(arr + j)) - 1) == '.') {
                        suff = calloc(strlen(*(arr + j)), sizeof(char));
                        memcpy(suff, *(arr + j), strlen(*(arr + j)) - 1 );
                    } else {
                        suff = calloc(strlen(*(arr + i + j)) + 1, sizeof(char));
                        memcpy(suff, *(arr + j), strlen(*(arr + j)));
                    }
                    int result = strInArr(suff, (char**)streetSuffixes, PARLOCSTRSUFS);
                    free(suff);
                    if (result) {
                        suffix_index = j;
                        break;
                    }
                }
            }
        }
    }

    // RESTRICTION -- EXPAND LATER
    // only allows for Street City State format as a substring
    if (suffix_index != -1 && state_len != 0) {
        for (int i = addr_index; i < state_index; i++) {
            if (*(occupied + i) != 0) {
                free(occupied);
                for (int i = 0; i < size; i++) {
                    free(*(arr + i));
                }
                free(arr);
                return; // EXIT if we flout this format
            }
        }

        addr_len = suffix_index - addr_index + 1;
        city_index = suffix_index + 1;
        city_len = state_index - suffix_index - 1;
        if (city_len < 1) {
            free(occupied);
            for (int i = 0; i < size; i++) {
                free(*(arr + i));
            }
            free(arr);
            return;
        }
    } else {
        free(occupied);
        for (int i = 0; i < size; i++) {
            free(*(arr + i));
        }
        free(arr);
        return; // EXIT IF NOT IN RESTRICTED FORMAT
    }

    state = getCanonicalState(arr, state_index, state_len);

    city = strBuildFromArr(str, city_index, city_len);

    addr = strBuildFromArr(str, addr_index, addr_len);

    if (zip_len != 0) zip = getCanonicalZip(arr, zip_index, zip_len);

    if (country_len != 0) country = getCanonicalCountry(arr, country_index, country_len);

    if (location->city != NULL) {
        free(location->city);
    }
    location->city = city;

    if (location->state != NULL) {
        free(location->state);
    }
    location->state = state;

    if (location->address_line_1 != NULL) {
        free(location->address_line_1);
    }

    location->address_line_1 = addr;

    if (zip_len != 0) {
        if (location->zip != NULL) {
            free(location->zip);
        }
        location->zip = zip;
    }

    if (country_len != 0) {
        if (location->country != NULL) {
            free(location->country);
        }
        location->country = country;
    }

    for (int i = 0; i < size; i++) {
        free(*(arr + i));
    }
    free(arr);
    free(occupied);
}

void parseLocation(ASInstance * inst, char * str) {
    ASLocation * loc = calloc(1, sizeof(ASLocation));

    parseLocationASLoc(loc, str);

    mergeASLocation(inst, loc); // FIX LATER!!! Will not overwrite values as we desire
}