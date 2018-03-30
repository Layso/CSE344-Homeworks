leysö: clean compile link 


N = 5
M = 100
X = file.dat

all: leysö run

compile:
	gcc -c 151044001_main.c -ansi -Wall -pedantic -errors
	
link:
	gcc -o multiprocess_DFT 151044001_main.o

run:
	./multiprocess_DFT -N $(N) -X $(X) -M $(M)

clean:
	rm -f *.o multiprocess_DFT
	clear

