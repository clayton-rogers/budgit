// *** Top level of the running program *** //
#include "program.h"

// starts the program running
void start (runinfo infos) {

	account* acc = NULL;

	int input;				// inputed character
	int scrollPos = 0;
	int numLines;
	int needRef;			// whether a refresh is needed
	int entryNum;
	char running = 1;
	char err = 0;
	
	acc = loadAccount("acct/test.act");
	if (acc == NULL) {
		printf("Account could not be loaded from file\n");
		return;
	}
	
	
	init();			//start curses
	
	numLines = calcTotalLines(acc);
	
	writeScreen(acc, scrollPos);
	
	// *** MAIN PROGRAM LOOP *** //
	while (running) {
		input = getch();
		
		// just refresh the screen if no account is load and the key is not valid
		if (acc == NULL && input != 'q' && input != 'l' && input != 'L') 
			input = 410;
		
		// figure out what the user wants to do
		switch (input) 	{
			case 'q':
				running = 0;
				break;
			case 410:
				needRef = 1;
				break;
			case KEY_UP:
				needRef = 1;
				--scrollPos;
				if (scrollPos < 0) scrollPos = 0;
				break;
			case KEY_DOWN:
				needRef = 1;
				++scrollPos;
				if (scrollPos > numLines) scrollPos = numLines;
				break;
			case 'a':						// add entry
			case 'A':
				if (gaddEntry(acc)) {
					// an entry was added
					numLines = calcTotalLines(acc);
					needRef = 1;
				} else {
					// an entry was not added
					needRef = 1;	// still need to refresh the screen to get
									// rid of the interface for adding
				}
				break;
			case 'r':						// remove entry
			case 'R':
				entryNum = selectEntry(acc, &scrollPos);
				if (entryNum == -1) {
					needRef = 1;
					break;
				}
				delEntry(acc, entryNum);
				numLines = calcTotalLines(acc);
				needRef = 1;
				break;
			case 'e':						// error on purpose
			case 'E':
				running = 0;
				err = 10;
				break;
			
			case 's':						// save
			case 'S':
				if (gwriteAccount(acc)) {
					running = 0;
					err = 3;
				}
				needRef = 1;
				break;
			case 'l':						// load
			case 'L':
				// get rid of the current account
				delAccount(acc);
				acc = NULL;			// the account has now been deleted
				
				// get the filename and load the account into the pointer
				acc = gloadAccount();
				needRef = 1;
				scrollPos = 0;
				break;
		}
		
		// refresh the screen if it is nesesary
		if (needRef) {
			writeScreen(acc, scrollPos);
			needRef = 0;
		}
	}
	
	
	endinit();		// end curses
	
	if (err) printf("There was an error during runtime.\n");

	
	
	
	

}



