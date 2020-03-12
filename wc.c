/**
 * @brief Word counting tool
 * @author Viktor Malik <vmalik@redhat.com>
 *
 * A simple word-counting tool.
 * Supports counting words, characters, lines, and phrases separated by
 * custom separator.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

void print_help() {
    printf("wc <filename> [-c | -l | -w | -s] [sep]\n");
    printf("Options:\n");
    printf("    -c:    Count characters in the file\n");
    printf("    -w:    Count words in the file\n");
    printf("    -l:    Count lines in the file\n");
    printf("    -s:    Count own phrases (separated by sep)\n");
    printf("    -h:    Print this help\n");
    printf("\n");
}


int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp("-h", argv[1]) == 0) {
        print_help();
        return 0;
    }

    if (!(argc == 3 || (argc == 4 && strcmp("-s", argv[2]) == 0))) {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }
    FILE *f = fopen(argv[1], "r");

    int c_opt = strcmp("-c", argv[2]) == 0;
    int l_opt = strcmp("-l", argv[2]) == 0;
    int w_opt = strcmp("-w", argv[2]) == 0;
    int s_opt = strcmp("-s", argv[2]) == 0;

    if (c_opt || l_opt || w_opt || s_opt) {
        int i = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (l_opt && c == '\n') {
                i++;
            } else if (c_opt) {
                i++;
            } else if (w_opt && isspace(c)) {
	    	i++;
	    } else if (s_opt && c == argv[3][0]) {
                i++;
            }
        }
        printf("%d\n", i);
    }

    return 0;
}
