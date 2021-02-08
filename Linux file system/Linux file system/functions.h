#ifndef functions_h
#define functions_h

// Global vars
typedef struct node {
	char name[64];// key
	char nodeType; // D for directory, F for file.
	struct node* childPtr, * siblingPtr, * parentPtr;
}NODE;

// Function declarations
//struct node initialize();


char* dirname(char* pathname);

char* basename(char* pathname);

void dbname(char* pathname);

void initialize();

int findCmd(char* command);

int mkdir(char* pathname); // or return int??

int rmdir(char* pathname);

void cd(char* pathname);

int ls(/*char *pathname*/);

void pwd();

int creat(char* pathname);

int rm(char* filename);

void save(char* filename);

void reload(char* filename);

void menu();

void quit();

// Helper functions
NODE *search(NODE* fileSys, char* pathName);

void show(NODE* fileSys);

#endif // !functions.h