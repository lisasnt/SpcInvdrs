/*
    Application that reads the keys pressed by the user,
    corresponding to the movement of one astronaut, sends them to the server, and
    receives the user score (how many aliens were zapped).
*/
#include "utils.h"

int main (void) {
    Player player;

    init_player_controller();
    sleep(1);

    // Socket 
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);
    zmq_connect (requester, CLIENT_ADDRESS);

    // Connect to the server (message Astronaut_connect) and receive the astronaut id
    s_send(requester, CONNECT);
    char buffer[MSG_SIZE];
    strcpy(buffer, s_recv(requester));
    if (strcmp(buffer, "SERVER IS FULL!") == 0) {
        mvprintw(0, 0, "Server is full!");
        refresh();
        sleep(1);
        zmq_close (requester);
        zmq_ctx_destroy (context); 
        endwin(); 
        return 0;
    }
    mvprintw(0, 0, "Received ID: %s", buffer);
    player.id = buffer[0];
    sleep(1);

    /* loop that:
        - reads a key press;
        - sends the respective Astronaut_movement or Astronaut_zap message to the server;
        - receives a reply with the astronaut score;
    */
    Remote_cmd_t command;
    int key = -1;
    while (key == -1) {
        key = getch();
    }
    while (key != 'q' && key != 'Q') {
        char message[MSG_SIZE];
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
            default:
                break;
        }
        s_sendmore(requester, command.msg_type);
        s_send(requester, command.action);
        char buffer[MSG_SIZE];
        strcpy(buffer, s_recv(requester));
        mvprintw(1, 0, "Received response: %s", buffer); 
        refresh(); 
        sleep(1);
    }
    

    zmq_close (requester);
    zmq_ctx_destroy (context); 

    // Disconnect from the server (message Astronaut_disconnect)
    //TODO If the user presses the q or Q keys, the client should terminate and send an Astronaut_disconnect message to the server.

    /*
    int n = 0;
    int key;

    do
    {
    	key = getch();		
        n++;
        switch (key)
        {
        case KEY_LEFT:
            mvprintw(0,0,"%d Left arrow is pressed", n);
            break;
        case KEY_RIGHT:
            mvprintw(0,0,"%d Right arrow is pressed", n);
            break;
        case KEY_DOWN:
            mvprintw(0,0,"%d Down arrow is pressed", n);
            break;
        case KEY_UP:
            mvprintw(0,0,"%d Up arrow is pressed", n);
            break;

        default:
        mvprintw(0,0,"%d: %c key was pressed", n, key);
            break;
        }
    	refresh(); 
    }while(key != 27);
    */
    
  	endwin();   /* End curses mode */

    return 0;
}