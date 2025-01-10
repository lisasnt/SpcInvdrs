/*
    Server application that receives messages from all the
    clients, handles outer space with all the aliens and astronauts, 
    implements the game rules, and sends updates to the outer-space-display.c. 
    This application also shows the outer space.
*/
#include "../utils.h"
#include <pthread.h>

// Define DEBUG to enable debug messages
//#define DEBUG 

char grid[GRID_SIZE][GRID_SIZE];
char aliens[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE];
Player players[MAX_PLAYERS]; // must be handled as a list
int n_players = 0;
const char player_id_chars[MAX_PLAYERS] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
void *context;
// int64_t last_kill_time = 0;
// int alien_count = 0;



typedef struct {
    void *publisher;
    void *responder;
    // pthread_mutex_t *data_mutex;
    // pthread_mutex_t *grid_mutex;
} thread_args_t;

void* read_commands_thread(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    void *publisher = args->publisher;
    void *responder = args->responder;

    char buffer[MSG_SIZE] = {0};
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
                pthread_mutex_lock(&data_mutex);
                Player new_player = {   player_id_chars[n_players], 
                                        0,  
                                        0, 
                                        0, 0, 0};
                add_player(grid, &new_player);  
                players[n_players++] = new_player;
                pthread_mutex_unlock(&data_mutex);
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
            pthread_mutex_lock(&data_mutex);
            remove_player(grid, &players[tmp_id], players, &n_players); 
            pthread_mutex_unlock(&data_mutex);
            get_score_board(players, n_players);
            send_display(grid, players, n_players, publisher);
        } else {
            #ifdef DEBUG
            update_debug_window("Received unknown message\n");
            #endif
        }
        free(buffer2);
    }
    return 0;
}

void* move_aliens_thread(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;

    int64_t start_time = s_clock();
    while(1) {
        int64_t current_time = s_clock();
        if (current_time - start_time >= 1000) {
            // Move aliens in the grid
            pthread_mutex_lock(&grid_mutex);
            move_aliens(grid);
            pthread_mutex_unlock(&grid_mutex);
            // if (current_time - last_kill_time >= 10000) {
            //     pthread_mutex_lock(&grid_mutex);
            //     increase_alien_population(grid);
            //     pthread_mutex_unlock(&grid_mutex);
            //     last_kill_time = current_time; 
            // }
            start_time = current_time;
        }       
    }
    return 0;
}

int main () {


    char buffer[MSG_SIZE] = {0};
    // Socket
    context = zmq_ctx_new ();
    assert(context != NULL);
    void *responder = zmq_socket (context, ZMQ_REP); 
    assert(responder != NULL);
    void *publisher = zmq_socket(context, ZMQ_PUB); 
    assert(publisher != NULL);
    int rc = zmq_bind (responder, SERVER_ADDRESS_REP);
    assert (rc == 0);
    rc = zmq_bind(publisher, SERVER_ADDRESS_PUB);
    assert (rc == 0);

    pthread_mutex_init(&grid_mutex, NULL);
    pthread_mutex_init(&data_mutex, NULL);
    thread_args_t args = {publisher, responder};
    pthread_t thread1, thread2;

    init_grid(grid, aliens);
    get_score_board(players, n_players);
    #ifdef DEBUG
    init_debug_window();
    #endif

    pthread_create(&thread1, NULL, read_commands_thread, &args);
    pthread_create(&thread2, NULL, move_aliens_thread, &args);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    zmq_close (responder);
    zmq_close (publisher);
    zmq_ctx_shutdown (context);
    endwin();   /* End curses mode */
    return 0;
}

//TODO 
// if some user presses Q server and clients should terminates orderly -> you can inform the thread of the socket that it must terminate wiht zmq_send() #define TERMINATE   "Terminate_client"
// move aliens every sencond
// protocol buffer with the highest score
// increase number of aliens +10% when no action in 10 sec is taken
// report
// show laser in every client


// void *frontend = zmq_socket(context, ZMQ_ROUTER); // for astronaut-display-client -> player commands
    // assert(frontend != NULL);
    // rc = zmq_bind(frontend, "tcp://*:5554");
    // assert (rc == 0);
    // void *backend = zmq_socket(context, ZMQ_DEALER); // for astronaut-display-client -> player commands
    // assert(backend != NULL);
    // rc = zmq_bind(backend, "tcp://*:5553");
    // assert (rc == 0);
    // void *publisher2 = zmq_socket(context, ZMQ_XPUB);// for astronaut-display-client -> display updates
    // assert(publisher2 != NULL); 
    // rc = zmq_bind(publisher2, "tcp://*:5552");

        // zmq_proxy(frontend, backend, NULL);
