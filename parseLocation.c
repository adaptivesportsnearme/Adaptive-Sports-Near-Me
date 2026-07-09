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
    In multiple places within this code, the term multistate is used.
    Multistate denotes a state, territory, or province and contains spaces

    For instance "New Jersey", "District of Columbia", and "Northwest Territories" 
    are all multistates, but "Alabama", "DC", and "NT" are not, even though
    "DC" and "NT" refer to the above states that use spaces. The reason for this
    discrepancy is that this is a programming-related label that will not be used outside
    of this module's internal workings.
*/


#include "parseLocation.h"
#include "locationReference.h"

typedef struct {

    // the string we are parsing
    char * source;

    // House number
    char * number;
    // the index the house number starts at in the string, 
    // and the length of the substring corresponding to the house number
    int number_index, number_len;

    // The direction of a street
    // (W Schiller Ave) -> st_predir == "W"
    char * st_predir;
    int st_predir_index, st_predir_len;

    // The name of a street
    // (W Schiller Ave) -> st_name == "Schiller"
    char * st_name;
    int st_name_index, st_name_len;

    // The suffix of a street
    // (W Schiller Ave) -> st_suffix = "Ave";
    char * st_suffix;
    int st_suffix_index, st_suffix_len;

    char * city;
    int city_index, city_len;

    char * state;
    int state_index, state_len;

    // the four digit zip code extension if provided.
    char * zip4;
    int zip4_index, zip4_len;

    char * country;
    int country_index, country_len;
} parseLoc;

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
        if (strEquals(str, (char *)*(arr + i))) return 1;
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

// Returns the canonical form of a given state in new memory
// INPUT MUST BE A MATCHING STATE, or in other words,
// matchStateStrong(arr, index, num) > 0. If this condition is not met,
// strange behavior may occur:
    // If the input is already two characters, then it will return those two 
    // characters in new memory REGARDLESS OF IF THEY REPRESENT A STATE.
    // Otherwise NULL will be returned
char * getCanonicalState(char ** arr, int index, int num) {

    // If the input is two characters, a copy will be returned in new memory
    if (strlen(*(arr + index)) == 2) {
        char * ret = calloc(3, sizeof(char));
        memcpy(ret, *(arr + index), 2);
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


/*  A function that takes a given valid c-string str and splits it along
    whitespaces for easier containment, putting it into dst. 
    Returns the number of pieces in the array, or 0 if the string
    has no non-space characters or is NULL*/
int splitString(char*** dst, char * str) {

    if (str == NULL) return 0;

    // The array of strings we will return
    char ** pieces;

    // the number of splits we will make. This excludes whitespace at 
    // the start and end of a string
    int spaces = 0;
    
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && isspace(*(str + start))) {
        start++;
    }

    if (start == strlen(str)) return 0; // returns if passed a string with only spaces

    // counts the number of segments needed to split the string
    int lastwasspace = 0;
    for (int i = start; i < strlen(str); i++) {
        if (isspace(*(str + i))) {
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
        while (!isspace(*(str + eind)) && eind < strlen(str)) {
            eind++;
        }

        // copies the ith string segment into *(pieces + i)
        *(pieces + currpiece) = calloc(eind - sind + 1, sizeof(char));
        memcpy(*(pieces + currpiece), str + sind, eind - sind);
        *(*(pieces + currpiece) + eind - sind) = '\0';

        // readies 
        while (isspace(*(str + eind)) && eind < strlen(str)) {
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

        ZIP - must be formatted correctly or information lost
            - usually at the end of an address or by the city or state
            - typically determines state
            - format determines country
        State - finite correctly spelled possibilities, but high possibility for misread
              - Determines country
              - usually at end of address and by country
              - country will almost always be after
        Street number - number, ideally in front of street name
                      - no letters unless there's a letter at the end
                      - usually first field in address
                      - usually not mistakeable for other fields except potentially zip
                      - always before street info
        street prefix - very fixed, has to be direction (can be abbreviated or in spanish though)
                      - smallest number of possibilities, but can be a multi-cue for a state
                      - Right after street number if present and right before street
        Street name - Loosest field
                    - usually towards start
                    - after street prefix or street number
                    - before street suffix if suffix is present
        Street suffix - wide array of possibilities but still limited
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

*/


void parseLocation(ASInstance * inst, char * str) {
    


}