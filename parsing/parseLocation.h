#ifndef PARSELOCATION_H
#define PARSELOCATION_H


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ASInstance.h"

// functions for testing -- delete later
// int matchZip(char** arr, int index, int size);
// int splitString(char*** dst, char * str);
// int matchStateStrong(char ** arr, int index, int size);
// char * getCanonicalState(char ** arr, int index, int num);
// char * getCanonicalZip(char** arr, int index, int num);
// int matchAddress(char ** arr, int index, int size);
// int matchCountry(char ** arr, int index, int size);
// char * strBuildFromArr(char* str, int start_index, int len);

/*  Takes a string as input and returns a new string with the first
    US/CA postal code in the string, or NULL if no postal code is found.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned. */
char * getPostalCode(char * string);

/*  Takes an address as input and returns a new string with the substring
    most likely to be a city name, or NULL if no such string is found.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getCity(char * string);

/*  Takes a string with location information as input and returns a new
    string corresponding to the country the string conveys, or NULL if
    one is unable to be found.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getCountry(char * string);

/*  Takes a string as input and returns a new string with the state
    the string corresponds to, or NULL if it is unable to be determined.
    For Canada, this returns the province/territory of the address.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getState(char * string);

/*  Takes a string as input and returns a new string with the street address the 
    string corresponds to, or NULL if it is unable to be determined.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getAddress(char * string);

/*  Takes a string as input and returns a new string with the primary street address
    string corresponds to, or NULL if it is unable to be determined.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getAddressLine1(char * string);

/*  Takes a string as input and returns a new string with the secondary street address
    string corresponds to, or NULL if it is unable to be determined.
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
char * getAddressLine2(char * string);

/*  Takes a string as input and populates the provided ASLocation with the
    location information the string contains. Unknown fields do not overwrite
    what is already stored in the struct. If the provided ASLocation is NULL,
    the function will return. 
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
void parseLocationASLoc(ASLocation * location, char * str);

/*  Takes a string as input and populates the provided ASInstance with the
    location information the string contains. Unknown fields do not overwrite
    what is already stored in the struct. If the provided ASInstance is NULL,
    the function will return. 
    
    The string should be able to convey information to a human about the
    location of an event, and contain little else. Strings containing irrelevant
    information or lacking necessary location information may cause 
    erroneous information to be returned.*/
void parseLocation(ASInstance * inst, char * str);

#endif