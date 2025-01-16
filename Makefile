INCLUDE_PATH = /usr/include/protobuf-c
CURRENT_DIR = .

all: server/game-server client/astronaut-client client/outer-space-display client-b/astronaut-display-client

server/game-server: utils.c server/game-server.c show_score_board.pb-c.h
	gcc -g -I$(INCLUDE_PATH) -I$(CURRENT_DIR) -pthread utils.c server/game-server.c  show_score_board.pb-c.c -o $@ -lncurses -lzmq -lprotobuf-c

client/astronaut-client: utils.c client/astronaut-client.c
	gcc -g utils.c client/astronaut-client.c -o $@ -lncurses -lzmq 

client/outer-space-display: utils.c client/outer-space-display.c
	gcc -g utils.c client/outer-space-display.c -o $@ -lncurses -lzmq 

client-b/astronaut-display-client: utils.c client-b/astronaut-display-client.c 
	gcc -g -pthread utils.c client-b/astronaut-display-client.c -o $@ -lncurses -lzmq

clean:
	rm -f server/game-server client/astronaut-client client/outer-space-display client-b/astronaut-display-client
                                                            