#ifndef TAG_H
#define TAG_H

#include <stdlib.h>         // malloc
#include <string.h>         // for string copy

#define TAG_DESC_SIZE       100     // the size of the description field of the tag
#define MAX_NUM_TOTAL_TAGS  15      // the maximum total number of unique tags


typedef struct _tag {
    int externalID;
    char desc[TAG_DESC_SIZE];
} tag;

int newTag(int exID, char * desc);      // creates a new tag, returns NULL on error
void delTag(int tagID);     // deletes a tag

tag * getTag(int tagID);        // gets a pointer to the specific tag (not useful)
char * getTagDesc(int tagID);   // gets the description of a tag

int doesTagExist(int tagID);    // checks whether a tag ID exists

#endif
