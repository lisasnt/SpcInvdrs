/*
    Server application that receives messages from all the
    clients, handles outer space with all the aliens and astronauts, 
    implements the game rules, and sends updates to the outer-space-display.c. 
    This application also shows the outer space.
*/
#include "utils.h"

// Define DEBUG to enable debug messages
//#define DEBUG 

char grid[GRID_SIZE][GRID_SIZE];
char aliens[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE];
Player players[MAX_PLAYERS]; // must be handled as a list
int n_players = 0;

const char player_id_chars[MAX_PLAYERS] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

int main () {
    char buffer[MSG_SIZE] = {0};
    // Socket
    void *context = zmq_ctx_new ();
    assert(context != NULL);
    void *responder = zmq_socket (context, ZMQ_REP);
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind (responder, SERVER_ADDRESS_REP);
    assert (rc == 0);
    rc = zmq_bind(publisher, SERVER_ADDRESS_PUB);
    assert (rc == 0);

    init_grid(grid, aliens);
    get_score_board(players, n_players);
    #ifdef DEBUG
    init_debug_window();
    #endif

    while (1) {
        s_send(publisher, UPDATE_DISPLAY);
        char* buffer2 = s_recv(publisher); // receive ACK
        send_display(grid, players, n_players, publisher);

        strcpy(buffer, s_recv(responder));
        if (strcmp(buffer, CONNECT) == 0) {
            if (n_players < MAX_PLAYERS) {
                #ifdef DEBUG 
                    update_debug_window("Received Astronaut_connect\n"); 
                #endif
                Player new_player = {   player_id_chars[n_players], 
                                        0,  
                                        0, 
                                        0, 0, 0};
                add_player(grid, &new_player);  
                players[n_players++] = new_player;
                get_score_board(players, n_players);
                send_display(grid, players, n_players, publisher);    
                s_send(responder, &new_player.id);
            } else {
                #ifdef DEBUG 
                update_debug_window("Received Astronaut_connect: SERVER IS FULL!\n");
                #endif
                s_send(responder, "SERVER IS FULL! Try again later.");
            }
        } else if (strcmp(buffer, MOVE) == 0) {
            #ifdef DEBUG
                update_debug_window("Received Astronaut_movement\n");
            #endif
            strcpy(buffer, s_recv(responder));  // receives the direction of the movement
            action_t tmp_direction = (int)(buffer[0]-'0');
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            move_player(grid, &players[tmp_id], tmp_direction);
            get_score_board(players, n_players);
            send_display(grid, players, n_players, publisher);
            sprintf(buffer, "%d", players[tmp_id].score);
            s_send(responder, buffer); // sends the astronaut score
        } else if (strcmp(buffer, ZAP) == 0) {
            #ifdef DEBUG
                update_debug_window("Received Astronaut_zap\n");
            #endif
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            laser_opponents(grid, &players[tmp_id], players, n_players);
            sprintf(buffer, "%d", players[tmp_id].score);
            s_send(responder, buffer); // sends the astronaut score
            get_score_board(players, n_players);
            send_display(grid, players, n_players, publisher);
        } else if (strcmp(buffer, DISCONNECT) == 0) {
            #ifdef DEBUG
                update_debug_window("Received Astronaut_disconnect\n");
            #endif
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            s_send(responder, ACK);
            remove_player(grid, &players[tmp_id], players, &n_players); 
            get_score_board(players, n_players);
            send_display(grid, players, n_players, publisher);
        } else {
            #ifdef DEBUG
            update_debug_window("Received unknown message\n");
            #endif
        }
        sleep (1); 
        free(buffer2);
    }
    zmq_close (responder);
    zmq_close (publisher);
    zmq_ctx_shutdown (context);
    endwin();   /* End curses mode */
    return 0;
}
