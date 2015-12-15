#include "graphics.h"

#include <ncurses.h>
#include <string.h>
#include "interface.h"
#include "accountfile.h"
#include "input.h"

// *** Helper fx *** //
static int dateEqual(date date1, date date2);          // only checks the month and year
static void printEntry(entry* myEntry, int col);       // prints an entry at row 'row' using 'col' columns
static void getDateHeader(char dest[], date myDate);   // puts the char representation of 'date' into dest
static void fillPads(char buffer[], int numPads);      // fill the buffer with pads
static int numDateChange(account * myAccout);          // returns the number of date headers that will be needed for an account

int dateEqual (date date1, date date2)  {
    if (date1.month == date2.month) {
        if (date1.year == date2.year) {
            return 1;
        }
    }

    return 0;
}

void printEntry(entry * myEntry, int col) {
    unsigned int descSize = col;
    char buffer[200];
    int dollars, cents;
    buffer[0] = '\0';   // empty string
    // figure out how much space we have for the description and how much padding we may need
    descSize -= 10 + 1;         // Date: 30/12/2031 plus a space
    descSize -= 9;              // Balance: $12022.01
    descSize -= 9 + 9;          // Credit and Debit colums;
    descSize -= 1 + 2 + 2;      // Spaces between balance/credit credit/debit debit/desc

    // print the date
    printw("%02d/%02d/%02d ", myEntry->Date.day, myEntry->Date.month, myEntry->Date.year);

    // print the description
    strncpy(buffer, myEntry->desc, descSize);
    while (strlen(buffer) < descSize) strcat(buffer, " ");
    printw("%s ", buffer);

    // print the debit / credit columns
    dollars = myEntry->amount / 100;            // supposed to be int div
    cents = myEntry->amount % 100;
    if (myEntry->type == 0) {
        // Debit
        printw("%5d.%02d           ", dollars, cents);      // stored as cents
    } else {
        // Credit
        printw("         %5d.%02d  ", dollars, cents);
    }

    // print the balance
    if (myEntry->balance < 0) {
        attron(COLOR_PAIR(1));                      // red
        printw("$%5.2f", myEntry->balance/100.0);
        attroff(COLOR_PAIR(1));
    } else {
        printw("$%5.2f", myEntry->balance/100.0);
    }


    // and a new line
    printw("\n");
}

void getDateHeader(char dest[], date myDate) {

    dest[0] = '\0';

    strcat(dest, "    ");           // so the heading is a little offset

    switch (myDate.month) {
        case 1:
            strcat(dest, "JANUARY");
            break;
        case 2:
            strcat(dest, "FEBRUARY");
            break;
        case 3:
            strcat(dest, "MARCH");
            break;
        case 4:
            strcat(dest, "APRIL");
            break;
        case 5:
            strcat(dest, "MAY");
            break;
        case 6:
            strcat(dest, "JUNE");
            break;
        case 7:
            strcat(dest, "JULY");
            break;
        case 8:
            strcat(dest, "AUGUST");
            break;
        case 9:
            strcat(dest, "SEPTEMBER");
            break;
        case 10:
            strcat(dest, "OCTOBER");
            break;
        case 11:
            strcat(dest, "NOVEMBER");
            break;
        case 12:
            strcat(dest, "DECEMBER");
            break;
        default:
            strcat(dest, "ERROR");
            printw("%d\n", myDate.month);
    }

    strcat(dest, " ");

    // FIXME: there is something wrong here
    sprintf(dest, "%s%d", dest, (myDate.year+2000));
}

void fillPads (char buffer[], int numPads) {
    int i;

    for (i = 0; i < numPads; ++i) {
        buffer[i] = ' ';
    }

    // put the end of string back on
    buffer[i] = '\0';
}

int numDateChange (account * myAccount) {

    int numChange = 0;
    entry * curEntry;
    int entryNum = 0;
    date prevDate;

    prevDate.month = 1;
    prevDate.year = 0;

    while (entryNum < myAccount->numEntry) {
        curEntry = getEntry(myAccount, entryNum);
        if (!dateEqual(curEntry->Date, prevDate)) {
            prevDate.month = curEntry->Date.month;
            prevDate.year = curEntry->Date.year;
            ++numChange;
        }


        ++entryNum;
    }

    return numChange;
}

// *** Code *** //
int init(void) {
    initscr();          // start screen
    if (has_colors() == FALSE) {
        return 1;
    }
    start_color();      // start color module
    cbreak();           // to catch key presses with no enter
    keypad(stdscr, TRUE); // for arrow keys
    noecho();           // we'll handle echo manually

    // define the colors we are going to use
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    // turn off cursor
    curs_set(0);

    return 0;
}

void endinit(void) {
    endwin();
}

void writeScreen (account *myAccount, int scrollLine) {

    if (myAccount == NULL) {
        clear();
        mvprintw(0, 0, "No account loaded to print\n");
        return;
    }

    int row, col;                   // the size of the screen
    char buffer[100];               // to pass around strings
    int pads, halfpad;              // pading info
    int curLine, curEntry;          // the current line and entry we're printing
    date prevDate;                  // to figure out whether we need to print the date heading
    int totalLines = calcTotalLines(myAccount); // calculates the number of lines that the account would need to be fully printed

    // clear the screen so we don't have to deal with phantom chars
    clear();

    // figure out how big our screen is and whether it's big enough
    getmaxyx(stdscr, row, col);
    if (col < 80) {
        mvprintw(0,0, "Screen must have at least 80 columns\n",NULL);
        return;
    }
    if (row < 10) {
        mvprintw(0,0, "Screen must have at least 10 rows\n", NULL);
        return;
    }


    // *** Draw the heading line *** //
    // figure out how many pads we will need
    pads = col;
    pads -= strlen ("budgit");
    pads -= strlen (myAccount->name);
    pads -= 10;                 // '1/5 (81%) '
    if (totalLines > 10) pads -= 2;
    if (totalLines > 100) pads -= 2;
    if (totalLines > 1000) pads -= 2;
    if (totalLines > 10000) pads -= 2;
    halfpad = pads/2;

    // load the line
    //attron(A_REVERSE);
    move(0,0);
    printw("budgit");
    fillPads(buffer, halfpad);
    printw("%s", buffer);
    printw(myAccount->name);
    fillPads(buffer, (pads-halfpad));
    printw("%s", buffer);
    printw("%d", scrollLine);
    printw("/");
    printw("%d", totalLines);
    printw(" (");
    printw("%02d", (scrollLine*100)/totalLines);
    printw("%) ");
    //attroff(A_REVERSE);

    move(0,0);
    chgat(-1, A_REVERSE, 0, NULL);

    move(1,0);
    // TODO: add another header line with
    // the month, 'debit' 'credit' 'balance' etc...

    // *** print each entry that will fit *** //
    prevDate.month = 0;
    prevDate.year = 0;
    curEntry = 0;
    curLine = 0;

    while (1) {
        // check whether we have run out of entries
        if (curEntry >= myAccount->numEntry) {
            break;
        }

        // if the month/year are the same then we can just print the entry
        if (dateEqual(prevDate, getEntry(myAccount,curEntry)->Date)) {
            // check to see if the line is on the screen
            if (curLine > (scrollLine+row-2)) {         // -2, one for header and one for footer
                break;      // we've reached the end of the screen, stop printing
            }
            if (curLine < scrollLine) {
                curEntry ++;
                curLine ++;     // before the screen starts
                continue;
            }
            printEntry(getEntry(myAccount,curEntry), col);
            curEntry ++;
            curLine ++;
        } else {
            // the dates are not equal so update it
            prevDate.month = getEntry(myAccount,curEntry)->Date.month;
            prevDate.year = getEntry(myAccount,curEntry)->Date.year;

            if (curLine > (scrollLine+row-2)) {         // reached the end of the screen
                break;
            }
            if (curLine < scrollLine) {     // don't print if it's before the start of the screen
                curLine ++;
                continue;
            }

            // print the header to the screen
            getDateHeader(buffer, prevDate);
            attron(COLOR_PAIR(3));                  // Green
            printw("%s\n", buffer);
            attroff(COLOR_PAIR(3));
            curLine ++;
        }
    }

    // finally sync the screen buffer
    refresh();
}

int calcTotalLines (account* myAccount) {

    if (myAccount == NULL) {
        return 0;
    }

    int i;
    int total = 0;
    entry* curEntry;
    date curDate;
    curDate.year = 0;

    for (i = 0; i < myAccount->numEntry; ++i) {
        curEntry = getEntry(myAccount,i);
        if (curDate.month == curEntry->Date.month
            && curDate.year == curEntry->Date.year) {
                total ++;
        } else {
            ++total; ++total;       // allow an extra space for the month header
            curDate.month = curEntry->Date.month;
            curDate.year =  curEntry->Date.year;    // update the curDate
        }
    }

    return total;
}

int selectEntry(account *myAccount, int *scrollLine) {
    // returns -1 when an invalid selection is made

    int header = 1;         // the number of lines in the header

    int scl = *scrollLine;
    int selected = calcLine(myAccount, myAccount->numEntry-1,scl) - scl;
        // ^ sets the currently selected entry to the last one
    int input;
    int running = 1;
    int row, col;
    short pair;
    attr_t prevAttr;

    getmaxyx(stdscr, row, col);
    if (col < 80) return -1;    // error

    // if the last entry is not visible then we will have to scroll down until it is
    // header fix here
    while (selected > row-2) {
        --selected;
        ++scl;
        writeScreenSelect (myAccount, scl, selected+header);
    }

    while (running) {

        // print the screen with hilighting
        writeScreenSelect(myAccount, scl, selected+header);

        // wait fot input
        input = getch();

        // process the input
        switch (input) {
            case KEY_UP:
            case 'k':
            case 'K':
                --selected;
                if (selected < 0) {
                    selected = 0;
                    --scl;
                    if (scl < 0) scl = 0;
                    writeScreenSelect (myAccount, scl, selected+header);
                }
                move(selected+1, 6);
                attr_get(&prevAttr, &pair, NULL);
                mvprintw(0,0, "%d", pair);
                if (pair == 3) {
                    --selected;
                    if (selected < 0) {
                        selected = 1;
                        --scl;
                        if (scl < 0) scl = 0;
                        writeScreenSelect (myAccount, scl, selected+header);
                    }
                }
                if (getEntryNum (myAccount, selected, scl) == -1) {
                    selected--;
                    if (selected < 0) {
                        selected = 1;
                        --scl;
                        if (scl < 0) {scl = 0;}
                        writeScreenSelect (myAccount, scl, selected+header);
                    }
                    continue;
                }
                break;
            case KEY_DOWN:
            case 'j':
            case 'J':
                // BUG: fix, still goes off end of screen
                ++selected;
                if (selected > (row-2)) {
                    --selected;
                    // What does the following code do? I think it stops date headers from being selectable
                    if (selected+scl <= myAccount->numEntry+numDateChange(myAccount)) {
                        ++scl;
                        writeScreenSelect (myAccount, scl, selected+header);
                    }
                }
                move(selected+1, 6);
                attr_get(&prevAttr, &pair, NULL);
                mvprintw(0,0, "%d", pair);
                if (pair == 3) {
                    ++selected;
                    if (selected > (row-2)) {
                        --selected;
                        ++scl;
                        if (scl+selected > myAccount->numEntry) --scl;
                        writeScreenSelect (myAccount, scl, selected+header);
                    }
                }
                if (getEntryNum (myAccount, selected, scl) == -1) {
                    ++selected;
                    if (selected > (row-2)) {
                        --selected;
                        if (selected+scl <= myAccount->numEntry+numDateChange(myAccount)) {
                            ++scl;
                            writeScreenSelect (myAccount, scl, selected+header);
                        }
                    }
                    continue;
                }
                break;
            case 10:
                running = 0;
                break;
            case 410:                           // window resize
                writeScreenSelect (myAccount, scl, selected+header);
                break;
            case 'q':
            case 'c':
                return -1;

        }
    }

    // now to figure out what the entrynum of the selected entry is
    selected = getEntryNum (myAccount, selected, scl);
    if (!selected) {
        ++selected;

    }

    *scrollLine = scl;
    writeScreen(myAccount, *scrollLine);
    return selected;
}

int getEntryNum (account *myAccount, int selected, int scl) {
    // returns -1 when an invalid selection is made
    int entryNum = 0;
    date curDate;
    entry *tmpEntry;

    scl += selected;

    curDate.year = 0;
    curDate.month = 0;

    while (entryNum < myAccount->numEntry) {

        tmpEntry = getEntry(myAccount, entryNum);
        if (dateEqual(curDate, tmpEntry->Date)) {
            if (scl == 0) return entryNum;
            ++entryNum;
        } else {
            if (scl == 0) return -1;
            curDate.month = tmpEntry->Date.month;
            curDate.year  = tmpEntry->Date.year;
        }
        --scl;
    }

    return -1;
}

int calcLine(account * myAccount, int entryNum, int scl) {

    int curEntry = 0;
    int lineNumber = 0;
    date curDate;
    curDate.month = 1;
    curDate.year = 1;
    entry * tmpEntry;

    while (curEntry != entryNum && curEntry < myAccount->numEntry) {
        tmpEntry = getEntry(myAccount, curEntry);
        if (dateEqual(curDate, tmpEntry->Date)) {
            ++curEntry;
        } else {
            curDate.month = tmpEntry->Date.month;
            curDate.year = tmpEntry->Date.year;
        }
        ++lineNumber;
    }

    return lineNumber;
}

void printRunningHelp() {

    clear();
    mvprintw(0,0, "Budgit\n");
    printw( "\tq - quit\t\t\n");
    printw( "\ta - add entry\t\t\tr - remove entry\n");
    printw( "\ts - save account\t\tl - load account\n");
    printw( "\tdown - scroll down\t\tup - scroll up\n");
    printw( "\tj - scroll down\t\t\tk - scroll up\n");
    printw( "\th - print this help\n");

    printw( "Debugging keys:\n");
    printw( "\te - error on purpose\n");

}
