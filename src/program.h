#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>  // for FILENAME_MAX

typedef struct _runinfo {
    char filename[FILENAME_MAX];

} runinfo;


void start (runinfo infos);             // starts the interative program

#endif
