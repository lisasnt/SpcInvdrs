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
    mvprintw(0, 0, "Your ID: %c", buffer[0]);
    mvprintw(1, 0, "Press Arrows to MOVE in your direction, Space to ZAP, q-Q to quit");
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
            case 'Q':
                command.msg_type = DISCONNECT;
                command.action = -1;
                break;
            default:
                command.msg_type = "Not valid";
                mvprintw(2, 0, "Not valid, try again");
                refresh(); 
                command.action = -1;
                break;
        }
        key = -1;
        if (command.action != -1) {
            sprintf(buffer, "%s", command.msg_type); // sends the message type MOVE or ZAP
            refresh();
            if (strcmp(buffer, MOVE) == 0) {
                sprintf(buffer, "%d", command.action);
                s_send(requester, buffer);  // sends the direction of the movement
            }
            memset(buffer, 0, MSG_SIZE);
            sprintf(buffer, "%s", player.id); 
            s_send(requester, buffer);          // sends the astronaut id
            memset(buffer, 0, MSG_SIZE);
            strcpy(buffer, s_recv(requester));  // receives the astronaut score
            mvprintw(4, 0, "Your score: %s", buffer);
            sleep(1);
        }
    }

    // Disconnect from the server (message Astronaut_disconnect)
    s_send(requester, DISCONNECT);
    mvprintw(4, 0, "You decide to disconnected :(");
    refresh();
    sleep(1);
    zmq_close (requester);
    zmq_ctx_destroy (context); 
  	endwin();   /* End curses mode */
    return 0;
}