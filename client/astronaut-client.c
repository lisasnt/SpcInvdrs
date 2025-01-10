/*
    Application that reads the keys pressed by the user,
    corresponding to the movement of one astronaut, sends them to the server, and
    receives the user score (how many aliens were zapped).
*/
#include "../utils.h"

int main (void) {
    Player player;

    init_player_controller();

    // Socket 
    void *context = zmq_ctx_new ();
    assert(context != NULL);
    void *requester = zmq_socket (context, ZMQ_REQ);
    assert(requester != NULL);
    int rc = zmq_connect (requester, CHILD_ADDRESS);
    assert (rc == 0);

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
        return 0;
    }
    mvprintw(0, 0, "Your ID: %c", buffer[0]);
    mvprintw(1, 0, "Press Arrows to MOVE in your direction, Space to ZAP, 'q' to quit your game, 'Q' to quit the server");
    mvprintw(4, 0, "Your score: %d", 0);
    refresh();
    player.id = buffer[0];

    /* loop that:
        - reads a key press;
        - sends the respective Astronaut_movement or Astronaut_zap message to the server;
        - receives a reply with the astronaut score;
    */
    Remote_cmd_t command;
    command.msg_type = "Not valid";
    command.action = -1;
    int key = -1; 
    
    while (strcmp(command.msg_type, DISCONNECT) != 0) {
        mvprintw(3, 0, "");
        refresh();
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
                break;
            default:
                command.msg_type = "Not valid";
                mvprintw(3, 0, "Not valid, try again");
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
            sprintf(buffer, "%c", player.id); 
            s_send(requester, buffer);          // sends the astronaut id
            memset(buffer, 0, MSG_SIZE);
            strcpy(buffer, s_recv(requester));  // receives the astronaut score
            mvprintw(4, 0, "Your score: %s", buffer);
        }
    }

    // Disconnect from the server (message Astronaut_disconnect)
    s_sendmore(requester, DISCONNECT);
    mvprintw(4, 0, "You decide to disconnected :(");
    refresh();
    memset(buffer, 0, MSG_SIZE);
    sprintf(buffer, "%c", player.id);
    s_send(requester, buffer);
    zmq_close (requester);
    zmq_ctx_destroy (context); 
  	endwin();   /* End curses mode */
    return 0;
}

// TODO disconnect the player also when the window is closed Ctrl+C
// TODO add a timeout for the server response 
// and also for all the other actions

