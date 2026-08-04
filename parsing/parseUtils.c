#include "parseUtils.h"


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


// Given an array and a string, returns the substring index on str that
// corresponds to the given index in a splitString array
int strIndexFromArr(char* str, int arr_index) {
    
    if (str == NULL) return -1;

    // an integer corresponding to an index in splitString(str)
    int index = 0;
    
    // the start of our substring
    int start = 0;

    // advances start to the index of the first non-space character in the string.
    while (start < strlen(str) && (isspace(*(str + start)) || *(str + start) == ',')) {
        start++;
    }

    if (start == strlen(str)) return -1; // returns if passed a string with only spaces and commas

    // keeps track of if the previous character was a space or comma
    int lastwasspace = 0;


    while (index < arr_index) {
        start++;
        if (start >= strlen(str)) return -1; // return if out of range
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

    return start;
    
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

// clears all entries in a string array with ct entries
void clearStrArr(char ** arr, int ct) {
    for (int i = 0; i < ct; i++) {
        if (*(arr + i) != NULL) free(*(arr + i));
    }
}

