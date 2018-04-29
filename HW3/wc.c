#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[]) {
	int fileDescriptor;
	char character;
	int lineCount = 0;
	int i = 0;
	
	
	if (argc < 2) {
		fprintf(stderr, "Usage: %s fileName\n", argv[0]);
		_exit(EXIT_FAILURE);
	}
	
	
	while (++i < argc && strcmp(argv[i],">")) {
		fileDescriptor = open(!strcmp("<", argv[i]) ? argv[++i] : argv[i], O_RDONLY);
		if (fileDescriptor == -1) {
			fprintf(stderr, "Error\nFile couldn't opened: %s\n", argv[i]);
			_exit(EXIT_FAILURE);
		}

		while (read(fileDescriptor, &character, sizeof(char)) > 0) {
			if (character == '\n')
				++lineCount;
		}

		printf("Total lines: %s: %d\n", argv[i], lineCount);
		close (fileDescriptor);
	}
	
	return EXIT_SUCCESS;
}