/**
 * @brief Word counting tool
 * @author Viktor Malik <vmalik@redhat.com>
 *
 * A simple word-counting tool.
 * Supports counting words, characters, lines, and phrases separated by
 * custom separator.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


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


struct Configuration {
    bool char_counter;    // character counter
    bool line_counter;    // line counter
    bool word_counter;    // word counter

    char* separator;      // define own separator for counting
    char* filename;       // the file that should be parsed
} config = {false, false, false, NULL};


int process_cmdline(int argc, char *argv[]) {
    if (argc == 2 && strcmp("-h", argv[1]) == 0) {
        print_help();
        exit(0);
    }

    if (argc < 3) {
        fprintf(stderr, "Incorrect number of arguments\n");
        return 1;
    }

    int count_defined = 0;
    for (int i=2; i < argc; i++) {
        if (strcmp("-c", argv[i]) == 0) {
            config.char_counter = true;
            count_defined++;
            continue;
        } else if (strcmp("-w", argv[i]) == 0) {
            config.word_counter = true;
            count_defined++;
            continue;
        } else if (strcmp("-l", argv[i]) == 0) {
            config.line_counter = true;
            count_defined++;
            continue;
        } else if (strcmp("-s", argv[i]) == 0) {
            if (i+1 >= argc) {
              fprintf(stderr, "The '-s' option requires argument.\n");
              return 1;
            }
            i++;
            config.separator = argv[i];
            count_defined++;
            continue;
        } else if (strcmp("-h", argv[i]) == 0) {
            print_help();
            exit(0);
        }
    }

    config.filename = argv[1];

    if (count_defined != 1) {
      fprintf(stderr, "Only one counter type is allowed in the same time.\n");
      return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {

    if (process_cmdline(argc, argv))
        return 1;

    FILE *f = fopen(config.filename, "r");

    // do nothing if no counter is defined
    if (config.char_counter || config.word_counter || config.line_counter
            || config.separator != NULL) {
        int i = 0;
        int c;
        while ((c = fgetc(f)) != EOF) {
            if (config.line_counter && c == '\n') {
                i++;
            } else if (config.char_counter) {
                i++;
            } else if (config.word_counter && isspace(c)) {
                i++;
            } else if (config.separator && c == config.separator[0]) {
                i++;
            }
        }
        printf("%d\n", i);
    }

    return 0;
}
