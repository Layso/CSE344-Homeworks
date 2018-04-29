#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>



int main(int argc, char *argv[]) {
	int fileDescriptor;
	char character;
	
	
	if (argc != 2) {
		fprintf(stderr, "Usage: %s fileName\n", argv[0]);
		_exit(EXIT_FAILURE);
	}
	
	fileDescriptor = open(argv[1], O_RDONLY);
	if (fileDescriptor == -1) {
		fprintf(stderr, "Error\nFile couldn't opened: %s\n", argv[1]);
		_exit(EXIT_FAILURE);
	}
	
	while (read(fileDescriptor, &character, sizeof(char)) > 0) {
		printf("%c", character);
	}
	
	close (fileDescriptor);
	return EXIT_SUCCESS;
}
