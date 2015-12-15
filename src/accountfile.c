#include "accountfile.h"

#include <stdio.h>                  // file input / output
#include <string.h>
#include <ctype.h>
#include "tag.h"                    // tag definitions

// *** Helper *** //
static void splitCents(int *dollars, int *cents, int inCents) {
    // splits cents into cents and dols

    *dollars = (inCents)/100;       // supposed to be int div
    *cents = inCents%100;
}


// *** Code *** //
account * loadAccount (char* filename) {

    FILE* fp = NULL;
    account* myAccount;
    entry tmpEntry;
    char buffer [BUFFER_SIZE];
    int pt;
    char accName [ACCOUNT_NAME_SIZE];
    int balanceFoward = 0;                  // it is optional, default 0
    int dollars, cents, ret;
    float tmpBalance;
    char tempDesc[TAG_DESC_SIZE];
    char tempTAGS[100];
    int tagID;

    accName[0] = '\0';                      // so we can check if it's modified

    // open the file
    fp = fopen (filename, "r");
    if (fp == NULL) {
        printf("Could not open the file\n");
        return NULL;
    }

    // Read until we get to the metadata section
    while (1) {
        fgets(buffer, BUFFER_SIZE, fp);
        if (ferror(fp) || feof(fp)) {
            return NULL;
        }

        if (strncmp(buffer, "META:", 5) == 0) break; // when reached the meta section
    }

    // read the metadata
    while (1)  {
        fgets(buffer, BUFFER_SIZE, fp);
        if (ferror(fp) || feof(fp)) {
            if (accName[0] == '\0') {
                return NULL;                    // account name is the min info we need
            } else {
                myAccount = newAccount(accName, balanceFoward);;
                return myAccount;
            }
        }
        if (strncmp(buffer, "TAGS:", 5) == 0) {     // in case we get to the next section
            if(accName[0] == '\0') {                // without getting an account name
                return NULL;
            } else {
                break;
            }
        }
        if (buffer[0] == '#') continue;                 // comments
        if (buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t') continue;
        if (strncmp(buffer, "name=", 5) == 0) {
            sscanf(buffer, "name=%100s", accName);
        }
        if (strncmp(buffer, "balanceFoward=", 14) == 0) {
            sscanf(buffer, "balanceFoward=%f", &tmpBalance);
            tmpBalance *= 100;
            balanceFoward = tmpBalance + 0.0001;
            // the decimal is there so that if the machine precision rounds to
            // x.9999999999 then the correct balance forward will still be had
        }
    }

    // we now have enough info to create the account
    myAccount = newAccount (accName, balanceFoward);
    if (myAccount == NULL) {
        printf("Account could not be created\n");
        return NULL;
    }

    // read in the tags
    while (1) {
        fgets(buffer, BUFFER_SIZE, fp);
        if (ferror(fp) || feof(fp)) break;  // finish when we hit the end of the file (should not occur)
        if (buffer[0] == '#') continue;     // comments
        if (buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t') continue;

        // check if we have reached the next section
        if (strncmp(buffer, "ENTRIES:", 8) == 0) break;

        // load the tag
        if (buffer[0] != '[') continue;
        sscanf(buffer, "[%d] %s", &tagID, tempDesc);

        newTag(tagID, tempDesc);
    }

    // read the entries
    while (1) {
        fgets(buffer, BUFFER_SIZE, fp);
        if (ferror(fp) || feof(fp)) break;  // finish when we hit the end of the file
        if (buffer[0] == '#') continue;     // comments
        if (buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\t') continue;

        // clean up the tmpEntry from last loop
        tmpEntry.Date.day = 0;
        tmpEntry.Date.month = 0;
        tmpEntry.Date.year = 0;
        strcpy(tmpEntry.desc, "");
        tmpEntry.type = 0;
        tmpEntry.amount = 0;
        tmpEntry.balance = 0;

        // get the date
        sscanf(buffer, "%2hhd/%2hhd/%2hhd", &tmpEntry.Date.day,
                                            &tmpEntry.Date.month,
                                            &tmpEntry.Date.year);
        // get rid of the first 8 chars (the date)
        strcpy(buffer, (buffer+8));

        // read in the description
        while (isspace(buffer[0])) {
            strcpy(buffer, (buffer+1));         // get rid of leading spaces
        }
        pt = 0;
        while (1) {
            if (isspace(buffer[0])) {
                // only allow a single spaces in entries
                if (!isspace(tmpEntry.desc[pt-1])) {
                    strncat(tmpEntry.desc, " ", 1);
                    ++pt;
                }
                // move buffer to next letter and continue
                strcpy(buffer, (buffer+1));
                continue;
            }
            if (buffer[1] == ':') break;
            strncat(tmpEntry.desc, buffer, 1);
            ++pt;
            strcpy(buffer, buffer+1);
        }

        // remove trailing spaces
        pt = strlen(tmpEntry.desc);
        --pt;
        tmpEntry.desc[pt] = '\0';

        // the buffer will now only contain the amount of the transaction and the tags
        sscanf(buffer, "%c:%d.%2d %s", &tmpEntry.type, &dollars, &cents, tempTAGS);

        if (tmpEntry.type == 'd' || tmpEntry.type == 'D') {
            tmpEntry.type = 0;
        }
        if (tmpEntry.type == 'c' || tmpEntry.type == 'C') {
            tmpEntry.type = 1;
        }

        cents += dollars*100;
        tmpEntry.amount = cents;

        // add the tags to the entry (clear them all first)
        int i, curTag = 0;
        for (i = 0; i < MAX_NUM_TAGS; ++i) {
            tmpEntry.tags[i] = 0;
        }
        for (i = 0; i <= BUFFER_SIZE; ++i) {
            if (tempTAGS[i] == ' ' || tempTAGS[i] == '\n' || tempTAGS[i] == '\0') break;
            if (tempTAGS[i] == ',') continue;

            // assume it is a number then
            tmpEntry.tags[curTag] = tempTAGS[i];
            ++curTag;
        }

        // finally add the entry to the account
        ret = addEntry(myAccount, tmpEntry);
        if (ret != 0) {
            printf("Error adding entry from file\n");
        }
    }

    // close the file
    fclose(fp);

    return myAccount;
}

int writeAccount (account* myAccount, char* filename) {
    char backupFilename[FILENAME_MAX];
    entry *tmpEntry;
    int dollars, cents;
    int retErr;
    int i;
    FILE * fp;
    backupFilename[0] = '\0';


    // make a backup of the old accountfile
    strncat(backupFilename, filename, FILENAME_MAX);
    strncat(backupFilename, ".bak", 4);

    retErr = rename(filename, backupFilename); // backup the old file if it works
    if (retErr != 0) return 10;


    // open the file
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Could not write to the file\n");
        return 3;
    }

    // write the account info to the file (overwrites the old one)
    fprintf(fp, "META:\n");
    fprintf(fp, "name=%s\n", myAccount->name);
    splitCents(&dollars, &cents, myAccount->balanceFoward);
    fprintf(fp, "balanceFoward=%d.%d\n", dollars, cents);
    fprintf(fp, "\n");

    // write the entries
    fprintf(fp, "ENTRIES:\n");
    for (i = 0; i < myAccount->numEntry; ++i) {
        tmpEntry = getEntry(myAccount, i);
        if (tmpEntry == NULL) break;
        fprintf(fp, "%02d/%02d/%02d\t", tmpEntry->Date.day,
                              tmpEntry->Date.month,
                              tmpEntry->Date.year);
        fprintf(fp, "%s\t\t\t", tmpEntry->desc);
        if (tmpEntry->type == 0) {
            fputc('d', fp);
        } else {
            fputc('c', fp);
        }
        splitCents(&dollars, &cents, tmpEntry->amount);
        fprintf(fp, ":%d.%d\t", dollars, cents);
        // TODO: add tag support
        fputc('\n',fp);
    }

    fclose(fp);

    return 0;
}

