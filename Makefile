all: server/game-server client/astronaut-client client/outer-space-display

server/game-server: utils.c server/game-server.c
	gcc -g utils.c server/game-server.c -o $@ -lncurses -lzmq 

client/astronaut-client: utils.c client/astronaut-client.c
	gcc -g utils.c client/astronaut-client.c -o $@ -lncurses -lzmq 

client/outer-space-display: utils.c client/outer-space-display.c
	gcc -g utils.c client/outer-space-display.c -o $@ -lncurses -lzmq 

clean:
	rm -f server/game-server client/astronaut-client client/outer-space-display
