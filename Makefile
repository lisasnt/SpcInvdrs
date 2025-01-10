all: server/game-server client/astronaut-client client/outer-space-display client-b/astronaut-display-client

server/game-server: utils.c server/game-server.c
	gcc -g utils.c -pthread server/game-server.c -o $@ -lncurses -lzmq 

client/astronaut-client: utils.c client/astronaut-client.c
	gcc -g utils.c client/astronaut-client.c -o $@ -lncurses -lzmq 

client/outer-space-display: utils.c client/outer-space-display.c
	gcc -g utils.c client/outer-space-display.c -o $@ -lncurses -lzmq 

client-b/astronaut-display-client: utils.c client-b/astronaut-display-client.c 
	gcc -g -pthread utils.c client-b/astronaut-display-client.c -o $@ -lncurses -lzmq

clean:
	rm -f server/game-server client/astronaut-client client/outer-space-display client-b/astronaut-display-client
