#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <zmq.h>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "zhelpers.h"
#include <pthread.h>


#define GRID_SIZE 20
#define OFFSET  2
#define OUTER_SPACE_SIZE (GRID_SIZE-2*OFFSET)
#define MAX_PLAYERS 8
#define ALIEN_SYMBOL '*'
#define LASER_SYMBOL '-'
#define LASER_SYMBOL_VERTICAL '|'
#define MSG_SIZE 256
#define CONNECT     "Astronaut_connect"
#define MOVE        "Astronaut_movement"
#define ZAP         "Astronaut_zap"
#define DISCONNECT  "Astronaut_disconnect"
#define UPDATE      "Outer_space_update"
#define TERMINATE   "Terminate_client"
#define ACK         "Message_received"
#define UPDATE_DISPLAY      "UPDATE_DISPLAY"
#define SERVER_ADDRESS_REP  "tcp://*:5555"
#define SERVER_ADDRESS_PUB  "tcp://*:5556"
#define CHILD_ADDRESS       "tcp://localhost:5555"
#define DISPLAY_ADDRESS     "tcp://localhost:5556"
#define random_from_i_to_n(i, n) (i + rand() % (n - i + 1));

typedef enum {UP, DOWN, LEFT, RIGHT, LASER} action_t;

extern const char player_id_chars[MAX_PLAYERS];
extern pthread_mutex_t grid_mutex;
extern pthread_mutex_t data_mutex;

typedef struct {
    char id;                // Unique identifier (e.g., 'A', 'B', ...)
    int x, y;               // Current position
    int score;              // Current score
    bool stunned;           // Stun flag (0 if not stunned)
    bool cooldown;          // Laser flag (0 if not in cooldown)
    int64_t stun_start;     // When the stun started
    int64_t cooldown_start; // When the cooldown started
} Player;

typedef struct {
    int x, y;           // Alien position
    int active;         // 1 if alive, 0 if destroyed
} Alien;

typedef struct {
    char* msg_type;     // MOVE, ZAP or DISCONNECT
    action_t action;
} Remote_cmd_t;

static void shuffle_array(char* array, size_t n);
static void init_aliens_array(char* aliens_array, int dim, int n_aliens);
static void send_grid(char grid[GRID_SIZE][GRID_SIZE], void *publisher);
static void send_score_board(Player* players, int n_players, void *publisher);
static void receive_grid(char grid[GRID_SIZE][GRID_SIZE], void *subscriber);
static void receive_score_board(Player* players, int* n_players, void *subscriber);
static int is_stunned(Player* player);
static int is_cooldown(Player* player);

void init_grid(char grid[GRID_SIZE][GRID_SIZE], char* aliens_array);
void get_score_board(Player* players, int n_players);
void init_player_controller();
void init_ncurses();
void add_player(char grid[GRID_SIZE][GRID_SIZE], Player* new_player);
void refresh_grid(char grid[GRID_SIZE][GRID_SIZE]);
void move_player(char grid[GRID_SIZE][GRID_SIZE], Player* player, action_t direction);
int get_id(char* buffer, const char* player_id_chars);
void remove_player(char grid[GRID_SIZE][GRID_SIZE], Player* player, Player players[MAX_PLAYERS], int* n_players);
void laser_opponents(char grid[GRID_SIZE][GRID_SIZE], Player* player, Player players[MAX_PLAYERS], int n_players);
void send_display(char grid[GRID_SIZE][GRID_SIZE], Player* players, int n_players, void *publisher);
void receive_display(char grid[GRID_SIZE][GRID_SIZE], Player* players, int* n_players, void *subscriber);
void get_stunned_time(Player* player);
void move_aliens(char grid[GRID_SIZE][GRID_SIZE]);
// Debug functions:
void init_debug_window();
void update_debug_window(char* msg);

#endif