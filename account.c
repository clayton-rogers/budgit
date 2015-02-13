#include "account.h"


// *** helper funcs *** //
void swapEntries (account* myAccount, int entryNum1, int entryNum2);	// swaps two entries
void sortAccount(account* myAccount);
int datecmp(date date1, date date2);				// compares dates, like strcmp
int entrydatecmp(entry* entry1, entry* entry2);		// compares the dates of entries
void balancaAccount (account* myAccount);			// updates the 'balance' field


void swapEntries(account* myAccount, int entryNum1, int entryNum2) {
	
	entry *tempEntry;
	
	// swap the two transactions tempEntry = getEntry(myAccount, entryNum1); 
	tempEntry = getEntry(myAccount, entryNum1);
	myAccount->trans[entryNum1] = getEntry(myAccount, entryNum2); 
	myAccount->trans[entryNum2] = tempEntry;

}

void sortAccount(account* myAccount) {
	// sorts all the entries of an account
	int isSorted = 0;		// False
	int i;
	int result;
	
	while (!isSorted) {
		isSorted = 1;		// True
		
		// the last entry is located at 'getEntry(myAccount, myAccount->numEntry-1)
		
		for (i = 0; i < myAccount->numEntry-1; ++i) {			// -1 cause we don't have to go right to the end of the list
			result = entrydatecmp(getEntry(myAccount,i),getEntry(myAccount, i+1));
			if (result > 0) {
				// first entry is bigger, we are sorting from small to big
				swapEntries(myAccount, i, i+1);
				isSorted = 0;		// False
			}
			
		}
	}
	
}

int datecmp (date date1, date date2) {
	if (date1.year > date2.year) {
		return 1;
	} else if (date1.year < date2.year) {
		return -1;
	}
	
	if (date1.month > date2.month) {
		return 1;
	} else if (date1.month < date2.month) {
		return -1;
	}
	
	if (date1.day > date2.day) {
		return 1;
	} else if (date1.day < date2.day) {
		return -1;
	}
	
	// they are equal dates
	return 0;	
}

int entrydatecmp(entry* entry1, entry* entry2) {
	
	// empty entry is biggest, put at end
	if (entry1 == NULL) {
		return 1;
	}
	if (entry2 == NULL) {
		return -1;
	}
	
	return datecmp(entry1->Date, entry2->Date);
}

void balanceAccount (account * myAccount) {

	entry* curEntry;
	int balance = myAccount->balanceFoward;
	int i;
	
	for (i = 0; i < myAccount->numEntry; ++i) {
		curEntry = getEntry(myAccount, i);
		if (curEntry->type == 0) {
			// debit
			balance -= curEntry->amount;
		} else {
			// credit
			balance += curEntry->amount;
		}
		curEntry->balance = balance;
	}
}


// *** Actual code *** //
account * newAccount (char name[], int balanceFoward) {
	// create the account
	account *myAccount = (account*)malloc(sizeof(account));
	if (myAccount == NULL) {
		printf("Not enough memory for new account\n");
		return NULL;
	}
	
	// load the name field
	strncpy(myAccount->name, name, ACCOUNT_NAME_SIZE);
	
	myAccount->balanceFoward = balanceFoward;
		
	// there are no entries yet
	myAccount->numEntry = 0;	
	myAccount->trans = NULL;
	
	return myAccount;
}

void delAccount (account* myAccount) {
	// need to delete all the entries as well as the account itself
	
	// can't delete what doesn't exist
	if (myAccount == NULL) return;
	
	// first delete any transactions the account may have
	if (myAccount->numEntry > 0) {
		int i;
		for (i = 0; i < myAccount->numEntry; ++i) {
			delEntry(myAccount, i);
		}
	}
	
	// remove the account itself
	free ( (void*) myAccount);
}

int addEntry (account* myAccount, entry myEntry) {
	
	entry** tmpTrans;
	
	if (myAccount == NULL) {
		printf("The account does not exist");
		return 15;
	}
	
	// Make space for the entry
	entry* newEntry = (entry*) malloc(sizeof(entry));
	if (newEntry == NULL) return 1;
	
	// if there is already 
	if (myAccount->numEntry != 0) {
		tmpTrans = malloc(sizeof(entry*) * (myAccount->numEntry));
		if (tmpTrans == NULL) {
			free(newEntry);
			return 2;
		}
	
		// copy the prev entries to tmp
		memcpy(tmpTrans, myAccount->trans, sizeof(entry*)*myAccount->numEntry);
		// clean the prev storate
		free(myAccount->trans);
	}
	
	// Allocate enough space for prev entries plus the one new one
	myAccount->trans = malloc (sizeof(entry*) * (myAccount->numEntry+1));
	if (myAccount->trans == NULL) {
		free(newEntry);
		return 2;
	}
	
	if (myAccount->numEntry != 0) {
		// copy all the previous entries back
		memcpy(myAccount->trans, tmpTrans, sizeof(entry*)*myAccount->numEntry);
	}
	
	// increase the number of entries in the account
	myAccount->numEntry ++;
	
	// link the entry into the account
	myAccount->trans[myAccount->numEntry-1] = newEntry;
	
	// load all the information into the new entry
	newEntry->Date = myEntry.Date;
	strncpy(newEntry->desc, myEntry.desc, ENTRY_DESC_SIZE);
	newEntry->type = myEntry.type;
	newEntry->amount = myEntry.amount;
	int i;
	for (i = 0; i < MAX_NUM_TAGS; ++i) {
		newEntry->tags[i] = myEntry.tags[i];
	}
	
	
	// the entry was added to the end so we must sort and balance the account
	sortAccount (myAccount);
	balanceAccount (myAccount);
	
	return 0;
}

void delEntry (account* myAccount, int entryNum) {
	
	// in case the entryNum is invalid
	if (entryNum >= myAccount->numEntry) return;
	
	// get the entry pointer and delete the mem
	entry * myEntry = getEntry(myAccount, entryNum);
	free ( (void*) myEntry );
		
	myAccount->trans[entryNum] = NULL;
	sortAccount(myAccount);				// puts the null at the end
	
	// account now has one less entry
	myAccount->numEntry--;
	
	// copy to a new location, then copy back
	entry** tmpTrans = malloc(sizeof(entry*) * myAccount->numEntry);
	if (tmpTrans == NULL) return;
	memcpy(tmpTrans, myAccount->trans, sizeof(entry*)*myAccount->numEntry);
	
	// free the prev mem
	free(myAccount->trans);
	
	// reallocate a smaller chunk and move it back from tmp storage
	myAccount->trans = malloc(sizeof(entry*)*myAccount->numEntry);
	if (myAccount->trans == NULL) return;
	
	memcpy(myAccount->trans, tmpTrans, sizeof(entry*)*(myAccount->numEntry));
	
	// free then tmp storage
	free ( (void*) tmpTrans);
	
	// finally, update the balances
	balanceAccount(myAccount);
}

entry * getEntry(account *myAccount, int entryNum) {
	entry *retEntry;
	
	// if the index is invalid, return NULL
	if (entryNum < 0) {
		return NULL;
	}
	
	if (entryNum >= myAccount->numEntry) {
		return NULL;
	}
	
	
	// Else return the proper entry
	retEntry = myAccount->trans[entryNum];
	
	return retEntry;		
}
