#include "utils.h"

void shuffle_array(char* array, size_t n) {
    srand(time(NULL)); 
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1); // Fisher-Yates shuffle algorithm
          char t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

void init_aliens_array(char* aliens_array, int dim) {    
    for (int i = 0; i < dim; i++) {
        aliens_array[i] = (i < (dim)/3) ? ALIEN : ' ';
    }
    shuffle_array(aliens_array, dim);
}

// Initialize and draw the battle field with 1/3 of the outer space having aliens in random positions
void init_grid(char grid[GRID_SIZE][GRID_SIZE], char aliens_array[OUTER_SPACE_SIZE*OUTER_SPACE_SIZE]) {
    // Initialize the ncurses window
    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    WINDOW* my_win = newwin(GRID_SIZE+3, GRID_SIZE+3, 0, 0);
    for (int i = 1; i < GRID_SIZE+1; i++) {
        mvwprintw(my_win, 0, i + 1, "%d", i % 10); // Print column numbers
    }    
    for (int i = 1; i < GRID_SIZE+1; i++) {
        mvwprintw(my_win, i + 1, 0, "%d", i % 10); // Print row numbers
    }
    wrefresh(my_win);
    WINDOW* game_border_win = newwin(GRID_SIZE+2, GRID_SIZE+2, 1, 1);
    box(game_border_win, 0 , 0);	
    wrefresh(game_border_win);

    init_aliens_array(aliens_array, OUTER_SPACE_SIZE*OUTER_SPACE_SIZE);

    WINDOW* outspc_win = newwin(OUTER_SPACE_SIZE, OUTER_SPACE_SIZE, 4, 4); 
    
    for (int i = 0; i < OUTER_SPACE_SIZE; i++) {
        for (int j = 0; j < OUTER_SPACE_SIZE; j++) {
            wmove(outspc_win, i, j);
            waddch(outspc_win, aliens_array[i*OUTER_SPACE_SIZE+j]);
        }
    }
    wrefresh(outspc_win);
    //endwin();   /* End curses mode */  ->>>>>>>> si pianta ?  
}

void new_position(int* x, int *y, direction_t direction){
    switch (direction)
    {
    case UP:
        (*x) --;
        if(*x ==0)
            *x = 2;
        break;
    case DOWN:
        (*x) ++;
        if(*x ==GRID_SIZE-1)
            *x = GRID_SIZE-3;
        break;
    case LEFT:
        (*y) --;
        if(*y ==0)
            *y = 2;
        break;
    case RIGHT:
        (*y) ++;
        if(*y ==GRID_SIZE-1)
            *y = GRID_SIZE-3;
        break;
    default:
        break;
    }
}