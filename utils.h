#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"

static void shuffle_array(char* array, size_t n);
static void init_aliens_array(char* aliens_array, int dim);
static void update_score_board(WINDOW* score_board_win, Player* players,  int n_players);

void init_grid(char grid[GRID_SIZE][GRID_SIZE], char aliens_array[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE]);
void init_score_board(Player* players, int n_players);

#endif