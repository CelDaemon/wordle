#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "config.h"

enum options {
    OPT_VERSION,
    OPT_HELP = 'h',
    OPT_ERR = '?',
    OPT_END = -1
};

static struct option long_options[] = {
    { "version", no_argument, NULL, OPT_VERSION },
    { "help",    no_argument, NULL, OPT_HELP },
    { NULL }
};

static char options[] = "h";
static char *program_name = "mkwords";

void usage(bool const error) {
    fprintf(error ? stderr : stdout,
            "Usage: %s [OPTION]... [answers-file] [guesses-file] [output-file]\n"
            "  -h, --help     display this help and exit\n"
            "      --version  output version information and exit\n",
            program_name);
}

void version() {
    printf(
            "%s (" PACKAGE_NAME ") " PACKAGE_VERSION "\n"
            "License GPLv3+: GNU GPL version 3 <https://gnu.org/licenses/gpl.html>\n"
            "\n"
            PACKAGE_NAME " home page: <" PACKAGE_URL ">\n"
            "E-mail bug reports to: <" PACKAGE_BUGREPORT ">\n",
            program_name);
}

void init_program_name(int const argc, char * const argv[]) {
    if(argc == 0 || argv[0] == NULL)
        return;

    char * const match = strrchr(argv[0], '/');

    if(match == NULL) {
        program_name = argv[0];
        return;
    }

    program_name = match + 1;
}

int main(int const argc, char * const argv[]) {
    init_program_name(argc, argv);
    enum options option;
    while((option = getopt_long(argc, argv, options, long_options, NULL)) != OPT_END) {
        switch(option) {
        case OPT_HELP:
            usage(false);
            return 0;
        case OPT_VERSION:
            version();
            return 0;
        default:
            usage(true);
            return 1;
        }
    }
    return 0;
}
