#ifndef INTERFACE_H
#define INTERFACE_H

#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include "graphics.h"
#include "account.h"
#include "accountfile.h"
#include "input.h"


int gaddEntry(account* myAccount);		// interface to add an entry		return true if adds entry

int gwriteAccount(account* myAccount);	// interface to save account		returns non zero on error

account* gloadAccount();				// asks for a filename and loads the account
										// returns non zero on failure

#endif
