leysö: clean compilecansi linkc



execName = exe

args = 10

extraFilesToClear = 




allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c

compilecansi:
	gcc -c *.c -ansi -Wall -pedantic -error -D_XOPEN_SOURCE=500

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o -lrt 

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

