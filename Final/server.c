#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



int main(int argc, char **argv) {
    int i;
    printf("Hello bebem, parametrelerim:\n");
    for (i=0; i<4; ++i) {
        printf("%s\n", argv[i]);
    }
    
    
    
    return 0;
}