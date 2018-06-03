leysö: clean compilecansi linkc


portno = 2529

serverName = homeworkServer
clientName = clientApp

serverFiles = server.o queue.o
clientFiles = client.o

argsServer = $(portno) data.dat log.dat
argsClient = hileci1 Q 45 127.0.0.1 $(portno)

serverLibs = -pthread
clientLibs = 

extraFilesToClear = log.dat




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
	gcc -o $(serverName) $(serverFiles) $(serverLibs)
	gcc -o $(clientName) $(clientFiles) $(clientLibs)

linkcpp:
	g++ -o $(execName) *.o $(extraLibsToLink)




runServer:
	./$(serverName) $(argsServer)

runValServer:
	valgrind ./$(serverName) $(argsServer)
	
runClient:
	./$(clientName) $(argsClient)

runValClient:
	valgrind ./$(clientName) $(argsClient)



clean: clear
	rm -f *.o $(clientName) $(serverName) $(extraFilesToClear)

clear:
	clear

