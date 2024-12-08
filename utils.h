#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "zhelpers.h"

static void shuffle_array(char* array, size_t n);
static void init_aliens_array(char* aliens_array, int dim);

void init_grid(char grid[GRID_SIZE][GRID_SIZE], char aliens_array[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE]);
void get_score_board(Player* players, int n_players);
void init_player_controller();
void init_ncurses();

// debug
void init_debug_window();
void update_debug_window(char* msg);

#endif