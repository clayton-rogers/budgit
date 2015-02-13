#include "tag.h"

// GLOBAL //
int isFirstRun_TAG = 1;				// so that we can initialize on first run
tag * tags[MAX_NUM_TOTAL_TAGS];

int newTag (int exID, char * desc) {
	if (isFirstRun_TAG) {
		isFirstRun_TAG = 0;
		// do init stuff
		int i;
		for (i = 0; i < MAX_NUM_TOTAL_TAGS; ++i) {
			tags[i] = NULL;
		}
	}
	
	int i;
	int isGood = 0;
	for (i = 0; i < MAX_NUM_TOTAL_TAGS; ++i) {
		// return i of unused tag
		if (tags[i] == NULL) {
			isGood = 1;
			break;
		}
	}
	
	if (!isGood) {
		return 0;
	}
	
	// We have a found an unused index, fill it
	tags[i] = (tag *) malloc (sizeof(tag));
	
	// copy the data 
	strncpy(tags[i]->desc, desc, TAG_DESC_SIZE);
	tags[i]->externalID = exID;
	
	// return the created tag
	return 0;
}

void delTag (int tagID) {
	if (tagID < 0 || tagID >= MAX_NUM_TOTAL_TAGS) {
		return;
	}
	// free the memory and set the pointer to NULL
	free((void*) tags[tagID]);
	tags[tagID] = NULL;
}

tag * getTag (int tagID) {
	if (tagID < 0 || tagID >= MAX_NUM_TOTAL_TAGS) {
		return NULL;
	}
	return tags[tagID];
}

char * getTagDesc (int tagID) {
	if (tagID < 0 || tagID >= MAX_NUM_TOTAL_TAGS) {
		return NULL;
	}
	
	// if the tag is unused it will have no desc
	if (tags[tagID] == NULL) {
		return NULL;
	}
	return tags[tagID]->desc;
}

int doesTagExist (int tagID) {
	
	if (tagID >= MAX_NUM_TOTAL_TAGS || tagID < 0) {
		return 0;
	}
	
	if (tags[tagID] == NULL) {
		return 0;
	} else {
		return 1;
	}
}