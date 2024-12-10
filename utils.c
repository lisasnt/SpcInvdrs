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
        aliens_array[i] = (i < (dim)/3) ? ALIEN_SYMBOL : ' ';
    }
    shuffle_array(aliens_array, dim);
}

void init_ncurses() {
    initscr();		    	/* Start curses mode 		*/
    cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
    noecho();			    /* Don't echo() while we do getch */
}

/*
* Initialize and draw the battle field with 1/3 of the outer space having aliens in random positions
*/
void init_grid(char grid[GRID_SIZE][GRID_SIZE], char* aliens_array) {
    init_ncurses();

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

    // save aliens positions in the grid
    for (int i = 0; i < OUTER_SPACE_SIZE; i++) {
        for (int j = 0; j < OUTER_SPACE_SIZE; j++) {
            grid[i+OFFSET][j+OFFSET] = aliens_array[j*OUTER_SPACE_SIZE+i];
        }
    }
}

void refresh_grid(char grid[GRID_SIZE][GRID_SIZE]) {
    WINDOW* game_border_win = newwin(GRID_SIZE+2, GRID_SIZE+2, 1, 1);
    box(game_border_win, 0 , 0);
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            mvwprintw(game_border_win, i+1, j+1, "%c", grid[j][i]);
        }
    }	
    wrefresh(game_border_win);
}

void get_score_board(Player* players, int n_players) {
    WINDOW* score_board_win = newwin(MAX_PLAYERS+2, GRID_SIZE+2, 1, GRID_SIZE+6);
    box(score_board_win, 0 , 0);	
    mvwprintw(score_board_win, 0, 1, "Score Board");
    for (int i = 0; i < n_players; i++) {
        mvwprintw(score_board_win, i+1, 1, "Player %c: %d", players[i].id, players[i].score);
    }
    wrefresh(score_board_win);
}

void init_debug_window() {
    WINDOW* debug_win = newwin(10, GRID_SIZE+2, MAX_PLAYERS+3, GRID_SIZE+6);
    box(debug_win, 0 , 0);	
    mvwprintw(debug_win, 0, 1, "Debug Window");
    wrefresh(debug_win);
}

void update_debug_window(char* msg) {
    WINDOW* debug_win = newwin(5, GRID_SIZE, MAX_PLAYERS+4, GRID_SIZE+7);
    mvwprintw(debug_win, 1, 1, msg);
    wrefresh(debug_win);
}

/*
* Initialize the player controller: keyboard input and print its own score
*/
void init_player_controller() {
    init_ncurses();

    mvprintw(0,0,"Score: %d", 0);
}

/*
*   Add a new player to the grid
*/
void add_new_ply_to_grid(char grid[GRID_SIZE][GRID_SIZE], Player* new_player) {
    char id = new_player->id;
    // TODO randomize the position inside each area
    switch (id)
    {
        case 'A':
            new_player->x = 3; //in the visual grid it is (3,1) but in the grid[][] is [2][0] because refresh_grid() offset x,y by 1 (in 0 there is the border line)
            new_player->y = 1;
            break;
        case 'B':
            new_player->x = 2;
            new_player->y = 1;
            break;
        case 'C':
            new_player->x = 18;
            new_player->y = 4;
            break;
        case 'D':
            new_player->x = 19;
            new_player->y = 5;
            break;
        case 'E':
            new_player->x = 16;
            new_player->y = 18;
            break;
        case 'F':
            new_player->x = 17;
            new_player->y = 19;
            break;
        case 'G':
            new_player->x = 0;
            new_player->y = 2;
            break;
        case 'H':
            new_player->x = 1;
            new_player->y = 3;
            break;
        default:
            break;
    }
    grid[(new_player->x)-1][(new_player->y)-1] = id;
    refresh_grid(grid);
}

void move_player(char grid[GRID_SIZE][GRID_SIZE], Player* player, action_t direction){
    switch (direction)
    {
    case LEFT:
        (player->x) --;
        if (player->x == 2)
            player->x = GRID_SIZE-OFFSET;
        break;
    case RIGHT:
        (player->x) ++;
        if(player->x == GRID_SIZE-OFFSET-1)
            player->x = 3;
        break;
    case UP:
        (player->y) --;
        if(player->y == 2)
            player->y =  GRID_SIZE-OFFSET;
        break;
    case DOWN:
        (player->y) ++;
        if(player->y == GRID_SIZE-OFFSET-1)
            player->y = 3;
        break;
    default:
        break;
    }
    grid[(player->x)-1][(player->y)-1] = player->id;
    refresh_grid(grid);
}

int get_id(char* buffer, const char* player_id_chars) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (player_id_chars[i] == buffer[0]) {
            return i;
        }
    }
    return -1;
}