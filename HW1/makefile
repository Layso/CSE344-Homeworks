# Main command requested by homework PDF
# 1 Compiles the project
layso: compile link

# Auto build & run command for developing
# 1 Deletes the remainings of older builds
# 2 Clears terminal screen
# 3 Compiles all C files
# 4 Links all compiled files into an executable
# 5 Runs the executable with fileName variable as command line argument
dev: clean compile link run

# Auto build & run command with valgrind for memory leak check
# 1 Does same with 'dev' command until 5th step
# 2 Runs the executable with valgrind instead of directly running it
val: clean compile link runValgrind

devwo:clean compilewo link run


# Variable to give as command line argument. Holds the image path
fileName = test.tif



# Compiles all C files
compile:
	gcc -c *.c -ansi -Wall -pedantic -errors

# Compiles without checking warnings
compilewo:
	gcc -c *.c

# Links all Object files into an executable named exe
link:
	gcc -o exe *.o

# Runs the executable named exe, gives fileName as second argument
run:
	./exe $(fileName)

# Runs the executable named exe with valgrind, gives fileName as second argument
runValgrind:
	valgrind ./exe $(fileName)

# Cleares the terminal
clear:
	clear

# Removes all Object files and exe file, clears terminal afterwards
clean: clear
	rm -f *.o exe



