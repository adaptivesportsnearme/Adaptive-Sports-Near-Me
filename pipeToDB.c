#include "ASInstance.h"
#include "parsing/parseLocation.h"
#include <fcntl.h>

#define SOURCE "./pipe.txt"
#define BUFFMULT 128

// gets the next line and feeds it into *str. Returns the length of the new *str buffer
int gtline(char ** str, int source) {

    if (*str != NULL) {
        free(*str);
        *str = NULL;
    }

    char * line = calloc(BUFFMULT, sizeof(char));

    char curr;

    int length = 0;
    int multmult = 1;

    int res = read(source, &curr, 1);

    if (res == 0) return -1;

    while (res != 0) {
        length++;
        if (length % BUFFMULT == BUFFMULT - 1) {
            multmult++;
            line = realloc(line, multmult * BUFFMULT * sizeof(char));
        }
        if (curr == '\n') {
            line = realloc(line, length * sizeof(char));
            *(line + length - 1) = '\0';
            *str = line;
            return length;
        }
        *(line + length - 1) = curr;
        res = read(source, &curr, 1);
    }

    line = realloc(line, length + 1);
    *(line + length) = '\0';
    *str = line;
    return length;

}


int main() {

    int fd = open(SOURCE, O_RDONLY);
    int success = 0;

    char * string = NULL;

    while (1) {
        success = gtline(&string, fd);

        if (success != -1) {
            printf("%s\n", string);
        }
        
    }



}