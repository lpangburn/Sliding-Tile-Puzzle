prog: slidingpuzzle-v3.o sp-pipe-client.o sp-pipe-server.o 
	gcc slidingpuzzle-v3.o sp-pipe-client.o sp-pipe-server.o  -o prog
slidingpuzzle-v3.o: slidingpuzzle-v3.c
	gcc -g -c slidingpuzzle-v3.c
sp-pipe-client.o: sp-pipe-client.c
	gcc -g -c sp-pipe-client.c
sp-pipe-server.o: sp-pipe-server.c
	gcc -g -c sp-pipe-server.c
clean:
	rm -f slidingpuzzle-v3.o sp-pipe-client.o sp-pipe-server.o prog