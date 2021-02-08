#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "functions.h"

//// Global vars
//typedef struct node {
//	char name[64];// key
//	char nodeType; // D for directory, F for file.
//	struct node* childPtr, * siblingPtr, * parentPtr;
//}NODE;

// Possible commands given by user.
char* cmd[] = { "mkdir", "rmdir", "ls", "cd", "pwd", "creat", "rm",
"reload", "save", "menu", "quit", NULL };

NODE* root, * cwd; // root and CWD pointers
//char line[128]; // user input command line
//char command[16], pathname[64]; // command and pathname strings
char dname[64], bname[64]; // dirname and basename string holders

char* dirname(char* pathname)
{
	static const char dot[] = ".";
	char* last_slash;
	/* Find last '/'.  */
	last_slash = pathname != NULL ? strrchr(pathname, '/') : NULL;
	if (last_slash != NULL && last_slash != pathname && last_slash[1] == '\0')
	{
		/* Determine whether all remaining characters are slashes.  */
		char* runp;
		for (runp = last_slash; runp != pathname; --runp)
			if (runp[-1] != '/')
				break;
		/* The '/' is the last character, we have to look further.  */
		/*if (runp != pathname)
			last_slash = __memrchr(pathname, '/', runp - pathname);*/
	}
	if (last_slash != NULL)
	{
		/* Determine whether all remaining characters are slashes.  */
		char* runp;
		for (runp = last_slash; runp != pathname; --runp)
			if (runp[-1] != '/')
				break;
		/* Terminate the path.  */
		if (runp == pathname)
		{
			/* The last slash is the first character in the string.  We have to
			   return "/".  As a special case we have to return "//" if there
			   are exactly two slashes at the beginning of the string.  See
			   XBD 4.10 Path Name Resolution for more information.  */
			if (last_slash == pathname + 1)
				++last_slash;
			else
				last_slash = pathname + 1;
		}
		else
			last_slash = runp;
		last_slash[0] = '\0';
	}
	else
		/* This assignment is ill-designed but the XPG specs require to
		   return a string containing "." in any case no directory part is
		   found and so a static and constant string is required.  */
		pathname = (char*)dot;
	return pathname;
}

char* basename(char* pathname)
{
	char* p;
	if (pathname == NULL || pathname[0] == '\0')
		/* We return a pointer to a static string containing ".".  */
		p = (char*)".";
	else
	{
		p = strrchr(pathname, '/');
		if (p == NULL)
			/* There is no slash in the filename.  Return the whole string.  */
			p = pathname;
		else
		{
			if (p[1] == '\0')
			{
				/* We must remove trailing '/'.  */
				while (p > pathname && p[-1] == '/')
					--p;
				/* Now we can be in two situations:
				   a) the string only contains '/' characters, so we return
					  '/'
				   b) p points past the last component, but we have to remove
					  the trailing slash.  */
				if (p > pathname)
				{
					*p-- = '\0';
					while (p > pathname && p[-1] != '/')
						--p;
				}
				else
					/* The last slash we already found is the right position
					   to return.  */
					while (p[1] != '\0')
						++p;
			}
			else
				/* Go to the first character of the name.  */
				++p;
		}
	}
	return p;
}

void dbname(char* pathname)
{
	char temp[128]; // dirname(), basename() destroy original pathname
	strcpy(temp, pathname);
	strcpy(dname, dirname(temp));
	strcpy(temp, pathname);
	strcpy(bname, basename(temp));
}

// maybe void since root is global?
//struct node initialize() {
void initialize() {
	// don't have to return because root is a global
	// initialize root node.
	root = (NODE*)malloc(sizeof(NODE));
	root->parentPtr = root; // No parent, pts to itself
	root->siblingPtr = root; // No siblings, pts to itself
	root->childPtr = NULL;
	strcpy_s(root->name, sizeof(root->name), "Root");
	root->nodeType = 'D';

	// don't have to return because cwd is a global
	// initialize cwd node.
	cwd = (NODE*)malloc(sizeof(NODE));
	cwd = root; // At initialization, cwd is root.

	// Testing
	/*printf("Inside initialize() \n");
	printf("root->name: %s \n", root->name);
	printf("root->siblingPtr->name: %s \n", root->siblingPtr->name);

	printf("Leaving initialize() \n");*/
}

int findCmd(char* command)
{
	int i = 0;
	while (cmd[i]) {
		if (!strcmp(command, cmd[i]))
			return i; // found command: return index i
		i++;
	}
	return -1; // not found: return -1
}

NODE* search(NODE* fileSys, char* dirName)
{
	//
	// Have to error check for NULL or empty dirname or filesys??
	//

	char* dir; // to store the single directory searching for.
	char tempDir[64];
	strcpy(tempDir, dirName);

	dir = strtok(tempDir, "/");

	NODE* traverseP = cwd;

	if (dirName[0] == '/') {
		traverseP = root;
	}
	else {
		traverseP = cwd;
	}

	// Have to check for file/Dir type
	while (dir != NULL && traverseP != NULL)// checking for end point of either.
	{
		if (traverseP->childPtr != NULL)
		{
			traverseP = traverseP->childPtr; // advancing to next child node.
		}
		while (traverseP != NULL && !(strcmp(traverseP->name, dir)))
		{
			traverseP = traverseP->siblingPtr; // advancing to next sibling node.
		}

		dir = strtok(0, "/");
	}

	return traverseP;
}

int mkdir(char* pathname)
{
	// Check for NULL
	if (pathname == NULL) {
		printf("invalid pathname.");
		return 0;
	}
	// Split pathname into dirname and basename
	dbname(pathname);

	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->parentPtr = newNode->siblingPtr = newNode->childPtr = NULL; // No siblings, or children on initialization.
	strcpy_s(newNode->name, sizeof(newNode->name), bname);
	newNode->nodeType = 'D';

	// searh for dirname node
	NODE* tempNode = search(root, dname);
	// check file/dir type here???
	if (tempNode->nodeType != 'D') {
		printf("Must be a directory.");
		return 0;
	}
	// check if Dir childPtr full
	if (tempNode->childPtr == NULL) {
		tempNode->childPtr = newNode;
		newNode->parentPtr = tempNode;
	}
	else {
		NODE* temp = tempNode->childPtr;

		while (temp->name == bname && temp->siblingPtr) {
			temp = temp->siblingPtr;
		}
		if (tempNode->childPtr->name == bname) {
			printf("This Directory already exists.");
			return 0;
		}
		temp->siblingPtr = newNode;
		newNode->parentPtr = temp;
	}

	return 1;
}

int rmdir(char* pathname) {
	// Check for NULL
	if (pathname == NULL) {
		printf("invalid pathname.");
		return 0;
	}

	// Split pathname into dirname and basename
	dbname(pathname);

	// searh for dirname node
	NODE* tempNode = search(root, dname);
	// check to see if found?
	// check file/dir type here???
	// check to make sure dir is empty?
	tempNode = tempNode->parentPtr;
	if (tempNode->name == root->name) {
		printf("Cannot remove root.");
		return 0;
	}
	else {
		tempNode->childPtr = NULL;
	}
	return 1;
}
void cd(char* pathname)
{
	//doesn't return anything, changes global cwd.
	// Check for NULL
	if (pathname == NULL)
	{
		printf("\n\n Please enter a valid pathname.");
		return;
	}
	// Split pathname into dirname and basename
	dbname(pathname);

	// searh for dirname node
	NODE* tempDir = search(root, dname);
	//check file/dir type
	// check for errors here.
	printf("\nTesting, in cd. tempDir->name: %s, tempdir->nodeType: %c", tempDir->name, tempDir->nodeType);
	if (tempDir->nodeType == 'D')
		cwd = tempDir;
}
int ls()
{
	NODE* tempDir = cwd;
	if (tempDir->nodeType == 'D') {
		NODE* temp2 = tempDir->childPtr;
		if (temp2 == NULL) {
			return 1;
		}
		while (temp2) // looking for the end.
		{
			printf("[%c %s] ", temp2->/*childPtr->*/nodeType, temp2/*->childPtr*/->name);
			temp2 = temp2->siblingPtr;
		}
	}
	else {
		printf("[%c %s]", tempDir->/*childPtr->*/nodeType, tempDir/*->childPtr*/->name);
	}
	return 1;
}
void pwd()
{
	// save name of the current Node
	char* path = cwd->name;
	char token = '\\';
	const char limit = " ";
	char* str;
	//change to if?
	while (cwd->name != root->name)
	{
		//first add the slash
		path = strcat(token, path);
		path = strcat(cwd->parentPtr->name, path);
		
		cwd = cwd->parentPtr;
	}
	
	str = strtok(path, limit);
	while (str) {
		printf("%s", str);
	}

	//for (int i = 0; i < strlen(path); i++)
	//{
	//	//printf("%c%c ", path[i], token);
	//	printf("%s%c ", cwd->name, token);
	//}

	//printf("%s, %c", path, token);


	// Need to reset cwd
	cwd = search(root, path);
	printf("Testing, inside pwd, cwd = %s \n", cwd->name);
}
int creat(char* pathname)
{
	// Check for NULL
	if (pathname == NULL) {
		printf("invalid pathname.");
		return 0;
	}
	// Split pathname into dirname and basename
	dbname(pathname);

	// searh for dirname node
	NODE* tempNode = search(root, dname);
	// check file/dir type here???
	if (tempNode->nodeType == 'F') {
		printf("Cannot make directory in file.");
		return 0;
	}

	// dirname exists and is a DIR
	// search for the basename in/under the dirname node;
	else if (tempNode->childPtr->name == bname) {
		printf("This Directory already exists.");
		return 0;
	}

	// else ADD a new DIR node under dirname and return success.
	NODE* newNode = (NODE*)malloc(sizeof(NODE));
	newNode->parentPtr = cwd;
	cwd->childPtr = newNode;
	newNode->siblingPtr = newNode->childPtr = NULL; // No siblings, or children on initialization.
	strcpy_s(newNode->name, sizeof(newNode->name), bname);
	newNode->nodeType = 'F';

	return 1;
}
int rm(char* filename)
{
	// Check for NULL
	if (filename == NULL) {
		printf("invalid pathname.");
		return 0;
	}

	// Split pathname into dirname and basename
	dbname(filename);

	// searh for dirname node
	NODE* tempNode = search(root, dname);
	// check to see if found?
	// check if file
	tempNode = tempNode->parentPtr;
	if (tempNode->name == root->name) {
		printf("Cannot remove root.");
		return 0;
	}
	else {
		tempNode->childPtr = NULL;
	}
	return 1;
}

// need filename as arg to save?
void save(char* fileName)
{
	NODE* tempNode = root->childPtr; // can omit root from file
	FILE* fp = fopen("/tmp/test.txt", "w+"); // fopen a FILE stream for WRITE

	while (tempNode->childPtr)
	{
		fprintf(fp, "%c %s", tempNode->nodeType, tempNode->name); // print a line to file

		if (tempNode->childPtr)
		{
			fprintf(fp, "%c %s", tempNode->nodeType, tempNode->name);
			fprintf(fp, "%c %s", tempNode->childPtr->nodeType, tempNode->childPtr->name);
		}
		if (tempNode->siblingPtr)
		{
			fprintf(fp, "%c %s", tempNode->nodeType, tempNode->name);
			fprintf(fp, "%c %s", tempNode->siblingPtr->nodeType, tempNode->siblingPtr->name);
		}

		// Now we advance the tempNode
		tempNode = tempNode->childPtr;
	}
	fclose(fp); // close FILE stream when done
}
// need filename as arg to read?
void reload(char* fileName)
{
	// check for null file
	char type[64];
	//char name[64];
	char* name = "";


	initialize();
	FILE* fp = fopen("/tmp/test.txt", "r+"); // fopen a FILE stream for READ
	while (!feof(fp))
	{
		fscanf(fp, "%s %s", type, name);
		if (type == "D")
		{
			mkdir(name);
		}
		else
		{
			creat(name);
		}

		fclose(fp);
	}
}
void menu()
{
	printf("Menu:\n");
	printf("mkdir  <[pathname]dir_name> -create a directory structure in the file system\n");
	printf("rmdir  <[pathname]dir_name> -remove or delete a directory structure\n");
	printf("cd  <pathname> -change the current working directory\n");
	printf("ls - list the contents of the current working directory\n");
	printf("pwd - print the current or \"working\" directory\n");
	printf("creat  <[pathname / ]filename> -create a new file\n");
	printf("rm  <[pathname / ]filename> -remove an existing file\n");
	printf("save - save the file system and contents to a real file\n");
	printf("reload - load a previously saved file system and its contents\n");
	printf("menu - show a help menu with the available commands in the simulator\n");
	printf("quit - exit the simulation\n");
}
void quit()
{
	save("file.txt");
	exit(0);
}
void show(NODE* fileSys)
{
	if (fileSys)//checking for NULL
	{
		printf("Node Name: %s\t Node Type: %c\n", fileSys->name, fileSys->nodeType);

		if (fileSys->childPtr && fileSys->childPtr != fileSys->childPtr)
		{
			show(fileSys->childPtr);
		}
		//root siblingPtr pts to itself so this will loop infinitely
		if (fileSys->siblingPtr && fileSys->siblingPtr != fileSys->siblingPtr)
		{
			show(fileSys->siblingPtr);
		}
	}

}

;
