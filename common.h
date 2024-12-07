#include <stdio.h>

#define GRID_SIZE 20
#define OFFSET  2
#define OUTER_SPACE_SIZE (GRID_SIZE-2*OFFSET)
#define MAX_PLAYERS 8
#define ALIEN '*'
#define LASER '-'
#define MSG_SIZE 256

// Define message types
#define CONNECT     "Astronaut_connect"
#define MOVE        "Astronaut_movement"
#define ZAP         "Astronaut_zap"
#define DISCONNECT  "Astronaut_disconnect"
#define UPDATE      "Outer_space_update"
#define ACK         "Message_received"

//TODO In order to simplify the execution of the various applications the addresses (IP address
//and port) of all the sockets in the project should be defined in a .h file 
#define SERVER_ADDRESS "tcp://*:5555"
#define CLIENT_ADDRESS "tcp://localhost:5555"

typedef enum {A, B, C, D, E, F, G, H} player_id_t;
typedef enum {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct {
    player_id_t id;      // Unique identifier (e.g., 'A', 'B', ...)
    int x, y;            // Current position
    int score;           // Current score
    int stunned;         // Stun counter (0 if not stunned)
    int cooldown;        // Laser cooldown counter
} Player;

typedef struct {
    int x, y;            // Alien position
    int active;          // 1 if alive, 0 if destroyed
} Alien;

struct remote_char_t {
    int msg_type;
    char ch;
    direction_t direction;
};
