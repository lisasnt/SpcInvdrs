/*
    Server application that receives messages from all the
    clients, handles outer space with all the aliens and astronauts, 
    implements the game rules, and sends updates to the outer-space-display.c. 
    This application also shows the outer space.
*/
#include "utils.h"

char grid[GRID_SIZE][GRID_SIZE];
char aliens[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE]; // TODO: or is it better int aliens[] with 1 and 0 ?????
Player players[MAX_PLAYERS];
int n_players = 0;

const char player_id_chars[MAX_PLAYERS] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

int main () {
    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, SERVER_ADDRESS);
    assert (rc == 0);
    
    init_grid(grid, aliens);
    get_score_board(players, n_players);
    init_debug_window();

    while (1) {
        //Socket
        char buffer[MSG_SIZE];
        strcpy(buffer, s_recv(responder));
        if (strcmp(buffer, CONNECT) == 0) {
            if (n_players < MAX_PLAYERS) {
                update_debug_window("Received Astronaut_connect\n");
                Player new_player = {   player_id_chars[n_players], 
                                        0,  
                                        0, 
                                        0, 0, 0};
                add_player(grid, &new_player);  
                players[n_players++] = new_player;
                get_score_board(players, n_players);
                s_send(responder, &new_player.id);
            } else {
                update_debug_window("Received Astronaut_connect: SERVER IS FULL!\n");
                sleep(1);
                s_send(responder, "SERVER IS FULL!");
            }
        } else if (strcmp(buffer, MOVE) == 0) {
            update_debug_window("Received Astronaut_movement\n");
            strcpy(buffer, s_recv(responder));  // receives the direction of the movement
            action_t tmp_direction = (int)(buffer[0]-'0');
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            move_player(grid, &players[tmp_id], tmp_direction);
            get_score_board(players, n_players);
            sprintf(buffer, "%d", players[tmp_id].score);
            s_send(responder, buffer); // sends the astronaut score
        } else if (strcmp(buffer, ZAP) == 0) {
            update_debug_window("Received Astronaut_zap\n");
            memset(buffer, 0, sizeof(buffer));
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            //TODO destroy aliens, freeze other astronauts, cooldown, update score
            laser_opponents(grid, &players[tmp_id], players, n_players);
            sprintf(buffer, "%d", players[tmp_id].score);
            s_send(responder, buffer); // sends the astronaut score
            get_score_board(players, n_players);
        } else if (strcmp(buffer, DISCONNECT) == 0) {
            update_debug_window("Received Astronaut_disconnect\n");
            //s_send(responder, ACK);
            strcpy(buffer, s_recv(responder)); // receives the astronaut id
            int tmp_id = get_id(buffer, player_id_chars);
            s_send(responder, ACK);
            remove_player(grid, &players[tmp_id]);
            n_players--; // next will override the last disconnected player
            // TODO fix, if one in the middle disocnneted, the last one will be overriden -> use a list!!!
            get_score_board(players, n_players);
            
        } else {
            update_debug_window("Received unknown message\n");
        }
        sleep (1); 
    }
    endwin();   /* End curses mode */
    return 0;
}


//TODO Declare an array of structures that will contain the information about the characters of the various clients. assign letter when the astronaut-client.c first connects.

//TODO Add one new structure to the array whenever the server receives a message of type connection
/*Whenever the server receives a message of type movement, the server should search for the
corresponding entry on the array, calculate the new the position of such character and update
the screen */
//TODO Whenever the server receives a message of type disconnection, the server should remove the corresponding entry from the array.

// TODOs 
// The aliens move randomly in outer space at a rate of one place per second
// If an astronaut-client.c sends no message, it will not get updates on the other astronaut’s scores.
// Students should decide what happens to a client that sends an Astronaut_connect message when 8 clients are already connected.
// DONE At the beginning of the game, 1/3 of the outer space should have aliens.
// Astronaut inserterd/removed from a list of astronauts
// display ncurses field of game (with astronauts, aliens, and laser rays) and the scores of all the astronauts. (same as outer-space-display.c)
// When an astronaut fires the laser, its ray should be drawn on the screen for 0.5 seconds. 
// When astronaut sends a command he must receive back its own score.
// Error treatment / Cheating prevention
// Zapping -> Aliens are destroyed, and a point is added to the astronaut.
//         -> Other astronauts are stunned and become unmovable for 10 seconds.
//         -> Astronauts can only fire the laser at a rate of one fire every 3 seconds.
// asser results of zmq functions:
                                    /*void *context = zmq_ctx_new ();
                                    assert (rc != NULL);
                                    void *responder = zmq_socket (context, ZMQ_REP);
                                    assert (responder != NULL);
                                    int rc = zmq_bind (responder, "tcp://*:5555");
                                    assert (rc == 0); */

// DONOT USE threds, select, non-blocking communication, active wait, signals