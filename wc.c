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

#include <sys/stat.h>
#include <dirent.h>

#ifndef S_IFMT
    // masks for the struct stat
    #define S_IFMT     0170000     // bit mask for the file type bit field
    #define S_IFREG    0100000     // regular file
    #define S_IFDIR    0040000     // directory
#endif

#define MAX_DEPTH  20          // define max depth can be reached in recursion
#define FILE_SEPARATOR "/"


void print_help() {
    printf("wc [-c | -l | -w | -s sep] <filename> [filename...]\n");
    printf("Options:\n");
    printf("    -c:    Count characters in the file\n");
    printf("    -w:    Count words in the file\n");
    printf("    -l:    Count lines in the file\n");
    printf("    -s:    Count own phrases (separated by sep)\n");
    printf("    -r:    If the filename is a directory, count recursively all files inside.\n");
    printf("    -h:    Print this help\n");
    printf("\n");
}

/**
 * The current configuration based on cmdline params.
 */
struct Configuration {
    bool char_counter;    // character counter
    bool line_counter;    // line counter
    bool word_counter;    // word counter
    bool recursive;       // if filename is directory, process all files
                          // recursively

    char separator;       // define own separator for counting

    int files_count;      // total number of files to parse
    char* filenames[100];  // the files that should be parsed
} config = {false, false, false, false, '\0', 0};


/**
 * Statistics of the processed file.
 */
typedef struct {
  unsigned int chars;           // number of characters
  unsigned int lines;           // number of lines
  unsigned int words;           // number of words
  unsigned int custom_phrases;  // number of phrases separated by custom sep.
} Stats;


/**
 * Process the cmdline.
 *
 * The processed data from the cmdline is stored inside the global config
 * variable.
 *
 * Return 0 when cmdline is correct; otherwise return 1;
 */
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
    for (int i = 1; i < argc; i++) {
        if (strcmp("-c", argv[i]) == 0) {
            config.char_counter = true;
            count_defined++;
        } else if (strcmp("-w", argv[i]) == 0) {
            config.word_counter = true;
            count_defined++;
        } else if (strcmp("-l", argv[i]) == 0) {
            config.line_counter = true;
            count_defined++;
        } else if (strcmp("-s", argv[i]) == 0) {
            if (i+1 >= argc) {
              fprintf(stderr, "The '-s' option requires argument.\n");
              return 1;
            }
            i++;
            config.separator = argv[i][0];
            count_defined++;
        } else if (strcmp("-r", argv[i]) == 0) {
            config.recursive = true;
        } else if (argv[i][0] != '-') {
            config.filenames[config.files_count++] = argv[i];
        } else if (strcmp("-h", argv[i]) == 0) {
            print_help();
            exit(0);
        }
    }

    if (config.files_count == 0) {
        fprintf(stderr, "The input file is not specified.\n");
        return 1;
    }

    if (count_defined == 0) {
      fprintf(stderr, "At least one counter has to be specified.\n");
      return 1;
    }

    return 0;
}


/**
 * Print results of specified counters.
 *
 * The function uses the global Configuration represented by the "config" var.
 * The output format is:
 *     <lines> <words> <chars> <custom_phrases> <filename>\n
 * Each field is separated by 1..N whitespaces and only results for specified
 * counters are printed. E.g. when the line counter is specified only, the
 * output format is:
 *     <lines> <filename>\n
 * The first field can have prefix 0..N whitespaces as well.
 */
void print_result(const Stats* stats, const char* filename) {
  if (config.line_counter) printf("%d ", stats->lines);
  if (config.word_counter) printf("%d ", stats->words);
  if (config.char_counter) printf("%d ", stats->chars);
  if (config.separator != '\0') printf("%d ", stats->custom_phrases);
  printf("%s\n", filename);
}


/**
 * Return true if the filename is a directory, otherwise false.
 */
bool is_dir(const char* filename) {
    struct stat sb;
    if(stat(filename, &sb) == -1) {
        fprintf(stderr, "Cannot get data about the %s file.\n", filename);
        return false;
    }


    if ((sb.st_mode & S_IFMT) == S_IFDIR)
        return true;

    return false;
}


/**
 * Return true if the filename is a regular file, otherwise false.
 */
bool is_file(const char* filename) {
    struct stat sb;
    if(stat(filename, &sb) == -1) {
        fprintf(stderr, "Cannot get data about the %s file.\n", filename);
        return false;
    }

    if ((sb.st_mode & S_IFMT) == S_IFREG)
        return true;

    return false;
}


/**
 * Process the file.
 *
 * According to the configuration, apply counters on the <filename> file.
 */
int process_file(const char* filename, int depth) {
    if (is_dir(filename)) {
        if (!config.recursive)
            return 0;    // skip the dir; recursion is not configured

        if (depth >= MAX_DEPTH) {
            fprintf(stderr,
                    "Max depth reached. Skipping the %s directory.\n", filename);
            return 1;
        }

        // process the files inside the filename directory
        DIR *fdir = opendir(filename);
        struct dirent *dir_entry;
        char pathname[1024];
        int r_val = 0;

        if (fdir == NULL) {
            fprintf(stderr, "The %s directory cannot be opened.\n", filename);
            return 1;
        }

        while((dir_entry = readdir(fdir)) != NULL) {
            strcpy(pathname, filename);
            strcat(pathname, FILE_SEPARATOR);
            strcat(pathname, dir_entry->d_name);
            if (strcmp(".", dir_entry->d_name) == 0
                || (strcmp("..", dir_entry->d_name) == 0))
                continue;

            if(process_file(pathname, ++depth))
                r_val = 1;
        }
        return r_val;

        closedir(fdir);
    }

    if (!is_file(filename)) {
        // we are interested just about regular files or directories; skip
        fprintf(stderr,
                "The %s file is not regular file nor directory. Skipping.\n",
                filename);
        return 0;
    }


    FILE *f = fopen(filename, "r");

    if (f == NULL) {
      fprintf(stderr,
              "The %s file cannot be opened. (Maybe it doesn't exist?)\n",
              filename);
      return 1;
    }

    int c;
    bool count_next = true;
    Stats stats = {0, 0, 0, 0};
    while ((c = fgetc(f)) != EOF) {
        if (config.line_counter && c == '\n') {
            stats.lines++;
        }
        if (config.char_counter) {
            stats.chars++;
        }
        if (config.word_counter) {
            if (count_next) {
                if (isspace(c))
                    stats.words++;
                    count_next = false;
            } else {
                count_next = true;
            }
        }
        if (config.separator && c == config.separator) {
            stats.custom_phrases++;
        }
    }

    // print results
    print_result(&stats, filename);
    return 0;
}


int main(int argc, char *argv[]) {
    if (process_cmdline(argc, argv))
        return 1;

    int retval = 0;
    for (int i = 0; i < config.files_count; i++) {
        int process_retval = process_file(config.filenames[i], 0);
        if (process_retval)
            retval = process_retval;
    }
    return retval;
}
