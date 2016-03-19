#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "src/translate_utils.h"
const char* hex_prefix = "0x";
const char* error_num = "0L";
int translate_num(long int* output, const char* str, long int lower_bound, long int upper_bound) {
    char* ok = strstr(str, hex_prefix);
    char *pEnd;
    long int ret;
    if (!str || !output) {
        return -1;
    }

    if (ok) { //if it IS hex
      ret = strtol(str, &pEnd, 16);
    } else { // if its NOT hex
      ret = strtol(str, &pEnd, 10);
    } 

    // Make sure its not a bad input
    if (ret == 0L) {
      return -1;
    }
    // make sure its within bounds
    if (ret < lower_bound || ret > upper_bound) {
      return -1;
    }
    // printf("ret val is %li\n", ret );
    *output = ret;

    return 0;
    
}

int main(int argc, char const *argv[])
{
    int retval;

    char* myHex = "0x1278f";
    long int out;
    retval = translate_num(&out, myHex, 0, 2);

    printf("%li\n", out );
    printf("%d\n", retval);
    return 0;
}