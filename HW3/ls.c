#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>



#define ZERO 0
#define ERROR_CODE -1
#define CURRENT_DIR "."
#define PREVIOUS_DIR ".."



int main(int argc, char *argv[]) {
	DIR *dirPtr = NULL;struct dirent *direntPtr = NULL;
	struct stat fileStruct;
	
	
	
	memset(&fileStruct, ZERO, sizeof(struct stat));
	dirPtr = opendir(CURRENT_DIR);
	
	
	if (dirPtr == NULL) {
		fprintf(stderr, "\nError\nDirectory pointer couldn't acquired\n%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	else {
		printf(" Type\tPermissions\tSize\t   File Name\n");
		while ((direntPtr = readdir(dirPtr)) != NULL) {
			if (strcmp(CURRENT_DIR, direntPtr->d_name) && strcmp(PREVIOUS_DIR, direntPtr->d_name)) {
				stat(direntPtr->d_name, &fileStruct);
				
				printf((S_ISDIR(fileStruct.st_mode)) ? "  d" : S_ISLNK(fileStruct.st_mode) ? "  l" : "  -");
				printf((fileStruct.st_mode & S_IRUSR) ? "\t r" : "\t -");
				printf((fileStruct.st_mode & S_IWUSR) ? "w" : "-");
				printf((fileStruct.st_mode & S_IXUSR) ? "x" : "-");
				printf((fileStruct.st_mode & S_IRGRP) ? "r" : "-");
				printf((fileStruct.st_mode & S_IWGRP) ? "w" : "-");
				printf((fileStruct.st_mode & S_IXGRP) ? "x" : "-");
				printf((fileStruct.st_mode & S_IROTH) ? "r" : "-");
				printf((fileStruct.st_mode & S_IWOTH) ? "w" : "-");
				printf((fileStruct.st_mode & S_IXOTH) ? "x" : "-");
				printf("\t%ld", fileStruct.st_size);
				printf("\t   %s\n", direntPtr->d_name);
			}
		}
	}
	
	closedir(dirPtr);

	
	return 0;
}