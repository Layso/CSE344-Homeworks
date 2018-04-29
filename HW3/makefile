leysö: clean compilecansi linkc



execName = shell

args = 

extraFilesToClear = $(lsExecName) $(catExecName) $(wcExecName)


lsExecName = ls

catExecName = cat

wcExecName = wc



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
	gcc -o $(execName) 151044001_HW3_CSE344.o
	gcc -o ls ls.o
	gcc -o cat cat.o
	gcc -o wc wc.o

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

