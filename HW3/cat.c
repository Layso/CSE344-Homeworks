#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>



int main(int argc, char *argv[]) {
	int fileDescriptor;
	char character;
	int i=0;
	
	if (argc < 2) {
		fprintf(stderr, "Usage: %s fileName\n", argv[0]);
		_exit(EXIT_FAILURE);
	}
	
	while (++i < argc && strcmp(argv[i],">")) {
		fileDescriptor = open(!strcmp("<", argv[i]) ? argv[++i] : argv[i], O_RDONLY);
		printf("\n\n\n%s\n-----------------\n", argv[i]);
		if (fileDescriptor == -1) {
			fprintf(stderr, "Error\nFile couldn't opened: %s\n", argv[i]);
			perror(argv[i]);
			_exit(EXIT_FAILURE);
		}
		
		while (read(fileDescriptor, &character, sizeof(char)) > 0) {
			printf("%c", character);
		}
		
		close (fileDescriptor);
		
	}
	return EXIT_SUCCESS;
}
