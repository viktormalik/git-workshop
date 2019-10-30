#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }
    FILE *f = fopen(argv[2], "r");

    if (strcmp("-c", argv[1]) == 0 || strcmp("-w", argv[1]) == 0) {
        int i = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
	    if (strcmp("-w", argv[1]) == 0 && isspace(c)) {
	    	i++;
	    } else if (strcmp("-c", argv[1]) == 0) {
	        i++;
	    }
        }
        printf("%d\n", i);
    }

    return 0;
}
