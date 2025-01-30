
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//#include "move.c"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // For usleep
#include <locale.h>
#include "3_floor.c"

typedef struct {
    int y;
    int x;
    int size;
} room_sf;

typedef struct {
    char symbol;   // Symbol to display (e.g., 'p')
    bool visible;  // Visibility state
    int health;
    int moveable;
    int number;
} Cell_sf;
int power_sf = 1;
int speed_sf = 1;
int health_increase_sf = 1;
int spell_impact_sf = 100;
int can_move_sf = -1;
char username_sf[20];
room_sf rooms_sf[6];
int health_sf = 101;
int room_num_sf = 0;
int show_map_sf = 0;
#define MAX_HEALTH_sf 100
volatile bool stop_thread_sf = false;
volatile bool stop_thread_sf_2 = false;
#define MAP_ROWS_sf 36
#define MAP_COLS_sf 160
int room_type_sf = 0;
Cell_sf map_sf[MAP_ROWS_sf][MAP_COLS_sf];
int key_pair_sf[9];
int x_sf = 0, y_sf = 0;
int show_health_bar_sf = 0;
char str1_sf[50] = "2";

void get_parts_sf(int *rands);

void move_enemy_sf(int *enemy_y_ff_copy, int *enemy_x_ff_copy, int y_d, int x_d);

int *check_proximity_of_enemy_sf(int y_ff, int x_ff, int arr[]);

void *increase_health_bar_sf();

void draw_room_sf();

void *draw_health_bar_sf();

void *change_able_pass_sf();

void update_visibility_sf(int player_y, int player_x);

int rands_sf[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

void fill_room_sf(int start_row, int start_col, int height, int width, int tmp, int tmp2);

void fill_room_out_window_sf(int start_row, int start_col, int height, int width);

void fill_room_out_door_sf(int start_row, int start_col, int height, int width);

void put_corridor_sf(int start_row, int start_col);

int add_file_sf(int row, int col, char character);

void draw_rectangle_sf(int start_row, int start_col, int height, int width);

int reverse_number_sf(int num);

int generate_pass_key_sf(int y, int x);

int check_key_sf(int pos);

void free_map_sf();

int get_part_sf(int y, int x);

void move_to_second_line_sf(FILE *file);

void load_map_from_file_sf();

void draw_map_to_terminal_sf();

void fight_sf(char *username, int y_ff, int x_ff);

void fight_by_Dagger_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage);

void fight_by_Dagger_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage);

void fight_by_Dagger_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage);

void figh_by_Dagger_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage);

void
fighy_by_magic_wand_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage);

void
fight_by_magic_wand_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage);

void
fight_by_magic_wand_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage);

void
fight_by_magic_wand_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage);

void
fight_by_normal_arrow_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2,
                               int damage);

void
fight_by_normal_arrow_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2, int damage);

void
fight_by_normal_arrow_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2, int damage);

void
fight_by_normal_arrow_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2, int damage);

void move_to_second_line_sf(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}

void load_map_from_file_sf(char *filename) {
    FILE *file = fopen(str1_sf, "r");
    if (!file) {
        FILE *file = fopen(str1_sf, "w");
        fclose(file);
        file = fopen(str1_sf, "r");
        if (!file) {
            perror("Error opening file after creation");
            return;
        }
    }
    move_to_second_line_sf(file);
    fseek(file, 2, SEEK_CUR);
    if (!file) {
        perror("Error opening file");
        // Initialize map_sf with default values if file doesn't exist
        for (int i = 0; i < MAP_ROWS_sf; i++) {
            memset(map_sf[i], '.', MAP_COLS_sf);
            map_sf[i][MAP_COLS_sf].symbol = '\0';
        }
        return;
    }

    for (int i = 0; i < MAP_ROWS_sf; i++) {
        for (int j = 0; j < MAP_COLS_sf; j++) {
            int ch = fgetc(file); // Read character by character
            if (ch == EOF) {
                map_sf[i][j].symbol = ' '; // Fill remaining cells with dots if EOF is reached
            } else if (ch == '\0') {
                map_sf[i][j].symbol = ' '; // Replace null character with space
            } else if (ch == '\n') {
                map_sf[i][j].symbol = '\n'; // Replace newline with dot (or another filler)
            } else {
                map_sf[i][j].symbol = (char) ch;
            }
            map_sf[i][j].visible = true;
            if ((map_sf[i][j].symbol == '^') || (map_sf[i][j].symbol == '?'))
                map_sf[i][j].visible = false;
        }
    }

    fclose(file);
}


void update_visibility_sf(int player_y, int player_x) {
    if ((map_sf[player_y - 2][player_x].symbol == '?' && !map_sf[player_y - 2][player_x].visible) ||
        (map_sf[player_y - 2][player_x].symbol == '^' && !map_sf[player_y - 2][player_x].visible)) {
        map_sf[player_y - 2][player_x].visible = true; // Make the hidden 'p' visible
    }
}

void draw_map_to_terminal_sf() {
    clear();
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(6, COLOR_CYAN, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_GREEN);
    }// Clear the screen
    for (int i = 0; i < MAP_ROWS_sf; i++) {
        for (int j = 0; j < MAP_COLS_sf; j++) {
            int c = 0;
            int pos = get_part_sf(i, j);
            int pos2 = get_part_sf(y_sf, x_sf);
            int tmp = 0;
            for (int k = 0; k < pos2; ++k) {
                if (rands_sf[k] == 1)
                    tmp++;
            }
            pos2 = tmp;
            for (int i = 0; i <= pos; ++i) {
                if (rands_sf[i] == 1) {
                    c++;
                }
            }
            if (c % 2 == 1 && rands_sf[pos] == 1) {
                attron(COLOR_PAIR(7));
                show_health_bar_sf = 1;
            }
//            || map_sf[i][j] == '*' || map_sf[i][j] == '=' || map_sf[i][j] == 's' ||
//               map_sf[i][j] == '+'
            if (map_sf[i][j].symbol == '#' || map_sf[i][j].symbol == ' ')
                attroff(COLOR_PAIR(7));
            if (map_sf[i][j].symbol == '@')
                attron(COLOR_PAIR(2));
            if (map_sf[i][j].symbol == 'M')
                attron(COLOR_PAIR(3));
            if (map_sf[i][j].symbol == '&')
                attron(COLOR_PAIR(6));
            if (map_sf[i][j].symbol == 'j')
                attron(COLOR_PAIR(5));
            if (map_sf[i][j].symbol == 'G')
                attron(COLOR_PAIR(4));
            int tar = show_map_sf % 2 == 1;
            if ((tar) ||
                (i >= rooms_sf[pos2].y - 2 && (i <= rooms_sf[pos2].y + rooms_sf[pos2].size - 2) &&
                 j >= rooms_sf[pos2].x &&
                 (j <= rooms_sf[pos2].x + rooms_sf[pos2].size))) {
                if (map_sf[i][j].symbol == '^' && map_sf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_sf[i][j].symbol == '?' && map_sf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_sf[i][j].symbol); }
            } else if ((i <= y_sf + 1) && (i >= y_sf - 5) && (j <= x_sf + 3) && (j >= x_sf - 3)) {
                if (map_sf[i][j].symbol == '^' && map_sf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_sf[i][j].symbol == '?' && map_sf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_sf[i][j].symbol); }
            } else {
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(6));
                attroff(COLOR_PAIR(3));
                attroff(COLOR_PAIR(5));
                attroff(COLOR_PAIR(4));
                attroff(COLOR_PAIR(7));
                mvaddch(i + 2, j, ' ');
            }
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(6));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(5));
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(7));
            show_health_bar_sf = 0;
            if (map_sf[i][j].symbol == '\n') {

            }
            refresh();// Print each character individually
        }
    }
    move(map_sf[0][0].symbol, map_sf[0][2].symbol);
    refresh();
}


void free_map_sf() {
    // Open the file in write mode to truncate it
    FILE *file = fopen(str1_sf, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // No need to write anything, file is now empty
    fclose(file);  // Close the file

    return;
}


void draw_rectangle_sf(int start_row, int start_col, int height, int width) {
    height++;
    width++;
    rooms_sf[room_num_sf].y = start_row;
    rooms_sf[room_num_sf].x = start_col;
    rooms_sf[room_num_sf].size = height;

    for (int i = start_col; i <= start_col + width; ++i) {
        add_file_sf(start_row, i, '*');
        mvaddch(start_row, i, '*');
        refresh();
        add_file_sf(start_row + height, i, '*');
        mvaddch(start_row + height, i, '*');
        refresh();
    }
    for (int i = start_row; i <= start_row + height; ++i) {
        add_file_sf(i, start_col, '*');
        mvaddch(i, start_col, '*');
        refresh();
        add_file_sf(i, start_col + width, '*');
        mvaddch(i, start_col + width, '*');
        refresh();
    }
    room_num_sf++;
}

void draw_border_sf() {
    free_map_sf();
    add_file_sf(2, 1, '#');
    mvaddch(2, 1, '#');
    for (int i = 2; i < 36; ++i) {
        add_file_sf(i, 2, '#');
        mvaddch(i, 3 - 1, '#');
        refresh();
        add_file_sf(i, 54, '#');
        mvaddch(i, 55 - 1, '#');
        refresh();
        add_file_sf(i, 106, '#');
        mvaddch(i, 107 - 1, '#');
        refresh();
        add_file_sf(i, 158, '#');
        mvaddch(i, 159 - 1, '#');
        refresh();
    }

    for (int i = 2; i < 159; ++i) {
        add_file_sf(2, i, '#');
        mvaddch(3 - 1, i, '#');
        refresh();
        add_file_sf(13, i, '#');
        mvaddch(14 - 1, i, '#');
        refresh();
        add_file_sf(24, i, '#');
        mvaddch(25 - 1, i, '#');
        refresh();
        add_file_sf(35, i, '#');
        mvaddch(36 - 1, i, '#');
        refresh();
    }

}

int second_floor(char *username, int new) {
    my_game.Health = 100;
    pthread_t thread_id_f;
    pthread_create(&thread_id_f, NULL, food_change, NULL);
    pthread_t thread_id_ff;
    pthread_create(&thread_id_ff, NULL, increase_health_bar_sf, NULL);
    int arr[3];
    strcpy(username_sf, username);
    my_game.current_gun = 0;
    my_game.Mace = 1;
    int color_pair;
    int ch;
    // Cursor starting position
    srand(time(NULL));
    setlocale(LC_ALL, "");
    initscr();
    clear();
    noecho();
    cbreak();
    resize_term(37, 162);
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_GREEN);
        init_pair(8, COLOR_WHITE, COLOR_RED);
        init_pair(9, COLOR_WHITE, COLOR_BLUE);
        mvprintw(0, 0, "Choose a color for your hero:\n");
        mvprintw(1, 0, "1. Red background\n");
        mvprintw(2, 0, "2. Blue background\n");
        mvprintw(3, 0, "Press 1 or 2 to select.");
        refresh();

        int ch = getch();
        color_pair = (ch == '1') ? 8 : 9;

        clear();
    }
    FILE *file = fopen(str1_sf, "r");
    if (!file) {
        strcat(str1_sf, username);
        strcat(str1_sf, ".txt");
    }
    if (new) {
        draw_border_sf();
        get_parts_sf(rands_sf);
        draw_room_sf();
    }
    clear();
    refresh();
    load_map_from_file_sf(username);
    draw_map_to_terminal_sf();
    attron(COLOR_PAIR(color_pair));
    move(y_sf, x_sf);
    char s = mvinch(y_sf, x_sf);
    mvaddch(y_sf, x_sf, s);
    attroff(COLOR_PAIR(color_pair));
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, change_able_pass_sf, NULL);
    while ((ch = getch()) != 'q') {
        int x_ff_copy;
        int y_ff_copy;
        x_ff_copy = x_sf;
        y_ff_copy = y_sf;
        if (ch == 'f') {
            ch = getch();
            switch (ch) {
                case KEY_UP:
                    while (y_sf > 0) {
                        y_sf -= speed_sf;
                        if ((mvinch(y_sf, x_sf) & A_CHARTEXT) != '.' && (mvinch(y_sf, x_sf) & A_CHARTEXT) != '#') {
                            y_sf += speed_sf;
                            break;
                        }
                    }
                    break;
                case KEY_DOWN:
                    while (y_sf < MAP_ROWS_sf - 1) {
                        y_sf += speed_sf;
                        if ((mvinch(y_sf, x_sf) & A_CHARTEXT) != '.' && (mvinch(y_sf, x_sf) & A_CHARTEXT) != '#') {
                            y_sf -= speed_sf;
                            break;
                        }
                    }
                    break;
                case KEY_LEFT:
                    while (x_sf > 0) {
                        x_sf -= speed_sf;
                        if ((mvinch(y_sf, x_sf) & A_CHARTEXT) != '.' && (mvinch(y_sf, x_sf) & A_CHARTEXT) != '#') {
                            x_sf += speed_sf;
                            break;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    while (x_sf < MAP_COLS_sf - 1) {
                        x_sf += speed_sf;
                        if ((mvinch(y_sf, x_sf) & A_CHARTEXT) != '.' && (mvinch(y_sf, x_sf) & A_CHARTEXT) != '#') {
                            x_sf -= speed_sf;
                            break;
                        }
                    }
                    break;
            }
        }
        switch (ch) {
            case ' ':
                //  char gun = getch();
                fight_sf(username, y_sf, x_sf);
                break;
            case 'e':
                clear();
                eat_food(&speed_sf, &power_sf);
                mvprintw(10, 4, "%d", speed_sf);
                spell_impact_sf = 1;
                getch();
                load_map_from_file_sf(username);
                draw_map_to_terminal_sf();
                break;
            case 'j':
                clear();
                display_spell();
                load_map_from_file_sf(username);
                draw_map_to_terminal_sf();
                break;
            case 'i':
                clear();
                display_guns();
                load_map_from_file_sf(username);
                draw_map_to_terminal_sf();
                break;
            case 'v':
                show_map_sf++;
                draw_map_to_terminal_sf();
                break;
            case KEY_UP:
                if (y_sf > 0) y_sf--;
                update_visibility_sf(y_sf, x_sf);
                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == '=') {
                    y_sf++;
                }
                break;
            case KEY_DOWN:
                if (y_sf < MAP_ROWS_sf - 1) y_sf += speed_sf;
                update_visibility_sf(y_sf, x_sf);
                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == '=') {
                    y_sf -= speed_sf;
                }
                break;
            case KEY_LEFT:
                if (x_sf > 0) x_sf -= speed_sf;
                update_visibility_sf(y_sf, x_sf);
                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == '=') {
                    x_sf += speed_sf;
                }
                break;
            case KEY_RIGHT:
                if (x_sf < MAP_COLS_sf - 1) x_sf += speed_sf;
                update_visibility_sf(y_sf, x_sf);

                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == '=') {
                    x_sf -= speed_sf;
                }
                break;
            case 'p':
                if (y_sf > 0) y_sf -= speed_sf;
                if (x_sf < MAP_COLS_sf - 1) x_sf += speed_sf;
                update_visibility_sf(y_sf, x_sf);

                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o') {
                    y_sf += speed_sf;
                    x_sf -= speed_sf;
                }
                break;
            case 'w':
                if (y_sf > 0) y_sf -= speed_sf;
                if (x_sf > 0) x_sf -= speed_sf;
                update_visibility_sf(y_sf, x_sf);

                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o') {
                    y_sf += speed_sf;
                    x_sf += speed_sf;
                }
                break;
            case ',':
                if (y_sf < MAP_ROWS_sf - 1) y_sf += speed_sf;
                if (x_sf < MAP_COLS_sf - 1) x_sf += speed_sf;
                update_visibility_sf(y_sf, x_sf);
                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o') {
                    y_sf -= speed_sf;
                    x_sf -= speed_sf;
                }
                break;
            case 'z':
                if (x_sf > 0) x_sf -= speed_sf;
                if (y_sf < MAP_COLS_sf - 1) y_sf += speed_sf;
                update_visibility_sf(y_sf, x_sf);

                if ((mvinch(y_sf, x_sf) & A_CHARTEXT) == '*' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'o') {
                    y_sf -= speed_sf;
                    x_sf += speed_sf;
                }
                break;
        }

        update_visibility_sf(y_sf, x_sf);
        draw_map_to_terminal_sf();
        if (x_sf < 1) x_sf = 1;
        if (y_sf >= 35) y_sf = 35;
        if (y_sf < 2) y_sf = 2;
        if (x_sf >= 161) x_sf = 161;
        int enemy_x_ff_copy;
        int enemy_y_ff_copy;
        if (check_proximity_of_enemy_sf(y_sf, x_sf, arr)[0]) {
            enemy_x_ff_copy = check_proximity_of_enemy_sf(y_sf, x_sf, arr)[2];
            enemy_y_ff_copy = check_proximity_of_enemy_sf(y_sf, x_sf, arr)[1];
            //  mvprintw(5, 0, "%d %d %c", enemy_y_ff_copy, enemy_x_ff_copy, mvinch(enemy_y_ff_copy, enemy_x_ff_copy));
            switch (check_proximity_of_enemy_sf(y_sf, x_sf, arr)[0]) {
                case 3:
                    can_move_sf = 5;
                    break;
                case 4:
                    can_move_sf = 100;
                    break;
                case 5:
                    can_move_sf = 5;
                    break;
            }
            stop_thread_sf = 0;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, draw_health_bar_sf, NULL);
            if (my_game.Health < 0) {
                break;
            }
        } else {
            if (can_move_sf > 0) {
                move_enemy_sf(&enemy_y_ff_copy, &enemy_x_ff_copy, y_sf - y_ff_copy, x_sf - x_ff_copy);
                can_move_sf--;
            }
            stop_thread_sf = 1;
        }
        chtype ch = mvinch(y_sf, x_sf);
        chtype attributes = ch & A_ATTRIBUTES;
        int color_pair_s = PAIR_NUMBER(attributes);
        if (color_pair_s == 7) {
            stop_thread_sf = false;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, draw_health_bar_sf, NULL);
            if (my_game.Health < 0) {
                break;
            }
        }
        attron(COLOR_PAIR(color_pair));
        move(y_sf, x_sf);
        char s = mvinch(y_sf, x_sf);
        mvaddch(y_sf, x_sf, s);
        attroff(COLOR_PAIR(color_pair));
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '&')) {
            generate_pass_key_sf(y_sf, x_sf);
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '<')) {
            stop_thread_sf = true;
            third_floor(username, 1);
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '>')) {
            stop_thread_sf = true;
            first(username);
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '^')) {
            health_sf--;
            mvprintw(0, 2, "Your health decreased by a trap!");
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '@')) {
            int pos = get_part_sf(y_sf, x_sf);
            check_key_sf(pos);
            attron(COLOR_PAIR(3));
            mvaddch(y_sf, x_sf, '@');
            attroff(COLOR_PAIR(3));
        }

        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == '#')) {
            stop_thread_sf = true;
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'G') || (mvinch(y_sf, x_sf) & A_CHARTEXT) == '$') {
            int achived_gold = calc_gold((mvinch(y_sf, x_sf) & A_CHARTEXT));
            add_file_sf(y_sf, x_sf, '.');
            mvaddch(y_sf, x_sf, '.');
            mvprintw(1, 2, "you achived %d golds", achived_gold);
            getch();
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
            refresh();
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'M') || (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'D' ||
            (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'W' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'A' ||
            (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'S') {
            char inp = getch();
            if (inp == 'g') {
                calc_gun(mvinch(y_sf, x_sf) & A_CHARTEXT, map_sf[y_sf - 2][x_sf].number);
                add_file_sf(y_sf, x_sf, '.');
                mvaddch(y_sf, x_sf, '.');
                load_map_from_file_sf(username);
                draw_map_to_terminal_sf();
            }
        }
        if (spell_impact_sf > 10) {
            power_sf = 1;
            speed_sf = 1;
            health_increase_sf = 1;
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'h') || (mvinch(y_sf, x_sf) & A_CHARTEXT) == 's' ||
            (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'd') {
            char inp = getch();
            if (inp == 'g') {
                if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'd')) {
                    power_sf = 2;
                }
                if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 's')) {
                    speed_sf = 2;
                }
                if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'h')) {
                    health_increase_sf = 2;
                    stop_thread_sf = 0;
//                    pthread_t thread_id;
//                    pthread_create(&thread_id, NULL, increase_health_bar_sf, NULL);
                }
                spell_impact_sf = 1;
                calc_spell(mvinch(y_sf, x_sf) & A_CHARTEXT);
                add_file_sf(y_sf, x_sf, '.');
                mvaddch(y_sf, x_sf, '.');
                load_map_from_file_sf(username);
                draw_map_to_terminal_sf();
            }
        }
        if (((mvinch(y_sf, x_sf) & A_CHARTEXT) == 'n') || (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'm' ||
            (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'b' || (mvinch(y_sf, x_sf) & A_CHARTEXT) == 'c') {
            char inp = getch();
            if (inp == 'g') {
                int food_res = calc_food(mvinch(y_sf, x_sf) & A_CHARTEXT);
                if (food_res) {
                    add_file_sf(y_sf, x_sf, '.');
                    mvaddch(y_sf, x_sf, '.');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                } else {
                    mvprintw(1, 2, "you already have maximum(5) food");
                    getch();
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    refresh();
                }
            }
        }
        attron(COLOR_PAIR(color_pair));
        move(y_sf, x_sf);
        char s1 = mvinch(y_sf, x_sf);
        mvaddch(y_sf, x_sf, s1);
        attroff(COLOR_PAIR(color_pair));
        spell_impact_sf++;
//        getch();
//        mvaddch(10,3,'T');
//        refresh();
    }
    update_game_in_database(username, 2);
    exit(1);
    // save_map_to_file(y_sf,x_sf);

    endwin();
    return 0;
}

int *check_proximity_of_enemy_sf(int y_ff, int x_ff, int arr[]) {
    arr[0] = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (mvinch(y_ff + i, x_ff + j) == 'N') {
                arr[0] = 1;
                arr[1] = y_ff + i;
                arr[2] = x_ff + j;
                return arr;
            }
            if (mvinch(y_ff + i, x_ff + j) == 'M') {
                arr[0] = 2;
                arr[1] = y_ff + i;
                arr[2] = x_ff + j;
                return arr;
            }
            if (mvinch(y_ff + i, x_ff + j) == 'g') {
                arr[0] = 3;
                arr[1] = y_ff + i;
                arr[2] = x_ff + j;
                return arr;
            }
            if (mvinch(y_ff + i, x_ff + j) == 'K') {
                arr[0] = 4;
                arr[1] = y_ff + i;
                arr[2] = x_ff + j;
                return arr;
            }

            if (mvinch(y_ff + i, x_ff + j) == 'U') {
                arr[0] = 5;
                arr[1] = y_ff + i;
                arr[2] = x_ff + j;
                return arr;
            }

        }
    }
    return arr;
}

//void move_enemy_sf(int *enemy_y_ff_copy, int *enemy_x_ff_copy, int y_d, int x_d) {
//    if (map_sf[(*enemy_y_ff_copy - 2)][(*enemy_x_ff_copy)].moveable <= 0)
//        return;
//    mvprintw(0, 2, "getting damage!");
//    char enemy_t = (mvinch(*enemy_y_ff_copy, *enemy_x_ff_copy) & A_CHARTEXT);
//    add_file_sf(y_sf + y_d, x_sf + x_d, enemy_t);
//    mvaddch(y_sf + y_d, x_sf + x_d, enemy_t);
//    map_sf[y_sf + y_d - 2][x_sf + x_d].symbol = map_sf[y_sf][x_sf].symbol;
//    map_sf[y_sf + y_d - 2][x_sf + x_d].health = map_sf[y_sf][x_sf].health;
//    map_sf[y_sf + y_d - 2][x_sf + x_d].moveable = map_sf[y_sf][x_sf].moveable - 1;
//    load_map_from_file_sf(username_sf);
//    draw_map_to_terminal_sf();
//    *enemy_y_ff_copy = (*enemy_y_ff_copy + y_d);
//    *enemy_x_ff_copy = (*enemy_x_ff_copy + x_d);
//}
void move_enemy_sf(int *enemy_y_ff_copy, int *enemy_x_ff_copy, int y_d, int x_d) {
    // Validate pointers
    if (!enemy_y_ff_copy || !enemy_x_ff_copy) {
        mvprintw(0, 0, "Error: Null pointer passed to move_enemy_sf");
        getch();
        return;
    }
    int y_enemy = *enemy_y_ff_copy;
    int x_enemy = *enemy_x_ff_copy;
    // Ensure moveability
    if (map_sf[y_enemy - 2][x_enemy].moveable <= 0)
        return;


    char enemy_t = mvinch(y_enemy, x_enemy);
    mvprintw(7, 4, "getting damage %c!", enemy_t);
//    getch();

    // Update map and character position
    {
        add_file_sf(y_enemy + y_d, x_enemy + x_d, enemy_t);
        mvaddch(y_enemy + y_d, x_enemy + x_d, enemy_t);

        map_sf[y_enemy + y_d - 2][x_enemy + x_d].symbol = map_sf[y_enemy - 2][x_enemy].symbol;
        map_sf[y_enemy + y_d - 2][x_enemy + x_d].health = map_sf[y_enemy - 2][x_enemy].health;
        map_sf[y_enemy + y_d - 2][x_enemy + x_d].moveable = map_sf[y_enemy - 2][x_enemy].moveable - 1;

        map_sf[y_enemy - 2][x_enemy].symbol = '.';
        map_sf[y_enemy - 2][x_enemy].health = 0;
        map_sf[y_enemy - 2][x_enemy].moveable = 0;
        add_file_sf(y_enemy, x_enemy, '.');
        mvaddch(y_enemy, x_enemy, '.');
        load_map_from_file_sf(username_sf);
        draw_map_to_terminal_sf();

        *enemy_y_ff_copy = (*enemy_y_ff_copy + y_d);
        *enemy_x_ff_copy = (*enemy_x_ff_copy + x_d);
    }
}


char *is_enemy_sf(int y, int x, int damage) {
    damage *= power_sf;
    char target = mvinch(y, x) & A_CHARTEXT;
    static char buffer[100];
    int health;

    switch (target) {
        case 'N':
            if (map_sf[y - 2][x].health <= 0) {
                mvprintw(0, 2, "The Deamon is died!");
                getch();
                add_file_sf(y, x, '.');
                map_sf[y - 2][x].number = 1;
                map_sf[y - 2][x].symbol = '.';
                mvaddch(y, x, '.');
                load_map_from_file_sf(username_sf);
                draw_map_to_terminal_sf();
                break;
            }
            map_sf[y - 2][x].health -= damage;
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Deamon : %d", health);
            return buffer;

        case 'M':
            if (map_sf[y - 2][x].health <= 0) {
                mvprintw(0, 2, "The Monster died!");
                getch();
                add_file_sf(y, x, '.');
                map_sf[y - 2][x].number = 1;
                map_sf[y - 2][x].symbol = '.';
                mvaddch(y, x, '.');
                load_map_from_file_sf(username_sf);
                draw_map_to_terminal_sf();
                break;
            }
            map_sf[y - 2][x].health -= damage;
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Monster : %d", health);
            return buffer;

        case 'g':
            if (map_sf[y - 2][x].health <= 0) {
                mvprintw(0, 2, "The Giant is died!");
                getch();
                add_file_sf(y, x, '.');
                map_sf[y - 2][x].number = 1;
                map_sf[y - 2][x].symbol = '.';
                mvaddch(y, x, '.');
                load_map_from_file_sf(username_sf);
                draw_map_to_terminal_sf();
                break;
            }
            map_sf[y - 2][x].health -= damage;
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Giant : %d", health);
            return buffer;

        case 'K':
            if (map_sf[y - 2][x].health <= 0) {
                mvprintw(0, 2, "The Snake is died!");
                getch();
                add_file_sf(y, x, '.');
                map_sf[y - 2][x].number = 1;
                map_sf[y - 2][x].symbol = '.';
                mvaddch(y, x, '.');
                load_map_from_file_sf(username_sf);
                draw_map_to_terminal_sf();
                break;
            }
            map_sf[y - 2][x].health -= damage;
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Snake : %d", health);
            return buffer;

        case 'U':
            if (map_sf[y - 2][x].health <= 0) {
                mvprintw(0, 2, "The Undeed is died!");
                getch();
                add_file_sf(y, x, '.');
                map_sf[y - 2][x].number = 1;
                map_sf[y - 2][x].symbol = '.';
                mvaddch(y, x, '.');
                load_map_from_file_sf(username_sf);
                draw_map_to_terminal_sf();
                break;
            }
            map_sf[y - 2][x].health -= damage;
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Undeed : %d", health);
            return buffer;

        default:
            return NULL;
    }
}


void fight_by_short_range_sf(int y_ff, int x_ff, int damage) {
    do {
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                char *str = is_enemy_sf(y_ff + i, x_ff + j, damage);
                if (str) {
                    mvprintw(0, 2, "You hitted %s !", str);
                    getch();
                }
            }
        }
    } while (getch() == 'a');
}

void fight_sf(char *username, int y_ff, int x_ff) {
    int x_copy;
    int y_copy;
    keypad(stdscr, TRUE); // Enable special keys (arrow keys)
    switch (my_game.current_gun) {
        case 0:
            fight_by_short_range_sf(y_ff, x_ff, 5);
            break;
        case 1:
            if (my_game.Dagger <= 0) {
                mvprintw(0, 2, "Your Daggers done!");
                getch();
                break;
            }
            int i = 0;
            int dir = getch();
            switch (dir) {
                int flag = 1;
                case KEY_RIGHT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_Dagger_right_sf(username, y_ff, x_ff, x_copy, y_copy, i, flag, 12);
                    break;
                case KEY_LEFT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_Dagger_left_sf(username, y_ff, x_ff, x_copy, y_copy, i, flag, 12);
                    break;
                case KEY_UP:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_Dagger_up_sf(username, y_ff, x_ff, x_copy, y_copy, i, flag, 12);
                    break;
                case KEY_DOWN:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    figh_by_Dagger_down_sf(username, y_ff, x_ff, x_copy, y_copy, i, flag, 12);
            }
            if (my_game.Dagger <= 0) {
                mvprintw(0, 2, "Your Daggers done!");
                break;
            }
            break;
        case 2:
            if (my_game.Magic_Wand <= 0) {
                mvprintw(0, 2, "Your Magic_Wands done!");
                getch();

                break;
            }
            int i1 = 0;
            int dir2 = getch();
            switch (dir2) {
                int flag = 1;
                case KEY_RIGHT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fighy_by_magic_wand_right_sf(username, y_ff, x_ff, x_copy, y_copy, i1, flag, 15);
                    break;
                case KEY_LEFT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_magic_wand_left_sf(username, y_ff, x_ff, x_copy, y_copy, i1, flag, 15);
                    break;
                case KEY_UP:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_magic_wand_up_sf(username, y_ff, x_ff, x_copy, y_copy, i1, flag, 15);
                    break;
                case KEY_DOWN:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_magic_wand_down_sf(username, y_ff, x_ff, x_copy, y_copy, i1, flag, 15);
            }
            if (my_game.Magic_Wand <= 0) {
                mvprintw(0, 2, "Your Magic_Wands done!");
                break;
            }
            break;
        case 3:
            if (my_game.Normal_Arrow <= 0) {
                mvprintw(0, 2, "Your Normal_Arrow's done!");
                getch();

                break;
            }
            int dir1 = getch();
            switch (dir1) {
                int flag = 1;
                int i2 = 0;
                case KEY_RIGHT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_normal_arrow_right_sf(username, y_ff, x_ff, x_copy, y_copy, flag, i2, 5);
                    break;
                case KEY_LEFT:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_normal_arrow_left_sf(username, y_ff, x_ff, x_copy, y_copy, flag, i2, 5);
                    break;
                case KEY_UP:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_normal_arrow_up_sf(username, y_ff, x_ff, x_copy, y_copy, flag, i2, 5);
                    break;
                case KEY_DOWN:
                    x_copy = x_ff;
                    y_copy = y_ff;
                    fight_by_normal_arrow_down_sf(username, y_ff, x_ff, x_copy, y_copy, flag, i2, 5);
            }
            if (my_game.Normal_Arrow <= 0) {
                mvprintw(0, 2, "Your Normal_Arrow's done!");
                break;
            }
            break;
        case 4:
            fight_by_short_range_sf(y_ff, x_ff, 10);
            break;
    }

}

char *get_victim_inf_sf(int y, int x) {
//    return "salam";
    char target = mvinch(y, x) & A_CHARTEXT;
    static char buffer[100];
    int health;

    switch (target) {
        case 'N':
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Deamon : %d", health);
            return buffer;

        case 'M':
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Monster : %d", health);
            return buffer;

        case 'g':
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Giant : %d", health);
            return buffer;

        case 'K':
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Snake : %d", health);
            return buffer;

        case 'U':
            health = map_sf[y - 2][x].health;
            snprintf(buffer, sizeof(buffer), "Undeed : %d", health);
            return buffer;

        default:
            return NULL;
    }
}

void
fight_by_normal_arrow_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2,
                              int damage) {
    damage *= power_sf;
    do {
        y_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                char *str = get_victim_inf_sf(y_ff, x_ff);
                mvprintw(0, 2, "You hitted %s!", str);
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!", str);
                    getch();
                    add_file_sf(y_ff, x_ff, 'A');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'A');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff++;
            i2++;
            if (i2 == 5)
                break;
        }

        if (flag) {
            y_ff--;
            add_file_sf(y_ff, x_ff, 'A');
            mvaddch(y_ff, x_ff, 'A');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Normal_Arrow--;
    } while (getch() == 'a');
}

void
fight_by_normal_arrow_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2, int damage) {
    damage *= power_sf;
    do {
        y_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'A');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'A');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff--;
            i2++;
            if (i2 == 5)
                break;
        }
        if (flag) {
            y_ff++;
            add_file_sf(y_ff, x_ff, 'A');
            mvaddch(y_ff, x_ff, 'A');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Normal_Arrow--;
    } while (getch() == 'a');
}

void
fight_by_normal_arrow_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2,
                              int damage) {
    damage *= power_sf;
    do {
        x_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'A');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'A');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff--;
            i2++;
            if (i2 == 5)
                break;
        }
        if (flag) {
            x_ff++;
            add_file_sf(y_ff, x_ff, 'A');
            mvaddch(y_ff, x_ff, 'A');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Normal_Arrow--;
    } while (getch() == 'a');
}

void
fight_by_normal_arrow_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int flag, int i2,
                               int damage) {
    damage *= power_sf;
    do {
        x_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'A');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'A');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff++;
            i2++;
            if (i2 == 5)
                break;
        }
        if (flag) {
            x_ff--;
            add_file_sf(y_ff, x_ff, 'A');
            mvaddch(y_ff, x_ff, 'A');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Normal_Arrow--;
    } while (getch() == 'a');
}

void
fight_by_magic_wand_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage) {
    damage *= power_sf;
    do {
        y_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                map_sf[y_ff - 2][x_ff].moveable = -1;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'W');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'W');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff++;
            i1++;
            if (i1 == 10)
                break;
        }
        if (flag) {
            y_ff--;
            add_file_sf(y_ff, x_ff, 'W');
            mvaddch(y_ff, x_ff, 'W');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Magic_Wand--;
    } while (getch() == 'a');
}

void
fight_by_magic_wand_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage) {
    damage *= power_sf;
    do {
        y_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                map_sf[y_ff - 2][x_ff].moveable = -1;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'W');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'W');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff--;
            i1++;
            if (i1 == 10)
                break;
        }
        if (flag) {
            y_ff++;
            add_file_sf(y_ff, x_ff, 'W');
            mvaddch(y_ff, x_ff, 'W');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Magic_Wand--;
    } while (getch() == 'a');
}

void
fight_by_magic_wand_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage) {
    damage *= power_sf;
    do {
        x_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                map_sf[y_ff - 2][x_ff].moveable = -1;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'W');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'W');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff--;
            i1++;
            if (i1 == 10)
                break;
        }
        if (flag) {
            x_ff++;
            add_file_sf(y_ff, x_ff, 'W');
            mvaddch(y_ff, x_ff, 'W');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Magic_Wand--;
    } while (getch() == 'a');
}

void
fighy_by_magic_wand_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i1, int flag, int damage) {
    damage *= power_sf;
    do {
        x_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                map_sf[y_ff - 2][x_ff].moveable = -1;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'W');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'W');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff++;
            i1++;
            if (i1 == 10)
                break;
        }
        if (flag) {
            x_ff--;
            add_file_sf(y_ff, x_ff, 'W');
            mvaddch(y_ff, x_ff, 'W');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Magic_Wand--;
    } while (getch() == 'a');
}

void figh_by_Dagger_down_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage) {
    damage *= power_sf;
    do {
        y_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'D');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'D');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff++;
            i++;
            if (i == 5)
                break;
        }
        if (flag) {
            y_ff--;
            add_file_sf(y_ff, x_ff, 'D');
            mvaddch(y_ff, x_ff, 'D');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Dagger--;
        if (my_game.Dagger <= 0) {
            mvprintw(0, 2, "Your Daggers done!");
            getch();
            break;
        }
    } while (getch() == 'a');
}

void fight_by_Dagger_up_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage) {
    damage *= power_sf;
    do {
        y_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'D');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'D');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            y_ff--;
            i++;
            if (i == 5)
                break;
        }
        if (flag) {
            y_ff++;
            add_file_sf(y_ff, x_ff, 'D');
            mvaddch(y_ff, x_ff, 'D');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Dagger--;
        if (my_game.Dagger <= 0) {
            mvprintw(0, 2, "Your Daggers done!");
            getch();
            break;
        }
    } while (getch() == 'a');
}

void fight_by_Dagger_left_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage) {
    damage *= power_sf;
    do {
        x_ff--;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'D');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'D');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff--;
            i++;
            if (i == 5)
                break;
        }
        if (flag) {
            x_ff++;
            add_file_sf(y_ff, x_ff, 'D');
            mvaddch(y_ff, x_ff, 'D');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
//                    getch();
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Dagger--;
        if (my_game.Dagger <= 0) {
            mvprintw(0, 2, "Your Daggers done!");
            getch();
            break;
        }
    } while (getch() == 'a');
}

void fight_by_Dagger_right_sf(char *username, int y_ff, int x_ff, int x_copy, int y_copy, int i, int flag, int damage) {
    damage *= power_sf;
    do {
        x_ff++;
        while ((mvinch(y_ff, x_ff) & A_CHARTEXT) != '*') {
            if ((mvinch(y_ff, x_ff) & A_CHARTEXT) == 'N' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'M' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'g' || (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'K' ||
                (mvinch(y_ff, x_ff) & A_CHARTEXT) == 'U') {
                map_sf[y_ff - 2][x_ff].health -= damage;
                mvprintw(0, 2, "You hitted %s!", get_victim_inf_sf(y_ff, x_ff));
                refresh();
//                getch();
                flag = 0;
                if (map_sf[y_ff - 2][x_ff].health <= 0) {
                    mvprintw(0, 2, "The enemy is died!");
                    getch();
                    add_file_sf(y_ff, x_ff, 'D');
                    map_sf[y_ff - 2][x_ff].number = 1;
                    mvaddch(y_ff, x_ff, 'D');
                    load_map_from_file_sf(username);
                    draw_map_to_terminal_sf();
                    break;
                }
            }
            x_ff++;
            i++;
            if (i == 5)
                break;
        }
        if (flag) {
            x_ff--;
            add_file_sf(y_ff, x_ff, 'D');
            mvaddch(y_ff, x_ff, 'D');
            load_map_from_file_sf(username);
            draw_map_to_terminal_sf();
        }
        x_ff = x_copy;
        y_ff = y_copy;
        my_game.Dagger--;
        if (my_game.Dagger <= 0) {
            mvprintw(0, 2, "Your Daggers done!");
            getch();
            break;
        }
    } while (getch() == 'a');
}

void get_parts_sf(int *rands) {
    int count = 0;
    while (true) {
        int rand_num = rand() % 9;
        if (rands[rand_num] == -1) {
            rands[rand_num] = 1;
            count++;
        }
        if (count == 6)
            break;
    }
}

void draw_room_sf() {
    int flag = 0;
    int flag2 = 0;

    int tmp = 0;
    int tmp2 = 0;
    int k = rand() % 6;
    int k2 = rand() % 6;
    k++;
    k2++;
    for (int i = 0; i < 9; ++i) {
        int start_row = rand() % 3;
        int start_col = rand() % 40;
        int size = (rand() % 3) + 4;
        if (rands_sf[i] == 1) {
            room_type_sf++;
            flag++;
            flag2++;
            if (flag == k) {
                tmp = 1;
            }
            if (flag2 == k2) {
                tmp2 = 1;
            }
            start_row += 3 + 11 * (i / 3);
            start_col += 3 + 52 * (i % 3);
            draw_rectangle_sf(start_row, start_col, size, size);
            fill_room_sf(start_row, start_col, size, size, tmp, tmp2);
            fill_room_out_window_sf(start_row, start_col, size, size);
            fill_room_out_door_sf(start_row, start_col, size, size);
            tmp = 0;
            tmp2 = 0;
        }
    }
}

void fill_room_sf(int start_row, int start_col, int height, int width, int tmp, int tmp2) {
    int o_num = rand() % 3;
    for (int i = start_col + 1; i <= start_col + width; ++i) {
        for (int j = start_row + 1; j <= start_row + height; ++j) {
            add_file_sf(j, i, '.');
            if (tmp) {
                y_sf = j;
                x_sf = i;
            }
            mvaddch(j, i, '.');
            refresh();
        }
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'o');
        mvaddch(row, col, 'o');
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, '^');
        mvaddch(row, col, '^');
    }

    if (room_type_sf % 2 != 1) {
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 'd');
            mvaddch(row, col, 'd');
        }
    }
    if (room_type_sf % 2 == 1) {
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_sf(row, col, 'd');
            mvaddch(row, col, 'd');
        }

    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'G');
        mvaddch(row, col, 'G');
    }
    int r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, '$');
        mvaddch(row, col, '$');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'D');
        map_sf[row - 2][col].number = 10;
        mvaddch(row, col, 'D');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'W');
        map_sf[row - 2][col].number = 8;
        mvaddch(row, col, 'W');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'A');
        map_sf[row - 2][col].number = 20;
        mvaddch(row, col, 'A');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'S');
        map_sf[row - 2][col].number = 1;
        mvaddch(row, col, 'S');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'n');
        mvaddch(row, col, 'n');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'm');
        mvaddch(row, col, 'm');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'b');
        mvaddch(row, col, 'b');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'c');
        mvaddch(row, col, 'c');
    }

    ////////////////////////////////
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'N');
        map_sf[row - 2][col].health = 5;
        map_sf[row - 2][col].moveable = 0;
        mvaddch(row, col, 'N');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'M');
        map_sf[row - 2][col].health = 10;
        map_sf[row - 2][col].moveable = 0;
        mvaddch(row, col, 'M');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'g');
        map_sf[row - 2][col].health = 15;
        map_sf[row - 2][col].moveable = 5;
        mvaddch(row, col, 'g');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'K');
        map_sf[row - 2][col].health = 20;
        map_sf[row - 2][col].moveable = 100;
        mvaddch(row, col, 'K');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'U');
        map_sf[row - 2][col].health = 30;
        map_sf[row - 2][col].moveable = 5;
        mvaddch(row, col, 'U');
    }
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, '>');
        mvaddch(row, col, '>');
    }
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, '<');
        mvaddch(row, col, '<');
    }

}

void fill_room_out_window_sf(int start_row, int start_col, int height, int width) {
    int index = rand() % (2 * (height + 1 + width + 1));
    if (index < width + 1) {
        add_file_sf(start_row, start_col + index, '=');
        mvaddch(start_row, start_col + index, '=');
    } else if (width < index && index < width + height + 2) {
        index -= (width + 1);
        add_file_sf(start_row + index, start_col + width + 1, '=');
        mvaddch(start_row + index, start_col + width + 1, '=');
    } else if (width + height + 1 < index && index < width + height + width + 3) {
        index -= (width + height + 2);
        add_file_sf(start_row + height + 1, start_col + width + 1 - index, '=');
        mvaddch(start_row + height + 1, start_col + width + 1 - index, '=');
    } else {
        index -= (width + height + width + 3);
        add_file_sf(start_row + height + 1 - index, start_col, '=');
        mvaddch(start_row + height + 1 - index, start_col, '=');
    }
}

void fill_room_out_door_sf(int start_row, int start_col, int height, int width) {
    int repeat = rand() % 2 + 1;
    int index = rand() % (2 * (height + 1 + width + 1));

    if (has_colors()) {
        start_color();
        init_color(COLOR_RED, 184, 142, 12);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
    }

    if (repeat == 1) {
        int index = rand() % (2 * (height + 1 + width + 1));
        if (index < width + 1) {
            add_file_sf(start_row, start_col + index, '?');
            mvaddch(start_row, start_col + index, '?');
            put_corridor_sf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_sf(start_row + index, start_col + width + 1, '?');
            mvaddch(start_row + index, start_col + width + 1, '?');
            put_corridor_sf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_sf(start_row + height + 1, start_col + width + 1 - index, '?');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '?');
            put_corridor_sf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_sf(start_row + height + 1 - index, start_col, '?');
            mvaddch(start_row + height + 1 - index, start_col, '?');
            put_corridor_sf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat; ++i) {
        int index = rand() % (2 * (height + 1 + width + 1));
        if (index < width + 1) {
            add_file_sf(start_row, start_col + index, '+');
            mvaddch(start_row, start_col + index, '+');
            put_corridor_sf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_sf(start_row + index, start_col + width + 1, '+');
            mvaddch(start_row + index, start_col + width + 1, '+');
            put_corridor_sf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_sf(start_row + height + 1, start_col + width + 1 - index, '+');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '+');
            put_corridor_sf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_sf(start_row + height + 1 - index, start_col, '+');
            mvaddch(start_row + height + 1 - index, start_col, '+');
            put_corridor_sf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat - 1; ++i) {
        if (index < width + 1) {
            add_file_sf(start_row, start_col + index, '@');
            mvaddch(start_row, start_col + index, '@');
            put_corridor_sf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_sf(start_row + index, start_col + width + 1, '@');
            mvaddch(start_row + index, start_col + width + 1, '@');
            put_corridor_sf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_sf(start_row + height + 1, start_col + width + 1 - index, '@');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '@');
            put_corridor_sf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_sf(start_row + height + 1 - index, start_col, '@');
            mvaddch(start_row + height + 1 - index, start_col, '@');
            put_corridor_sf(start_row + height + 1 - index, start_col);
        }
    }
    if (repeat == 2) {
        start_row++;
        start_col++;
        width -= 2;
        height -= 2;
        if (index < width + 1) {
            add_file_sf(start_row, start_col, '&');
            mvaddch(start_row, start_col, '&');
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_sf(start_row, start_col + width + 1, '&');
            mvaddch(start_row, start_col + width + 1, '&');
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_sf(start_row + height + 1, start_col + width + 1, '&');
            mvaddch(start_row + height + 1, start_col + width + 1, '&');
        } else {
            index -= (width + height + width + 3);
            add_file_sf(start_row + height + 1, start_col, '&');
            mvaddch(start_row + height + 1, start_col, '&');
        }
    }
}

void put_corridor_sf(int start_row, int start_col) {
    int flag = 0;
    while ((mvinch(start_row + 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_sf(start_row + 1, start_col, '#');
        mvaddch(start_row + 1, start_col, '#');
        start_row++;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col + 1) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_sf(start_row, start_col + 1, '#');
        mvaddch(start_row, start_col + 1, '#');
        start_col++;
    }
    if (flag)
        return;
    while ((mvinch(start_row - 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_sf(start_row - 1, start_col, '#');
        mvaddch(start_row - 1, start_col, '#');
        start_row--;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col - 1) & A_CHARTEXT) == ' ') {
        add_file_sf(start_row, start_col - 1, '#');
        mvaddch(start_row, start_col - 1, '#');
        start_col--;
        if (start_col == 3) {
            add_file_sf(start_row, 2, '#');
            mvaddch(start_row, 2, '#');
        }
    }

}


#include <stdio.h>

int add_file_sf(int row, int col, char character) {
    FILE *file;

    file = fopen(str1_sf, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    long position = 0;
    char ch;

    for (int i = 0; i < row; i++) {
        while ((ch = fgetc(file)) != EOF && ch != '\n') {
            position++;
        }
        if (ch == EOF) {
            fseek(file, position, SEEK_SET);
            fputc('\n', file);
            break;
        }
        if (ch == '\n') {
            position++;  // Move past the newline
        }
    }

    // Now, position is set to the beginning of the row, and we add the character at column
    position += col;

    fseek(file, position, SEEK_SET);
    fputc(character, file);

    fclose(file);

    return 0;
}

int generate_pass_key_sf(int y, int x) {
    int pos = get_part_sf(y, x);
    srand(time(0));
    int password = 1000 + rand() % 9000;

    int number = password; // The number to display

    // Initialize ncurses
    initscr();
    noecho();
    cbreak();

    // Display the number
    mvprintw(0, 2, "%d", number); // Center the number on the screen
    refresh();

    // Wait for 30 seconds
    for (int i = 5; i > 0; i--) {
        mvprintw(1, 2, "password will disappear in %d seconds...", i);
        refresh();
        sleep(1); // Wait for 1 second
    }

    // Clear the screen and refresh
//    clear();
    move(0, 0);  // Move to line 5, column 0
    clrtoeol();
    move(1, 0);  // Move to line 5, column 0
    clrtoeol();
    refresh();
    // End ncurses mode
    key_pair_sf[pos] = password;
//    endwin();
    return password;
}

int get_part_sf(int y, int x) {
    int part_width = 156 / 3;
    int part_height = 33 / 3;

    int row = (y - 2) / part_height;
    int col = (x - 2) / part_width;

    return row * 3 + col; // 0-based part index
}

int check_key_sf(int pos) {
    int i = 0;   // Wrong attempt count
    while (1) {
        mvprintw(0, 2, "Enter key: ");
        int password;
        scanw("%d", &password);
        int r_key = reverse_number_sf(key_pair_sf[pos]);
        if ((key_pair_sf[pos] != password) && (r_key != password)) {
            if (i == 0) {
                init_pair(1, COLOR_YELLOW, COLOR_BLACK);
                attron(COLOR_PAIR(1));
                mvprintw(1, 2, "warning you entered first wrong password!        ");
                attroff(COLOR_PAIR(1));
            } else if (i == 1) {
                init_pair(2, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(2));
                mvprintw(1, 2, "warning you entered second wrong password!       ");
                attroff(COLOR_PAIR(2));
            } else if (i == 2) {
                for (int i = 10; i > 0; i--) {
                    move(0, 0);  // Move to line 5, column 0
                    clrtoeol();
                    refresh();
                    mvprintw(1, 2, "wait for %d seconds...", i);
                    refresh();
                    sleep(1); // Wait for 1 second
                }      // Exit the program after banning
            }
            i++;
        } else {
            mvprintw(1, 2, "Access granted!                                  ");
            break;
        }
        refresh();
    }

    return 0;
}

int reverse_number_sf(int num) {
    int reversed = 0;

    while (num != 0) {
        int digit = num % 10;         // Extract the last digit
        reversed = reversed * 10 + digit; // Add the digit to the reversed number
        num /= 10;                    // Remove the last digit from the original number
    }

    return reversed;
}

void *change_able_pass_sf() {
    while (1) {
        key_pair_sf[2] = 1000 + rand() % 9000;
        key_pair_sf[5] = 1000 + rand() % 9000;
        key_pair_sf[8] = 1000 + rand() % 9000;
        sleep(5);
    }
    return NULL;
}

void *draw_health_bar_sf() {
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    while (!stop_thread_sf) {
        //  move(0, 0);  // Move to line 5, column 0
        // clrtoeol();
        // refresh();
        //  move(1, 0);  // Move to line 5, column 0
        // clrtoeol();
        //  refresh();
        int width = 50; // Width of the health_sf bar
        int bar_length = (my_game.Health * width) / MAX_HEALTH_sf;
        my_game.Health--;
        // Draw the label and open the health_sf bar
        mvprintw(0, 40, "Health: [");

        // Draw the green part of the health_sf bar
        attron(COLOR_PAIR(1));
        for (int i = 0; i < bar_length; i++) {
            mvaddch(0, 40 + i, ' ');
        }
        attroff(COLOR_PAIR(1));

        // Draw the empty part of the health_sf bar
        for (int i = bar_length; i < width; i++) {
            mvaddch(0, 40 + i, ' ');
        }

        // Close the health_sf bar
        mvaddch(0, 40 + width, ']');

        // Display the current health_sf value below the bar
        mvprintw(1, 40, "Current Health: %d%%", my_game.Health);
        if (health_sf == 0) {
            clear();
            refresh();
            mvprintw(1, 40, "Health finished , you died!");
            exit(1);
        }
        move(y_sf, x_sf);
        refresh();
        sleep(4);
    }
    return NULL;
}

void *increase_health_bar_sf() {
    // تعریف رنگ برای سلامتی
    init_pair(2, COLOR_GREEN, COLOR_GREEN);

    while (!stop_thread_sf_2) {
        // اگر بازیکن گرسنه نباشد و سلامتی کمتر از حداکثر باشد
        if (!is_hungry() && my_game.Health < MAX_HEALTH_sf) {
            my_game.Health += health_increase_sf; // افزایش تدریجی سلامتی

            // رسم مجدد نوار سلامتی
            int width = 50; // عرض نوار سلامتی
            int bar_length = (my_game.Health * width) / MAX_HEALTH_sf;

            mvprintw(0, 40, "Health: [");

            // قسمت سبز نوار سلامتی
            attron(COLOR_PAIR(2));
            for (int i = 0; i < bar_length; i++) {
                mvaddch(0, 40 + i, ' ');
            }
            attroff(COLOR_PAIR(2));

            // قسمت خالی نوار سلامتی
            for (int i = bar_length; i < width; i++) {
                mvaddch(0, 40 + i, ' ');
            }

            // بستن نوار سلامتی
            mvaddch(0, 40 + width, ']');

            // نمایش مقدار سلامتی فعلی زیر نوار
            mvprintw(1, 40, "Current Health: %d%%", my_game.Health);
            move(y_sf, x_sf);
            refresh();
            if (my_game.Health >= 100)
                return NULL;
        }

        sleep(4); // تأخیر برای افزایش تدریجی
    }

    return NULL;
}


