// *** Top level of the running program *** //
#include <stdbool.h>

#include <ncurses.h>

#include "program.h"
#include "input.h"
#include "settings.h"
#include "account.h"            // account manipulation functions and definitions
#include "accountfile.h"        // to get accounts from file and to save them
#include "graphics.h"           // drawscreen utility
#include "interface.h"          // the interactive interface

// starts the program running
void start (runinfo infos) {

    account* acc = NULL;

    int input;              // inputed character
    int scrollPos = 0;
    int numLines;
    int needRef;            // whether a refresh is needed
    int entryNum;
    bool running = true;
    char err = 0;

    acc = loadAccount(infos.filename);     // TODO: don't autoload account
    if (acc == NULL) {
        printf("Account could not be loaded from file\n");
        return;
    }


    init();         //start curses

    numLines = calcTotalLines(acc);

    writeScreen(acc, scrollPos);

    // *** MAIN PROGRAM LOOP *** //
    while (running) {
        input = getch();

        // just refresh the screen if no account is load and the key is not valid
        if (acc == NULL && input != 'q' && input != 'l' && input != 'L')
            input = 410;

        // figure out what the user wants to do
        switch (input)  {
            case 'q':
                running = false;
                break;
            case 410:
                needRef = 1;
                break;
            case KEY_UP:
            case 'k':
            case 'K':
                needRef = 1;
                --scrollPos;
                if (scrollPos < 0) scrollPos = 0;
                break;
            case KEY_DOWN:
            case 'j':
            case 'J':
                needRef = 1;
                ++scrollPos;
                if (scrollPos > numLines) scrollPos = numLines;
                break;
            case 'a':                       // add entry
            case 'A':
                if (gaddEntry(acc)) {
                    // an entry was added
                    numLines = calcTotalLines(acc);
                    needRef = 1;
                } else {
                    // an entry was not added
                    needRef = 1;    // still need to refresh the screen to get
                                    // rid of the interface for adding
                }
                break;
            case 'r':                       // remove entry
            case 'R':
                entryNum = selectEntry(acc, &scrollPos);
                if (entryNum == -1) {
                    needRef = 1;
                    break;
                }
                delEntry(acc, entryNum);
                numLines = calcTotalLines(acc);
                needRef = 1;
                break;
            case 'e':                       // error on purpose
            case 'E':
                running = false;
                err = 10;
                break;

            case 's':                       // save
            case 'S':
                if (gwriteAccount(acc)) {
                    running = false;
                    err = 3;
                }
                needRef = 1;
                break;
            case 'l':                       // load
            case 'L':
                // get rid of the current account
                delAccount(acc);
                acc = NULL;         // the account has now been deleted

                // get the filename and load the account into the pointer
                acc = gloadAccount();
                needRef = 1;
                scrollPos = 0;
                break;
            case 'h':
            case 'H':
                printRunningHelp();
                input = getch();        // wait for user to press a key
                needRef = 1;
                break;
        }

        // refresh the screen if it is nesesary
        if (needRef) {
            writeScreen(acc, scrollPos);
            needRef = 0;
        }
    }


    endinit();      // end curses

    if (err) printf("There was an error during runtime.\n");

}



