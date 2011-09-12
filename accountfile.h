#ifndef ACCOUNTFILE_H
#define ACCOUNTFILE_H

#include <stdio.h>					// file input / output
#include <string.h>
#include <ctype.h>
#include "account.h"				// account definitions

#define BUFFER_SIZE 300

// loads an account with the info from filename    (non zero return -> error)
account* loadAccount (char* filename);
// write the info from an account to a file   (non zero return -> error)
int writeAccount (account* myAccount, char* filename);

#endif
