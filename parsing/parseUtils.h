#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int splitString(char*** dst, char * str);

int strEquals(char * str1, char * str2);

char * strCombineSpace(char ** arr, int index, int num);

int strInArr(char * str, char** arr, int array_size);

int strFindInArr(char * str, char** arr, int size);

int strIsDigit(char * str);

char * strBuildFromArr(char* str, int start_index, int len);

int strIndexFromArr(char* str, int arr_index);

int getNextBreakInArr(char* str, int start_index, int size);

void clearStrArr(char ** arr, int ct);