#ifndef PARSETIME_H
#define PARSETIME_H

#include "ASInstance.h"
#include <time.h>
#include <ctype.h>



/*  Given an ASTime and a string, will attempt to pull all time information
    contained in the string into the ASTime. 
*/
void parseTimeASTime(ASTime * time, char * str);

/*  Given an ASFullTime and a string, will attempt to pull all time information
    contained in the string into the ASFullTime. 
*/
void parseTimeASFullTime(ASTime * time, char * str);

/*  Given an ASInstance and a string, will attempt to pull all time information
    contained in the string into the ASInstance. 
*/
void parseTime(ASInstance * inst, char * str);

#endif