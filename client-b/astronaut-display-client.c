#include <pthread.h>
#include "../utils.h"

typedef struct {
    void *context;
    void *subscriber;
    void *requester;
    char (*grid)[GRID_SIZE];
    Player *players;
    int *n_players;
    char player_id;
    volatile int terminate; 
} thread_args_t;

void* get_player_action(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    void *context = args->context;
    void *requester = args->requester;
    char (*grid)[GRID_SIZE] = args->grid;
    Player *players = args->players;
    int *n_players = args->n_players;
    char player_id = args->player_id;
    volatile int *terminate = &args->terminate;

    char buffer[MSG_SIZE];

    Remote_cmd_t command;
    command.msg_type = "Not valid";
    command.action = -1;
    int key = -1; 
    
    while(*terminate == 0) {
        while (strcmp(command.msg_type, DISCONNECT) != 0) {
            
            while (key == -1) {
                key = getch();
            }
            switch (key) {
                case KEY_LEFT:
                    command.msg_type = MOVE;
                    command.action = LEFT;                
                    break;
                case KEY_RIGHT:
                    command.msg_type = MOVE;
                    command.action = RIGHT;
                    break;
                case KEY_DOWN:
                    command.msg_type = MOVE;
                    command.action = DOWN;
                    break;
                case KEY_UP:
                    command.msg_type = MOVE;
                    command.action = UP;
                    break;
                case ' ':
                    command.msg_type = ZAP;
                    command.action = LASER;
                    break;
                case 'q':
                //case 'Q':
                    command.msg_type = DISCONNECT;
                    command.action = -1;
                    *terminate = 1;
                    break;
                default:
                    command.msg_type = "Not valid";
                    // mvprintw(13, 26, "Not valid, try again");
                    // refresh(); 
                    // mvprintw(13, 26, "                     ");
                    refresh();
                    command.action = -1;
                    break;
            }
            key = -1;
            if (command.action != -1) {
                sprintf(buffer, "%s", command.msg_type); // message type: MOVE or ZAP
                if (strcmp(buffer, MOVE) == 0) {
                    s_sendmore(requester, buffer);  // sends the message MOVE
                    memset(buffer, 0, MSG_SIZE);
                    sprintf(buffer, "%d", command.action);
                    s_sendmore(requester, buffer);  // sends the direction of the movement
                } else {
                    s_sendmore(requester, buffer);      // sends the message ZAP
                }
                memset(buffer, 0, MSG_SIZE);
                sprintf(buffer, "%c", player_id); 
                s_send(requester, buffer);          // sends the astronaut id
                s_recv(requester); // not used (astronaut score) 
            }
        }
        s_sendmore(requester, DISCONNECT);
        mvprintw(4, 0, "You decide to disconnected :(");
        refresh();
        memset(buffer, 0, MSG_SIZE);
        sprintf(buffer, "%c", player_id);
        s_send(requester, buffer);
        zmq_close (requester);
    }
    return 0;
}

void* display_game(void* arg) {
    thread_args_t* args = (thread_args_t*)arg;
    void *subscriber = args->subscriber;
    char (*grid)[GRID_SIZE] = args->grid;
    Player *players = args->players;
    int *n_players = args->n_players;
    volatile int *terminate = &args->terminate;

    while (*terminate == 0) {
        char *buffer = s_recv(subscriber); 
        while (strcmp(buffer, UPDATE_DISPLAY) != 0) {
            buffer = s_recv(subscriber);
        }
        s_send(subscriber, ACK);
        receive_display(grid, players, n_players, subscriber);             
        free(buffer);
    }
    return 0;
}

int main() {
    Player players[MAX_PLAYERS]; 
    int n_players = 0;
    char grid[GRID_SIZE][GRID_SIZE];

    // Socket 
    void *context = zmq_ctx_new();
    assert(context != NULL);
    void *requester = zmq_socket (context, ZMQ_REQ);
    assert(requester != NULL);
    int rc = zmq_connect (requester, CHILD_ADDRESS);
    assert (rc == 0);
    void *subscriber = zmq_socket (context, ZMQ_SUB);
    assert(subscriber != NULL); 
    rc = zmq_connect (subscriber, DISPLAY_ADDRESS);
    assert (rc == 0);
    rc = zmq_setsockopt (subscriber, ZMQ_SUBSCRIBE, "", 0); //  Subscribe to all messages
    assert (rc == 0);

    init_ncurses();
    //mvwprintw(stdscr, 0, 0, "Outer Space Display"); // TODO display grid here
    //refresh();

    // Connect to the server (message Astronaut_connect) and receive the astronaut id
    s_send(requester, CONNECT);
    char buffer[MSG_SIZE];
    strcpy(buffer, s_recv(requester));
    if (strcmp(buffer, "SERVER IS FULL!") == 0) {
        mvprintw(0, 0, "Server is full, try later!");
        refresh();
        sleep(1);
        zmq_close (requester);
        zmq_ctx_destroy (context); 
        endwin(); 
    }
    char player_id = buffer[0];

    thread_args_t args = {context, subscriber, requester, grid, players, &n_players, player_id};

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, get_player_action, &args);
    pthread_create(&thread2, NULL, display_game, &args);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // zmq_close(requester);
    // zmq_close(subscriber);
    // zmq_ctx_destroy(context);
    // endwin();
    return 0;
}