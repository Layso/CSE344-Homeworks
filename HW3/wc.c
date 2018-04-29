#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[]) {
	int fileDescriptor;
	char character;
	int lineCount = 0;
	int i = -1;
	int bytes = 0;
	int index = 0;
	
	
	/* Reassigning argc since it includes whole command line */
	while (argv[++i] != NULL) {
		if (!strcmp(argv[i], "wc")) {
			index = i;
		}
	}
	
	
	if (index != 0) {
		while ((bytes += read(STDIN_FILENO, &character, sizeof(char))) > 0) {
			if (character == '\n')
				++lineCount;
				
			if (character == '\0') {
				break;
			}
		}
		printf("Total lines: %d\n", lineCount);
	}
	
	else {
		i = 0;
		while (++i < argc && strcmp(argv[i],">")) {
			fileDescriptor = open(!strcmp("<", argv[i]) ? argv[++i] : argv[i], O_RDONLY);
			if (fileDescriptor == -1) {
				fprintf(stderr, "Error\nFile couldn't opened: %s\n", argv[i]);
				return EXIT_FAILURE;
			}

			while (read(fileDescriptor, &character, sizeof(char)) > 0) {
				if (character == '\n')
					++lineCount;
			}

			printf("Total lines: %s: %d\n", argv[i], lineCount);
			close (fileDescriptor);
		}
	}
	
	return EXIT_SUCCESS;
}