leysö: compile link 

N = 5
M = 100
X = file.dat

all: clean leysö run

compile:
	gcc -c 151044001_main.c ParentHelpers.c ChildHelpers.c -D_XOPEN_SOURCE=500 -ansi -Wall -pedantic -error
	
link:
	gcc -o multiprocess_DFT 151044001_main.o ParentHelpers.o ChildHelpers.o

run:
	./multiprocess_DFT -N $(N) -X $(X) -M $(M)

clean:
	rm -f *.o multiprocess_DFT
	clear

