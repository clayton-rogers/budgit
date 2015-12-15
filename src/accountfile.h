#ifndef ACCOUNTFILE_H
#define ACCOUNTFILE_H

#include <stdio.h>                  // file input / output
#include <string.h>
#include <ctype.h>
#include "account.h"                // account definitions
#include "tag.h"                    // tag definitions

#define BUFFER_SIZE 300             // line buffer length for reading in

// loads an account with the info from filename (returns NULL on error)
account* loadAccount (char* filename);
// write the info from an account to a file (returns nonzero on error)
int writeAccount (account* myAccount, char* filename);

#endif
