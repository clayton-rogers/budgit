#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "account.h"

int init(void);         // starts up all the ncurses stuff (non zero return -> error)
void endinit(void);     // end curses
void writeScreen (account *myAccount, int scrollLine);  // writes the whole screen, scrolled to 'scrollLine'
int selectEntry(account *myAccount, int *scrollLine);                   // return the selected entryNum after it is selected

int calcTotalLines(account * myAccount);        // calculates the number of lines needed to print the account in full
int calcLine(account * myAccount, int entryNum); // calculates the line on which a given entry will be given the scroll number

int getEntryNum(account *myAccount, int selected, int scl);         // gets the entrynum from the selected offset and the scroll line number

void printRunningHelp();                                // print the help info to the screen


#endif
