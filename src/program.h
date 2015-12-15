#ifndef PROGRAM_H
#define PROGRAM_H

#include "input.h"
#include "settings.h"
#include "account.h"			// account manipulation functions and definitions
#include "accountfile.h"		// to get accounts from file and to save them
#include "graphics.h"			// drawscreen utility
#include "interface.h"			// the interactive interface

#include <ncurses.h>


typedef struct _runinfo {
	char filename[FILENAME_MAX];
	
} runinfo;


void start (runinfo infos);				// starts the interative program

#endif
