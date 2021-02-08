#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"functions.h"

char line[128]; // user input command line
char command[16], pathname[64]; // command and pathname strings

int main() {
	int index;

	// Call to initialize root node.
	initialize();

	// Testing
	//printf("root->siblingPtr->name: %s\n\n" ,root->siblingPtr->name);

	while (1) {
		// Testing
		/*printf("Testing show(root) at beginning of while loop: ");
		show(root);
		printf("root->childPtr->name: %s\n\n", root->childPtr->name);
		printf("root->name: %s\n\n", root->name);*/

		//Prompt user
		printf("Input a command: ");

		fgets(line, 128, stdin); // get at most 128 chars from stdin
		line[strlen(line) - 1] = 0; // kill \n at end of line

		// Had to add CRT_SECURE_NO_WARNINGS in preprocessor definitions
		sscanf(line, "%s %s", command, pathname);

		// Testing
		/*printf("command = %s \n", command);
		printf("pathname = %s \n", pathname);*/
		//printf("%c \n", pathname[0]);

		// Testing
		/*dbname(pathname);
		printf("dname = %s \n", dname);
		printf("bname = %s \n", bname);
		printf("Testing show() : ");
		show(root);

		printf("cwd->name = %s \n", cwd->name);*/

		index = findCmd(command);
		switch (index) {
		case 0: mkdir(pathname); break;
		case 1: rmdir(pathname); break;
		case 2: ls(pathname); break;
		case 3: cd(pathname); break;
		case 4: pwd(); break;
		case 5: creat(pathname); break;
		case 6: rm(pathname); break;
		case 7: reload("filename"); break;
		case 8: save(pathname); break;
		case 9: menu(); break;
		case 10: quit(); break;
		default: printf("invalid command % s \n", command);
		}

		printf("\n");
	}

	return 0;
}

