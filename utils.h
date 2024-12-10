#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "zhelpers.h"

#define random_from_i_to_n(i, n) (i + rand() % (n - i + 1));

static void shuffle_array(char* array, size_t n);
static void init_aliens_array(char* aliens_array, int dim);

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

// debug
void init_debug_window();
void update_debug_window(char* msg);

#endif