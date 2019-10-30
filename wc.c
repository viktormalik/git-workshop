#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }
    FILE *f = fopen(argv[2], "r");

    if (strcmp("-c", argv[1]) == 0) {
        int i = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            i++;
        }
        printf("%d\n", i);
    }

    return 0;
}
