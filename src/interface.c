#include "interface.h"

#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include "graphics.h"
#include "accountfile.h"
#include "input.h"

//*** Helper fx ***//
int getFilename(char* filename);        // gets a filename from the user, returns non-zero on failure


// *** Actual Code ***//
void gaddEntry_printLine(entry * tmpEntry, int hilight) {
    // hilight is the field number that is going to be hilighted
    int row, col;
    getmaxyx(stdscr, row, col);
    if (col < 80) return;

    move(row-1, 0);         // move to the second last line

    // print the date
    if (hilight == 1) attron(COLOR_PAIR(2));
    printw("%02d", tmpEntry->Date.day);
    if (hilight == 1) attroff(COLOR_PAIR(2));
    addch('/');

    if (hilight == 2) attron(COLOR_PAIR(2));
    printw("%02d", tmpEntry->Date.month);
    if (hilight == 2) attroff(COLOR_PAIR(2));
    addch('/');

    if (hilight == 3) attron(COLOR_PAIR(2));
    printw("%02d", tmpEntry->Date.year);
    if (hilight == 3) attroff(COLOR_PAIR(2));
    addch(' ');

    // print the description
    if (hilight == 4) attron(COLOR_PAIR(2));
    printw("%s", tmpEntry->desc);
    if (strlen(tmpEntry->desc) == 0) addch('-');
    if (hilight == 4) attroff(COLOR_PAIR(2));
    addch(' ');

    // print either D or C for debit or credit
    if (hilight == 5) attron(COLOR_PAIR(2));
    if (tmpEntry->type == 0)
        addch('D');
    else
        addch('C');
    if (hilight == 5) attroff(COLOR_PAIR(2));
    addstr("    ");

    // print the amount
    if (hilight == 6) attron(COLOR_PAIR(2));
    printw("$%5.2f", tmpEntry->amount/100.0);
    if (hilight == 6) attroff(COLOR_PAIR(2));
    addch(' ');

    // and that's it!
}

int gaddEntry (account* myAccount) {
    // Returns true(1) if it adds an entry

    int field = 1;
    entry tmpEntry;
    entry * lastEntry = getEntry(myAccount, myAccount->numEntry-1);
    int notDone = 1;
    int input;
    char app[2];
    app[1] = '\0';

    // default to the date of the last entry in the account
    tmpEntry.Date.day = lastEntry->Date.day;
    tmpEntry.Date.month = lastEntry->Date.month;
    tmpEntry.Date.year = lastEntry->Date.year;

    tmpEntry.desc[0] = '\0';            // empty description
    tmpEntry.type = 0;                  // default to debit
    tmpEntry.amount = 0;

    gaddEntry_printLine(&tmpEntry, field);

    // Get the actual information from the user
    while (notDone) {
        input = getch();

        if (field != 4) {
            if (input == 'j') input = KEY_DOWN;
            if (input == 'k') input = KEY_UP;
        }


        switch (input) {
            case 410:           // screen resize
                return 0;
            case KEY_RIGHT:
            case KEY_TAB:
                ++field;
                if (field > 6) field = 1;
                break;
            case KEY_LEFT:
                --field;
                if (field < 1) field = 6;
                break;
            case KEY_UP:
                if (field == 1) {
                    --tmpEntry.Date.day;
                    if (tmpEntry.Date.day < 1) tmpEntry.Date.day = 31;
                }
                if (field == 2) {
                    --tmpEntry.Date.month;
                    if (tmpEntry.Date.month < 1) tmpEntry.Date.month = 12;
                }
                if (field == 3) {
                    --tmpEntry.Date.year;
                    // no overflow to worry about
                }
                if (field == 5) {
                    if (tmpEntry.type == 1) {
                        tmpEntry.type = 0;
                    } else {
                        tmpEntry.type = 1;
                    }
                }
                break;
            case KEY_DOWN:
                if (field == 1) {
                    ++tmpEntry.Date.day;
                    if (tmpEntry.Date.day > 31) tmpEntry.Date.day = 1;
                }
                if (field == 2) {
                    ++tmpEntry.Date.month;
                    if (tmpEntry.Date.month > 12) tmpEntry.Date.month = 1;
                }
                if (field == 3) {
                    ++tmpEntry.Date.year;
                    // no overflow to worry about
                }
                if (field == 5) {
                    if (tmpEntry.type == 1) {
                        tmpEntry.type = 0;
                    } else {
                        tmpEntry.type = 1;
                    }
                }
                break;
            case KEY_ENTER_WORKS:
                notDone = 0;
                break;
            case KEY_BACKSPACE:
                if (field == 4) {           // desc
                    // remove the last char.
                    tmpEntry.desc[strlen(tmpEntry.desc)-1] = '\0';
                }
                if (field == 6) {           // amount
                    tmpEntry.amount /= 10;
                }
                //strcpy(tmpEntry.desc, "bs");
                break;
            case KEY_DC:                        // del key
                if (field == 1) {
                    tmpEntry.Date.day = 1;
                }
                if (field == 2) {
                    tmpEntry.Date.month = 1;
                }
                if (field == 3) {
                    tmpEntry.Date.year = 10;
                }
                if (field == 4) {
                    strcpy(tmpEntry.desc, "");
                }
                if (field == 6) {
                    tmpEntry.amount = 0;
                }
                //strcpy(tmpEntry.desc, "del");
                break;
            case KEY_ESC:
                return 0;
            default:
                //printw("%d", input);

                //if (field == 1 || field == 2 || field == 3) field = 4;
                if (field == 4) {           // desc
                    if (strlen(tmpEntry.desc) == (ENTRY_DESC_SIZE -1)) {
                        break;
                    }
                    app[0] = input;
                    strcat(tmpEntry.desc, app);
                }
                if (field == 6) {           // amount
                    // check if the number entered is a number or period (ignort the '.'
                    if (isdigit(input)) {
                        tmpEntry.amount *= 10;
                        tmpEntry.amount += (DIGIT_BIT ^ input);     // strip the char bits
                    }
                }
                break;

        }

        gaddEntry_printLine(&tmpEntry, field);
    }

    if (strlen(tmpEntry.desc) == 0) return 0;

    if (addEntry(myAccount, tmpEntry)) {
        return 0;       // return 0 if fail
    } else {
        return 1;
    }

    return 0;
}

void writeScreenSelect (account *myAccount, int scrollLine, int selectLine) {
    writeScreen (myAccount, scrollLine);
    mvchgat (selectLine, 0, -1, A_REVERSE, 2, NULL);
}

account* gloadAccount() {

    account * myAccount;
    char filename[FILENAME_MAX];
    char localFilename[FILENAME_MAX];

    // preload the default path
    strcpy(filename, "./acct/");

    // get the local filename
    if (getFilename(localFilename)) {
        return NULL;
    }

    strncat(filename, localFilename, FILENAME_MAX-strlen(filename)-1);

    myAccount = loadAccount(filename);

    return myAccount;
}

int gwriteAccount (account * myAccount) {
    // interface to save account
    // returns non zero on error

    char filename[FILENAME_MAX];
    char localFilename[FILENAME_MAX];

    strcpy(filename, "./acct/");

    if (getFilename(localFilename)) {
        return 1;
    }

    strncat(filename, localFilename, FILENAME_MAX-strlen(filename)-1);

    // Now that we have the filename we can write the account
    if (writeAccount(myAccount, filename)) {
        return 2;
    }

    return 0;

}


int getFilename (char* filename) {

    char app[2];
    app[1] = '\0';
    int row, col, index, hasExt = 0;
    int running = 1, err = 0;
    int input;
    int curCol = 0;

    // get screen dimentions
    getmaxyx(stdscr, row, col);
    if (col < 80) return 2;

    // turn the cursor back on
    curs_set(1);

    // make sure the filename is empty to begin with
    filename[0] = '\0';

    // get the filename from the user
    move(row-1, 0);
    addstr("Please enter the filename: ");
    curCol = 27;

    while (running) {
        input = getch();

        switch (input) {
            case KEY_ENTER_WORKS:
                running = 0;
                break;
            case KEY_ESC:
                running = 0;
                err = 1;
                break;
            case KEY_BACKSPACE:
                filename[strlen(filename)-1] = '\0';
                --curCol;
                mvprintw(row-1, curCol, "  ");
                break;
            default:
                // add the letter to the filename
                app[0] = input;
                strcat(filename, app);
                // print the letter to the screen so the user knows it's worked
                mvprintw(row-1, curCol, "%c", input);
                ++curCol;
                break;
        }
    }

    // add the file extention if one was not given
    index = strlen(filename);
    --index;                    // now we're pointed at the last entry
    while (index >= 0) {
        if (filename[index] == '.') {
            hasExt = 1;
            break;
        }
        --index;
    }

    // is an extention is not present add it
    if (!hasExt) {
        strcat(filename, ".act");
    }

    // turn the cursor back off again
    curs_set(0);

    // if there is an error, return it
    return err;
}
