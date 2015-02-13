#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string.h>		// strncpy
#include <stdio.h>		// error mesg
#include <stdlib.h>		// for malloc

#define ACCOUNT_NAME_SIZE 	100		// size of the name field of an account
#define	ENTRY_DESC_SIZE 	100		// size of the description field of an entry
#define MAX_NUM_TAGS		15		// the maximum number of tags that can be assosiated with an entry

typedef struct _date {
	char day;
	char month;
	char year;
} date;

typedef struct _entry {
	date Date;						// date of the transaction
	char desc[ENTRY_DESC_SIZE];		// description of the transaction
	char type;						// 0 - debit, 1 - credit, else - error
	int amount;						// amount to be cashed
	int balance;					// the balace imediately after the transaction
	int tags[MAX_NUM_TAGS];			// the tags associated with the enty
} entry;
	
typedef struct _account {
	char name[ACCOUNT_NAME_SIZE];
	int balanceFoward;			// the balance the account should start with
	int numEntry;				// the number of entries in the account
	entry **trans;				// array of transactions
} account;

// manipulate accounts
account * newAccount (char name[], int balanceFoward);	// creates a new account in mem
void delAccount (account* myAccount);					// deletes an account in mem

// manipulate entries of accounts
int addEntry (account* myAccount, entry myEntry);		// adds an entry to my account
														// returns 1 if there is an error TODO: just crash if there is an error
void delEntry (account* myAccount, int entryNum);		// remove the entry with given number
entry * getEntry (account* myAccount, int entryNum);	// safe way to get the entry pointer

#endif
