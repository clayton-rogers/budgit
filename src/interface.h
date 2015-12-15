#ifndef INTERFACE_H
#define INTERFACE_H

#include "account.h"

int gaddEntry(account* myAccount);      // interface to add an entry        return true if adds entry

void writeScreenSelect (account *myAccount, int scrollLine, int selectLine);  // writes the whole screen with the selected line hilighted

int gwriteAccount(account* myAccount);  // interface to save account        returns non zero on error

account* gloadAccount();                // asks for a filename and loads the account
                                        // returns non zero on failure

#endif
