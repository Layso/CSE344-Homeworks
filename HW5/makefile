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
	gcc -c *.c -ansi -pedantic -error

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o -pthread -lm

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

