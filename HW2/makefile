leysö: compile link 

N = 5
M = 50
X = fileawd.dat

all: clean leysö run

compile:
	gcc -c 151044001_main.c ParentHelpers.c ChildHelpers.c -D_XOPEN_SOURCE=500 -ansi -Wall -pedantic -error
	
link:
	gcc -o multiprocess_DFT 151044001_main.o ParentHelpers.o ChildHelpers.o -lm

run:
	./multiprocess_DFT -N $(N) -X $(X) -M $(M)

runValgrind:
	valgrind ./multiprocess_DFT -N $(N) -X $(X) -M $(M)

clean:
	rm -f *.o *.log multiprocess_DFT
	clear

