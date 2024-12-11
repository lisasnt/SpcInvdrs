all: game-server astronaut-client outer-space-display

game-server: utils.c game-server.c
	gcc -g -lncurses -lzmq utils.c game-server.c -o $@

astronaut-client: utils.c astronaut-client.c
	gcc -g -lncurses -lzmq utils.c astronaut-client.c -o $@

outer-space-display: utils.c outer-space-display.c
	gcc -g -lncurses -lzmq utils.c outer-space-display.c -o $@

clean:
	rm -f game-server astronaut-client outer-space-display
