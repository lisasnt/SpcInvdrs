#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"

void shuffle_array(char* array, size_t n);
void init_aliens_array(char* aliens_array, int dim);
void init_grid(char grid[GRID_SIZE][GRID_SIZE], char aliens_array[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE]);
void new_position(int* x, int *y, direction_t direction);

#endif