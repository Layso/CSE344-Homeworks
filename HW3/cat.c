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
	int i = -1;
	
	
	while (strcmp(argv[++i], "cat"));
	
	while (++i < argc && strcmp(argv[i],">") && strcmp(argv[i],"|")) {
		fileDescriptor = open(!strcmp("<", argv[i]) ? argv[++i] : argv[i], O_RDONLY);
		if (fileDescriptor == -1) {
			fprintf(stderr, "Error\nFile couldn't opened: %s\n", argv[i]);
			perror(argv[i]);
			_exit(EXIT_FAILURE);
		}
		
		printf("\nFile: %s\n-----------------\n", argv[i]);
		while (read(fileDescriptor, &character, sizeof(char)) > 0) {
			printf("%c", character);
		}
		
		close (fileDescriptor);
		printf("\n");
	}
	
	printf("%c", '\0');
	return EXIT_SUCCESS;
}
