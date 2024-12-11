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
#define LASER_SYMBOL_VERTICAL '|'
#define MSG_SIZE 256

// Define message types
#define CONNECT     "Astronaut_connect"
#define MOVE        "Astronaut_movement"
#define ZAP         "Astronaut_zap"
#define DISCONNECT  "Astronaut_disconnect"
#define UPDATE      "Outer_space_update"
#define ACK         "Message_received"
#define UPDATE_DISPLAY "UPDATE_DISPLAY"
#define SERVER_ADDRESS_REP "tcp://*:5555"
#define SERVER_ADDRESS_PUB "tcp://*:5556"
#define CHILD_ADDRESS "tcp://localhost:5555"
#define DISPLAY_ADDRESS "tcp://localhost:5556"

typedef enum {UP, DOWN, LEFT, RIGHT, LASER} action_t;

extern const char player_id_chars[MAX_PLAYERS];

typedef struct {
    char id;            // Unique identifier (e.g., 'A', 'B', ...)
    int x, y;           // Current position
    int score;          // Current score
    bool stunned;        // Stun flag (0 if not stunned)
    bool cooldown;       // Laser flag (0 if not in cooldown)
    int64_t stun_start; // When the stun started
    int64_t cooldown_start; // When the cooldown started
} Player;

typedef struct {
    int x, y;           // Alien position
    int active;         // 1 if alive, 0 if destroyed
} Alien;

typedef struct {
    char* msg_type;     // MOVE, ZAP or DISCONNECT
    //int key;
    action_t action;
} Remote_cmd_t;
