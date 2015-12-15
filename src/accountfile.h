#ifndef ACCOUNTFILE_H
#define ACCOUNTFILE_H

#include "account.h"                // account definitions

#define BUFFER_SIZE 300             // line buffer length for reading in

// loads an account with the info from filename (returns NULL on error)
account* loadAccount (char* filename);
// write the info from an account to a file (returns nonzero on error)
int writeAccount (account* myAccount, char* filename);

#endif
