// *** Please see NOTES for notes on programming style of this program *** //

#include <stdio.h>
#include <string.h>
#include "settings.h"
#include "program.h"

#define FILENAME_LEN FILENAME_MAX
#define CONFIG_FILE "/home/clayton/.budgit/budgit.conf"

void printHelp(void);

int main (int argc, char* argv[]) {


    int i;
    char *arg;          // the current argument to be parsed
    //int settings = 0; // the settings that the program is to run with
    char filename[FILENAME_LEN];    // input filename
    FILE *fp;           // input file
    runinfo info;       // all the information that we need to pass to the running program

    // *** Parse the command line arguments *** //
    for (i = 1; i < argc; ++i) {
        // update the current argument
        arg = argv[i];

        // check to see if it's an option
        if (arg[0] == '-') {
            if (arg[1] == '-') {
                // long option
                if (strcmp(arg, "--help") == 0) {
                    printHelp();
                    return 0;
                }

            } else {
                // short option
                if (strcmp(arg, "-h") == 0) {
                    printHelp();
                    return 0;
                }
            }
        } else {
            // if it's not an option, it must be the filename
            // NOTE: only the last filename will be used
            strncpy(filename, arg, FILENAME_LEN);
        }

    }

    // now that the command line settings are accounted for, check for a config file
    fp = fopen(CONFIG_FILE, "r");
    if (fp == NULL) {
        // file does not exist
        printf ("Note: No config file found at ~/.budgit/budgit.conf\n");
    } else {
        // file found, parse it
        printf ("Config file found, parsing...\n");
        // TODO: parse config file
    }

    // the file will need to be passed as well.
    if (argc != 1) {
        printf("%s\n", filename);
    }
    // finally run the program with the gathered settings
    start(info);

    if (fp != NULL) {
        fclose(fp);
    }

    return 0;
}

void printHelp () {
    printf ("Usage:\n");
    printf ("\t");
    printf ("bugit [OPTION...] [ACCOUNT-FILE...]\n");
    printf ("\n");
    printf ("Options:\n");
    printf ("\t");
    printf ("-s, --summary\t\tPrints only a summary of the budget, then exits\n");
    printf ("\t");
    printf ("-h, --help\t\tPrints this help\n\n");
}
