/*
    Application that shows the outer space with all the
    participants in the same way as the game-server.c. 
    Multiple outer-space-display.c can run simultaneously.
*/
#include "utils.h"

int main (void) {
    Player players[MAX_PLAYERS]; 
    int n_players = 0;
    char grid[GRID_SIZE][GRID_SIZE];

    // Socket 
    void *context = zmq_ctx_new ();
    assert(context != NULL);
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    assert(subscriber != NULL); 
    int rc = zmq_connect (subscriber, DISPLAY_ADDRESS);
    assert (rc == 0);
    rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0); //  Subscribe to all messages
    assert (rc == 0);

    init_ncurses();
    mvwprintw(stdscr, 0, 0, "Outer Space Display");
    refresh();
    
    while(1) {
        char *buffer = s_recv(subscriber); 
        while (strcmp(buffer, UPDATE_DISPLAY) != 0) {
           buffer = s_recv(subscriber);
        }
        s_send(subscriber, ACK);
        receive_display(grid, players, &n_players, subscriber);             
        free(buffer);
    }
    // TODO Handle this -> I never get here, but clean up anyhow
    zmq_close (subscriber);
    zmq_ctx_destroy (context);
    endwin();   /* End curses mode */
    return 0;
}