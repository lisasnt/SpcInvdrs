#include <zmq.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#define GRID_SIZE 20
#define OFFSET  2
#define OUTER_SPACE_SIZE (GRID_SIZE-2*OFFSET)
#define MAX_PLAYERS 8
#define ALIEN_SYMBOL '*'
#define LASER_SYMBOL '-'
#define MSG_SIZE 32

// Define message types
#define CONNECT     "Astronaut_connect"
#define MOVE        "Astronaut_movement"
#define ZAP         "Astronaut_zap"
#define DISCONNECT  "Astronaut_disconnect"
#define UPDATE      "Outer_space_update"
//#define ACK         "Message_received"

//TODO In order to simplify the execution of the various applications the addresses (IP address
//and port) of all the sockets in the project should be defined in a .h file 
#define SERVER_ADDRESS "tcp://*:5555"
#define CLIENT_ADDRESS "tcp://localhost:5555"

typedef enum {UP, DOWN, LEFT, RIGHT, LASER} action_t;

extern const char player_id_chars[MAX_PLAYERS];

typedef struct {
    char id;            // Unique identifier (e.g., 'A', 'B', ...)
    int x, y;           // Current position
    int score;          // Current score
    int stunned;        // Stun counter (0 if not stunned)
    int cooldown;       // Laser cooldown counter
} Player;

typedef struct {
    int x, y;           // Alien position
    int active;         // 1 if alive, 0 if destroyed
} Alien;

typedef struct {
    int msg_type;
    //char ch;
    action_t action;
} Remote_cmd_t;
