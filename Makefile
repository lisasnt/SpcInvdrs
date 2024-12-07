all: game-server astronaut-client outer-space-display

game-server: game-server.c
	gcc -lncurses -lczmq -lzmq utils.c game-server.c -o $@

astronaut-client: astronaut-client.c
	gcc -lncurses -lzmq -lczmq astronaut-client.c -o $@

outer-space-display: outer-space-display.c
	gcc -lncurses -lzmq -lczmq outer-space-display.c -o $@

clean:
	rm -f game-server astronaut-client outer-space-display
