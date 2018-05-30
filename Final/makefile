leysö: clean compilecansi linkc



execName = exe

args = 3169 data.dat log.dat

extraFilesToClear = 

extraLibsToLink = -pthread


allc: compilec linkc run

allcansi: compilecansi linkc run

allcpp: compilecpp linkcpp run




compilec:
	gcc -c *.c

compilecansi:
	gcc -c *.c -ansi -Wall -pedantic -error

compilecpp:
	g++ -c *.cpp




linkc:
	gcc -o $(execName) *.o $(extraLibsToLink)

linkcpp:
	g++ -o $(execName) *.o $(extraLibsToLink)




run:
	./$(execName) $(args)

runVal:
	valgrind ./$(execName) $(args)



clean: clear
	rm -f *.o $(execName) $(extraFilesToClear)

clear:
	clear

