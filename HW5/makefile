leysö: clean compilecansi linkc



execName = exe

args = data.dat

extraFilesToClear = 




allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c

compilecansi:
	gcc -c *.c -ansi -Wall -pedantic -error -pthread

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o

linkcpp:
	g++ -o $(execName) *.o




run:
	./$(execName) $(args)

runVal:
	valgrind ./$(execName) $(args)



clean: clear
	rm -f *.o $(execName) $(extraFilesToClear)

clear:
	clear

