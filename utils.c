#include "utils.h"

static void shuffle_array(char* array, size_t n) {
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

static void init_aliens_array(char* aliens_array, int dim) {    
    for (int i = 0; i < dim; i++) {
        aliens_array[i] = (i < (dim)/3) ? ALIEN_SYMBOL : ' ';
    }
    shuffle_array(aliens_array, dim);
}

/*
* Initialize ncurses mode
*/
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

    // save aliens positions in the grid and initialize the austronats areas to ' '
    for (int i = 0; i < OUTER_SPACE_SIZE; i++) {
        for (int j = 0; j < OUTER_SPACE_SIZE; j++) {
            grid[i+OFFSET][j+OFFSET] = aliens_array[j*OUTER_SPACE_SIZE+i];
        }
    }
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] != ALIEN_SYMBOL) {
                grid[i][j] = ' ';
            }
        }
    }
}

/*
* Refresh the grid with the new positions of the players and different aliens
*/
void refresh_grid(char grid[GRID_SIZE][GRID_SIZE]) {
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
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            mvwprintw(game_border_win, i+1, j+1, "%c", grid[j][i]);
        }
    }	
    wrefresh(game_border_win);
}

/*
* Print the last score board on the screen
*/
void get_score_board(Player* players, int n_players) {
    WINDOW* score_board_win = newwin(MAX_PLAYERS+2, GRID_SIZE+2, 1, GRID_SIZE+6);
    box(score_board_win, 0 , 0);	
    mvwprintw(score_board_win, 0, 1, "Score Board");
    for (int i = 0; i < n_players; i++) {
        mvwprintw(score_board_win, i+1, 1, "Player %c: %d", players[i].id, players[i].score);
    }
    wrefresh(score_board_win);
}

/*
*   Initialize the debug window
*/
void init_debug_window() {
    WINDOW* debug_win = newwin(10, GRID_SIZE+2, MAX_PLAYERS+3, GRID_SIZE+6);
    box(debug_win, 0 , 0);	
    mvwprintw(debug_win, 0, 1, "Debug Window");
    wrefresh(debug_win);
}

/*
*   Update the debug window with a new message
*/
void update_debug_window(char* msg) {
    WINDOW* debug_win = newwin(5, GRID_SIZE, MAX_PLAYERS+4, GRID_SIZE+7);
    mvwprintw(debug_win, 1, 1, msg);
    wrefresh(debug_win);
}

/*
*   Initialize the player controller: keyboard input and print its own score
*/
void init_player_controller() {
    init_ncurses();
    mvprintw(0,0,"Score: %d", 0);
}

/*
*   Add a new player to the grid
*/
void add_player(char grid[GRID_SIZE][GRID_SIZE], Player* new_player) {
    char id = new_player->id;
    /*
       Boundaries check for every player area
        from (x, y) to (x, y)
        E   (3,1) to (18,1)
        G   (3,2) to (18,2)
        A   (1,3) to (1,18)
        H   (2,3) to (2,18)
        B   (3,19) to (18,19)
        C   (3,20) to (18,20)
        F   (20, 3) to (20, 18)
        D   (19, 3) to (19, 18)
    */
   srand(time(NULL));
    switch (id)
    {
        case 'A':
            new_player->x = 1; 
            new_player->y = random_from_i_to_n(3,18);
            break;
        case 'B':
            new_player->x = random_from_i_to_n(3,18);
            new_player->y = 19;
            break;
        case 'C':
            new_player->x = random_from_i_to_n(3,18);
            new_player->y = 20;
            break;
        case 'D':
            new_player->x = 19;
            new_player->y = random_from_i_to_n(3,18);
            break;
        case 'E':
            new_player->x = random_from_i_to_n(3,18);
            new_player->y = 1;
            break;
        case 'F':
            new_player->x = 20;
            new_player->y = random_from_i_to_n(3,18);
            break;
        case 'G':
            new_player->x = random_from_i_to_n(3,18);
            new_player->y = 2;
            break;
        case 'H':
            new_player->x = 2;
            new_player->y = random_from_i_to_n(3,18);
            break;
        default:
            break;
    }
    grid[(new_player->x)-1][(new_player->y)-1] = id;
    refresh_grid(grid);
}

/*
*   Move the player in the grid
*/
void move_player(char grid[GRID_SIZE][GRID_SIZE], Player* player, action_t direction){
    if (!is_stunned(player)) {
        char id = player->id;
        int old_x = player->x;
        int old_y = player->y;
        grid[old_x-1][old_y-1] = ' ';
        switch (id) {
            case 'A':
            case 'H':
            case 'F':
            case 'D':
                if (direction == UP) {
                    (player->y) --;
                    if(player->y == 2)
                    player->y =  GRID_SIZE-OFFSET;
                } else if (direction == DOWN) {
                    (player->y) ++;
                    if(player->y == GRID_SIZE-1)
                    player->y = 3;
                }
                break;
            case 'B':
            case 'C':   
            case 'E':
            case 'G':
                if (direction == RIGHT) {
                    (player->x) ++;
                    if(player->x == GRID_SIZE-1)
                    player->x = 3;
                } else if (direction == LEFT) {
                    (player->x) --;
                    if (player->x == 2)
                    player->x = GRID_SIZE-OFFSET;
                }
                break;
            default:
                player->x = old_x;
                player->y = old_y;
                break;
        }
        grid[(player->x)-1][(player->y)-1] = player->id;
        refresh_grid(grid);
    }
}

/*
*   Get the id of the player
*/
int get_id(char* buffer, const char* player_id_chars) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (player_id_chars[i] == buffer[0]) {
            return i;
        }
    }
    return -1;
}

/*
*   Remove a player from the grid and the list of players, decreasing the total number of players
*/
void remove_player(char grid[GRID_SIZE][GRID_SIZE], Player* player, Player players[MAX_PLAYERS], int* n_players) {
    // remove player from the grid
    grid[(player->x)-1][(player->y)-1] = ' ';
    refresh_grid(grid);
    // remove player from the list
    for (int i = 0; i < *n_players; i++) {
        if (players[i].id == player->id) {
            for (int j = i; j < *n_players-1; j++) {
                players[j] = players[j+1];
            }
            (*n_players) --;
            break;
        }
    }
}

static int is_stunned(Player* player) {
    if (player->stunned) {
        int64_t current_time = s_clock();
        if (current_time - player->stun_start >= 10000) {
            player->stunned = 0;
        }
    }
    return player->stunned;
}

static int is_cooldown(Player* player) {
    if (player->cooldown) {
        int64_t current_time = s_clock();
        if (current_time - player->cooldown_start >= 3000) {
            player->cooldown = 0;
        }
    }
    return player->cooldown;
}

/*
*   Draw the - line on the screen for 0.5 seconds,
*   and after delete aliens in line of sight
*   flag the other astronauts as stunned in the same line of sight
*/
void laser_opponents(char grid[GRID_SIZE][GRID_SIZE], Player* player, Player players[MAX_PLAYERS], int n_players) {
    if (!is_cooldown(player)) {   
        int id = player->id;
        int x = player->x;
        int y = player->y;  

        switch (id) {
            case 'A':
            case 'H':
            case 'F':
            case 'D':
                for(int i = 1; i <= GRID_SIZE; i++) {
                     if (grid[i-1][y-1] != id) {
                        if (grid[i-1][y-1] == ALIEN_SYMBOL) {
                            player->score ++;
                            grid[i-1][y-1] = LASER_SYMBOL;
                        }
                        if (grid[i-1][y-1] == ' ') {
                            grid[i-1][y-1] = LASER_SYMBOL;
                        }
                        for (int j = 0; j < n_players; j++) {
                            if (grid[i-1][y-1] == players[j].id) {
                                players[j].stunned = 1; 
                                players[j].stun_start = s_clock();
                            }
                        }
                    }
                }
                player->cooldown = 1; 
                player->cooldown_start = s_clock();
                refresh_grid(grid); // TODO here I should send the grid to the display -> not worthy to implement it with this 'bad' architecture
                s_sleep(500);
                for(int i = 1; i <= GRID_SIZE; i++) {
                    if (grid[i-1][y-1] == LASER_SYMBOL) {
                        grid[i-1][y-1] = ' ';
                    }
                }
                refresh_grid(grid);
                break;
            case 'B':
            case 'C':   
            case 'E':
            case 'G':
                for(int i = 1; i <= GRID_SIZE; i++) {
                    if (grid[x-1][i-1] != id) {
                        if (grid[x-1][i-1] == ALIEN_SYMBOL) {
                            player->score ++;
                            grid[x-1][i-1] = LASER_SYMBOL_VERTICAL;
                        } 
                        if (grid[x-1][i-1] == ' ') {
                            grid[x-1][i-1] = LASER_SYMBOL_VERTICAL;
                        }
                        for (int j = 0; j < n_players; j++) {
                            if (grid[x-1][i-1] == players[j].id) {
                                players[j].stunned = 1; 
                                players[j].stun_start = s_clock();
                            }
                        }
                    }
                }
                player->cooldown = 1; 
                refresh_grid(grid);
                s_sleep(500);
                for(int i = 1; i <= GRID_SIZE; i++) {
                    if (grid[x-1][i-1] == LASER_SYMBOL_VERTICAL) {
                        grid[x-1][i-1] = ' ';
                    }
                }
                refresh_grid(grid);
                break;
            default:
                break;   
        }
    }   
}

static void send_grid(char grid[GRID_SIZE][GRID_SIZE], void *publisher) {
    char buffer[MSG_SIZE];
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            buffer[j] = grid[i][j];
        }
        s_send(publisher, buffer);
        // s_sendmore(publisher, buffer);
        // if (i == GRID_SIZE-1) {
        //     s_send(publisher, buffer);
        // }
    }
}

static void send_score_board(Player* players, int n_players, void *publisher) {
    char buffer[MSG_SIZE];
    for (int i = 0; i < n_players; i++) {
        sprintf(buffer, "%c:%d", players[i].id, players[i].score);
        s_sendmore(publisher, buffer);
        if (i == n_players-1) {
            s_send(publisher, buffer);
        }
    }
}

static void receive_grid(char grid[GRID_SIZE][GRID_SIZE], void *subscriber) {
    char buffer[MSG_SIZE];
    for (int i = 0; i < GRID_SIZE; i++) {
        strcpy(buffer, s_recv(subscriber)); // TODO maybe s_dump()???
        for (int j = 0; j < GRID_SIZE; j++) {
            grid[i][j] = buffer[j];
        }
    }
    refresh_grid(grid);
}

static void receive_score_board(Player* players, int* n_players, void *subscriber) {
    char buffer[MSG_SIZE];
    for (int i = 0; i < *n_players; i++) {
        // parse this string ("%c:%d", id, score)
        players[i].id = buffer[0];
        char* token = strtok(buffer, ":");
        token = strtok(0, ":"); // get second token
        players[i].score = atoi(token);
    }
    get_score_board(players, *n_players);
}

/*
*  Send the grid and the score board to the displays
*/
void send_display(char grid[GRID_SIZE][GRID_SIZE], Player* players, int n_players, void *publisher) {
    send_grid(grid, publisher);
    send_score_board(players, n_players, publisher);
}

/*
*  Receive the grid and the score board from the game server
*/
void receive_display(char grid[GRID_SIZE][GRID_SIZE], Player* players, int* n_players, void *subscriber) {
    receive_grid(grid, subscriber);
    receive_score_board(players, n_players, subscriber);
}
