all: game-server astronaut-client outer-space-display

game-server: utils.c game-server.c
	gcc -lncurses -lczmq -lzmq utils.c game-server.c -o $@

astronaut-client: utils.c astronaut-client.c
	gcc -lncurses -lzmq -lczmq utils.c astronaut-client.c -o $@

outer-space-display: utils.c outer-space-display.c
	gcc -lncurses -lzmq -lczmq utils.c outer-space-display.c -o $@

clean:
	rm -f game-server astronaut-client outer-space-display
