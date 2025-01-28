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
#include "4_floor.c"

char str3[50] = "3";
typedef struct {
    int y;
    int x;
    int size;
} room_tf;

typedef struct {
    char symbol;   // Symbol to display (e.g., 'p')
    bool visible;  // Visibility state
    int number;
} Cell_tf;
room_tf rooms_tf[6];
int health_tf = 101;
int room_num_tf = 0;
int show_map_tf = 0;
#define MAX_HEALTH_tf 100
volatile bool stop_thread_tf = false;
#define MAP_ROWS_tf 36
#define MAP_COLS_tf 160
int room_type_tf = 0;
Cell_tf map_tf[MAP_ROWS_tf][MAP_COLS_tf];
int key_pair_tf[9];
int x_tf = 0, y_tf = 0;
int show_health_bar_tf = 0;

void get_parts_tf(int *rands);

void draw_room_tf();

void *draw_health_bar_tf();

void *change_able_pass_tf();

void update_visibility_tf(int player_y, int player_x);

int rands_tf[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

void fill_room_tf(int start_row, int start_col, int height, int width, int tmp, int tmp2);

void fill_room_out_window_tf(int start_row, int start_col, int height, int width);

void fill_room_out_door_tf(int start_row, int start_col, int height, int width);

void put_corridor_tf(int start_row, int start_col);

int add_file_tf(int row, int col, char character);

void draw_rectangle_tf(int start_row, int start_col, int height, int width);

int reverse_number_tf(int num);

int generate_pass_key_tf(int y, int x);

int check_key_tf(int pos);

void free_map_tf();

int get_part_tf(int y, int x);

void move_to_second_line_tf(FILE *file);

void load_map_from_file_tf();

void draw_map_to_terminal_tf();

void move_to_second_line_tf(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}

void load_map_from_file_tf(char *filename) {
    FILE *file = fopen(str3, "r");
    if (!file) {
        FILE *file = fopen(str3, "w");
        fclose(file);
        file = fopen(str3, "r");
        if (!file) {
            perror("Error opening file after creation");
            return;
        }
    }
    move_to_second_line_tf(file);
    fseek(file, 2, SEEK_CUR);
    if (!file) {
        perror("Error opening file");
        // Initialize map_ff with default values if file doesn't exist
        for (int i = 0; i < MAP_ROWS_tf; i++) {
            memset(map_tf[i], '.', MAP_COLS_tf);
            map_tf[i][MAP_COLS_tf].symbol = '\0';
        }
        return;
    }

    for (int i = 0; i < MAP_ROWS_tf; i++) {
        for (int j = 0; j < MAP_COLS_tf; j++) {
            int ch = fgetc(file); // Read character by character
            if (ch == EOF) {
                map_tf[i][j].symbol = ' '; // Fill remaining cells with dots if EOF is reached
            } else if (ch == '\0') {
                map_tf[i][j].symbol = ' '; // Replace null character with space
            } else if (ch == '\n') {
                map_tf[i][j].symbol = '\n'; // Replace newline with dot (or another filler)
            } else {
                map_tf[i][j].symbol = (char) ch;
            }
            map_tf[i][j].visible = true;
            if ((map_tf[i][j].symbol == '^') || (map_tf[i][j].symbol == '?'))
                map_tf[i][j].visible = false;
        }
    }

    fclose(file);

}


void update_visibility_tf(int player_y, int player_x) {
    if ((map_tf[player_y - 2][player_x].symbol == '?' && !map_tf[player_y - 2][player_x].visible) ||
        (map_tf[player_y - 2][player_x].symbol == '^' && !map_tf[player_y - 2][player_x].visible)) {
        map_tf[player_y - 2][player_x].visible = true; // Make the hidden 'p' visible
    }
}

void draw_map_to_terminal_tf() {
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
    for (int i = 0; i < MAP_ROWS_tf; i++) {
        for (int j = 0; j < MAP_COLS_tf; j++) {
            int c = 0;
            int pos = get_part_tf(i, j);
            int pos2 = get_part_tf(y_tf, x_tf);
            int tmp = 0;
            for (int k = 0; k < pos2; ++k) {
                if (rands_tf[k] == 1)
                    tmp++;
            }
            pos2 = tmp;
            for (int i = 0; i <= pos; ++i) {
                if (rands_tf[i] == 1) {
                    c++;
                }
            }
            if (c % 2 == 1 && rands_tf[pos] == 1) {
                attron(COLOR_PAIR(7));
                show_health_bar_tf = 1;
            }
//            || map_tf[i][j] == '*' || map_tf[i][j] == '=' || map_tf[i][j] == 's' ||
//               map_tf[i][j] == '+'
            if (map_tf[i][j].symbol == '#' || map_tf[i][j].symbol == ' ')
                attroff(COLOR_PAIR(7));
            if (map_tf[i][j].symbol == '@')
                attron(COLOR_PAIR(2));
            if (map_tf[i][j].symbol == 'M')
                attron(COLOR_PAIR(3));
            if (map_tf[i][j].symbol == '&')
                attron(COLOR_PAIR(6));
            if (map_tf[i][j].symbol == 'j')
                attron(COLOR_PAIR(5));
            if (map_tf[i][j].symbol == 'G')
                attron(COLOR_PAIR(4));
            int tar = show_map_tf % 2 == 1;
            if ((tar) ||
                (i >= rooms_tf[pos2].y - 2 && (i <= rooms_tf[pos2].y + rooms_tf[pos2].size - 2) &&
                 j >= rooms_tf[pos2].x &&
                 (j <= rooms_tf[pos2].x + rooms_tf[pos2].size))) {
                if (map_tf[i][j].symbol == '^' && map_tf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_tf[i][j].symbol == '?' && map_tf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_tf[i][j].symbol); }
            } else if ((i <= y_tf + 1) && (i >= y_tf - 5) && (j <= x_tf + 3) && (j >= x_tf - 3)) {
                if (map_tf[i][j].symbol == '^' && map_tf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_tf[i][j].symbol == '?' && map_tf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_tf[i][j].symbol); }
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
            show_health_bar_tf = 0;
            if (map_tf[i][j].symbol == '\n') {

            }
            refresh();// Print each character individually
        }
    }
    move(map_tf[0][0].symbol, map_tf[0][2].symbol);
    refresh();
}


void free_map_tf() {
    // Open the file in write mode to truncate it
    FILE *file = fopen(str3, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // No need to write anything, file is now empty
    fclose(file);  // Close the file

    return;
}


void draw_rectangle_tf(int start_row, int start_col, int height, int width) {
    height++;
    width++;
    rooms_tf[room_num_tf].y = start_row;
    rooms_tf[room_num_tf].x = start_col;
    rooms_tf[room_num_tf].size = height;

    for (int i = start_col; i <= start_col + width; ++i) {
        add_file_tf(start_row, i, '*');
        mvaddch(start_row, i, '*');
        refresh();
        add_file_tf(start_row + height, i, '*');
        mvaddch(start_row + height, i, '*');
        refresh();
    }
    for (int i = start_row; i <= start_row + height; ++i) {
        add_file_tf(i, start_col, '*');
        mvaddch(i, start_col, '*');
        refresh();
        add_file_tf(i, start_col + width, '*');
        mvaddch(i, start_col + width, '*');
        refresh();
    }
    room_num_tf++;
}

void draw_border_tf() {
    free_map_tf();
    add_file_tf(2, 1, '#');
    mvaddch(2, 1, '#');
    for (int i = 2; i < 36; ++i) {
        add_file_tf(i, 2, '#');
        mvaddch(i, 3 - 1, '#');
        refresh();
        add_file_tf(i, 54, '#');
        mvaddch(i, 55 - 1, '#');
        refresh();
        add_file_tf(i, 106, '#');
        mvaddch(i, 107 - 1, '#');
        refresh();
        add_file_tf(i, 158, '#');
        mvaddch(i, 159 - 1, '#');
        refresh();
    }

    for (int i = 2; i < 159; ++i) {
        add_file_tf(2, i, '#');
        mvaddch(3 - 1, i, '#');
        refresh();
        add_file_tf(13, i, '#');
        mvaddch(14 - 1, i, '#');
        refresh();
        add_file_tf(24, i, '#');
        mvaddch(25 - 1, i, '#');
        refresh();
        add_file_tf(35, i, '#');
        mvaddch(36 - 1, i, '#');
        refresh();
    }

}

int third_floor(char *username, int new) {
    int color_pair;
    int ch;
    // Cursor starting position
    srand(time(NULL));
    setlocale(LC_ALL, "");
    initscr();
    clear();
    refresh();
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
    FILE *file = fopen(str3, "r");
    if (!file) {
        strcat(str3, username);
        strcat(str3, ".txt");
    }
    if (new) {
        draw_border_tf();
        get_parts_tf(rands_tf);
        draw_room_tf();
    }
    clear();
    refresh();
    load_map_from_file_tf(username);
    draw_map_to_terminal_tf();
    attron(COLOR_PAIR(color_pair));
    move(y_tf, x_tf);
    char s = mvinch(y_tf, x_tf);
    mvaddch(y_tf, x_tf, s);
    attroff(COLOR_PAIR(color_pair));
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, change_able_pass_tf, NULL);
    while ((ch = getch()) != 'q') {
        if (ch == 'f') {
            ch = getch();
            switch (ch) {
                case KEY_UP:
                    while (y_tf > 0) {
                        y_tf--;
                        if ((mvinch(y_tf, x_tf) & A_CHARTEXT) != '.' && (mvinch(y_tf, x_tf) & A_CHARTEXT) != '#') {
                            y_tf++;
                            break;
                        }
                    }
                    break;
                case KEY_DOWN:
                    while (y_tf < MAP_ROWS_tf - 1) {
                        y_tf++;
                        if ((mvinch(y_tf, x_tf) & A_CHARTEXT) != '.' && (mvinch(y_tf, x_tf) & A_CHARTEXT) != '#') {
                            y_tf--;
                            break;
                        }
                    }
                    break;
                case KEY_LEFT:
                    while (x_tf > 0) {
                        x_tf--;
                        if ((mvinch(y_tf, x_tf) & A_CHARTEXT) != '.' && (mvinch(y_tf, x_tf) & A_CHARTEXT) != '#') {
                            x_tf++;
                            break;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    while (x_tf < MAP_COLS_tf - 1) {
                        x_tf++;
                        if ((mvinch(y_tf, x_tf) & A_CHARTEXT) != '.' && (mvinch(y_tf, x_tf) & A_CHARTEXT) != '#') {
                            x_tf--;
                            break;
                        }
                    }
                    break;
            }
        }
        switch (ch) {
            case 'e':
                clear();
                //eat_food();
                load_map_from_file_tf(username);
                draw_map_to_terminal_tf();
                break;
            case 'j':
                clear();
                display_spell();
                load_map_from_file_tf(username);
                draw_map_to_terminal_tf();
                break;
            case 'i':
                clear();
                display_guns();
                load_map_from_file_tf(username);
                draw_map_to_terminal_tf();
                break;
            case 'v':
                show_map_tf++;
                draw_map_to_terminal_tf();
                break;
            case KEY_UP:
                if (y_tf > 0) y_tf--;
                update_visibility_tf(y_tf, x_tf);
                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == '=') {
                    y_tf++;
                }
                break;
            case KEY_DOWN:
                if (y_tf < MAP_ROWS_tf - 1) y_tf++;
                update_visibility_tf(y_tf, x_tf);
                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == '=') {
                    y_tf--;
                }
                break;
            case KEY_LEFT:
                if (x_tf > 0) x_tf--;
                update_visibility_tf(y_tf, x_tf);
                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == '=') {
                    x_tf++;
                }
                break;
            case KEY_RIGHT:
                if (x_tf < MAP_COLS_tf - 1) x_tf++;
                update_visibility_tf(y_tf, x_tf);

                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == '=') {
                    x_tf--;
                }
                break;
            case 'p':
                if (y_tf > 0) y_tf--;
                if (x_tf < MAP_COLS_tf - 1) x_tf++;
                update_visibility_tf(y_tf, x_tf);

                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o') {
                    y_tf++;
                    x_tf--;
                }
                break;
            case 'w':
                if (y_tf > 0) y_tf--;
                if (x_tf > 0) x_tf--;
                update_visibility_tf(y_tf, x_tf);

                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o') {
                    y_tf++;
                    x_tf++;
                }
                break;
            case ',':
                if (y_tf < MAP_ROWS_tf - 1) y_tf++;
                if (x_tf < MAP_COLS_tf - 1) x_tf++;
                update_visibility_tf(y_tf, x_tf);
                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o') {
                    y_tf--;
                    x_tf--;
                }
                break;
            case 'z':
                if (x_tf > 0) x_tf--;
                if (y_tf < MAP_COLS_tf - 1) y_tf++;
                update_visibility_tf(y_tf, x_tf);

                if ((mvinch(y_tf, x_tf) & A_CHARTEXT) == '*' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'o') {
                    y_tf--;
                    x_tf++;
                }
                break;
            case ' ':
                map_tf[y_tf][x_tf].symbol = (map_tf[y_tf][x_tf].symbol == '.') ? '#' : '.';
                break;
        }
        update_visibility_tf(y_tf, x_tf);
        draw_map_to_terminal_tf();
        if (x_tf < 1) x_tf = 1;
        if (y_tf >= 35) y_tf = 35;
        if (y_tf < 2) y_tf = 2;
        if (x_tf >= 161) x_tf = 161;
        chtype ch = mvinch(y_tf, x_tf);
        chtype attributes = ch & A_ATTRIBUTES;
        int color_pair_s = PAIR_NUMBER(attributes);
        if (color_pair_s == 7) {
            stop_thread_tf = false;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, draw_health_bar_tf, NULL);
        }
        attron(COLOR_PAIR(color_pair));
        move(y_tf, x_tf);
        char s = mvinch(y_tf, x_tf);
        mvaddch(y_tf, x_tf, s);
        attroff(COLOR_PAIR(color_pair));
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == '&')) {
            generate_pass_key_tf(y_tf, x_tf);
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == '<')) {
            stop_thread_tf = true;
            last_floor(username, 1);
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == '^')) {
            health_tf--;
            mvprintw(0, 2, "Your health decreased by a trap!");
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == '@')) {
            int pos = get_part_tf(y_tf, x_tf);
            check_key_tf(pos);
            attron(COLOR_PAIR(3));
            mvaddch(y_tf, x_tf, '@');
            attroff(COLOR_PAIR(3));
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == '#')) {
            stop_thread_tf = true;
        }

        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == 'G') || (mvinch(y_tf, x_tf) & A_CHARTEXT) == '$') {
            int achived_gold = calc_gold((mvinch(y_tf, x_tf) & A_CHARTEXT));
            add_file_tf(y_tf, x_tf, '.');
            mvaddch(y_tf, x_tf, '.');
            mvprintw(1, 2, "you achived %d golds", achived_gold);
            getch();
            load_map_from_file_tf(username);
            draw_map_to_terminal_tf();
            refresh();
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == 'M') || (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'D' ||
            (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'W' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'A' ||
            (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'S') {
            char inp = getch();
            if (inp == 'g') {
                calc_gun(mvinch(y_tf, x_tf) & A_CHARTEXT, map_tf[y_tf - 2][x_tf].number);
                add_file_tf(y_tf, x_tf, '.');
                mvaddch(y_tf, x_tf, '.');
                load_map_from_file_tf(username);
                draw_map_to_terminal_tf();
            }
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == 'h') || (mvinch(y_tf, x_tf) & A_CHARTEXT) == 's' ||
            (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'd') {
            char inp = getch();
            if (inp == 'g') {
                calc_spell(mvinch(y_tf, x_tf) & A_CHARTEXT);
                add_file_tf(y_tf, x_tf, '.');
                mvaddch(y_tf, x_tf, '.');
                load_map_from_file_tf(username);
                draw_map_to_terminal_tf();
            }
        }
        if (((mvinch(y_tf, x_tf) & A_CHARTEXT) == 'n') || (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'm' ||
            (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'b' || (mvinch(y_tf, x_tf) & A_CHARTEXT) == 'c') {
            char inp = getch();
            if (inp == 'g') {
                int food_res = calc_food(mvinch(y_tf, x_tf) & A_CHARTEXT);
                if (food_res) {
                    add_file_tf(y_tf, x_tf, '.');
                    mvaddch(y_tf, x_tf, '.');
                    load_map_from_file_tf(username);
                    draw_map_to_terminal_tf();
                } else {
                    mvprintw(1, 2, "you already have maximum(5) food");
                    getch();
                    load_map_from_file_tf(username);
                    draw_map_to_terminal_tf();
                    refresh();
                }
            }
        }

        attron(COLOR_PAIR(color_pair));
        move(y_tf, x_tf);
        char s1 = mvinch(y_tf, x_tf);
        mvaddch(y_tf, x_tf, s1);
        attroff(COLOR_PAIR(color_pair));
//        getch();
//        mvaddch(10,3,'T');
//        refresh();
    }
    update_game_in_database(username, 3);
    exit(1);
    // save_map_to_file(y_tf,x_tf);

    endwin();
    return 0;
}

void get_parts_tf(int *rands) {
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

void draw_room_tf() {
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
        if (rands_tf[i] == 1) {
            room_type_tf++;
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
            draw_rectangle_tf(start_row, start_col, size, size);
            fill_room_tf(start_row, start_col, size, size, tmp, tmp2);
            fill_room_out_window_tf(start_row, start_col, size, size);
            fill_room_out_door_tf(start_row, start_col, size, size);
            tmp = 0;
            tmp2 = 0;
        }
    }
}

void fill_room_tf(int start_row, int start_col, int height, int width, int tmp, int tmp2) {
    int o_num = rand() % 3;
    for (int i = start_col + 1; i <= start_col + width; ++i) {
        for (int j = start_row + 1; j <= start_row + height; ++j) {
            add_file_tf(j, i, '.');
            if (tmp) {
                y_tf = j;
                x_tf = i;
            }
            mvaddch(j, i, '.');
            refresh();
        }
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'o');
        mvaddch(row, col, 'o');
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, '^');
        mvaddch(row, col, '^');
    }

    if (room_type_tf % 2 != 1) {
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 'd');
            mvaddch(row, col, 'd');
        }
    }
    if (room_type_tf % 2 == 1) {
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_tf(row, col, 'd');
            mvaddch(row, col, 'd');
        }

    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'G');
        mvaddch(row, col, 'G');
    }
    int r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, '$');
        mvaddch(row, col, '$');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'M');
        mvaddch(row, col, 'M');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'D');
        mvaddch(row, col, 'D');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'W');
        mvaddch(row, col, 'W');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'A');
        mvaddch(row, col, 'A');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'S');
        mvaddch(row, col, 'S');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'n');
        mvaddch(row, col, 'n');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'm');
        mvaddch(row, col, 'm');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'b');
        mvaddch(row, col, 'b');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, 'c');
        mvaddch(row, col, 'c');
    }
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_tf(row, col, '<');
        mvaddch(row, col, '<');
    }
}

void fill_room_out_window_tf(int start_row, int start_col, int height, int width) {
    int index = rand() % (2 * (height + 1 + width + 1));
    if (index < width + 1) {
        add_file_tf(start_row, start_col + index, '=');
        mvaddch(start_row, start_col + index, '=');
    } else if (width < index && index < width + height + 2) {
        index -= (width + 1);
        add_file_tf(start_row + index, start_col + width + 1, '=');
        mvaddch(start_row + index, start_col + width + 1, '=');
    } else if (width + height + 1 < index && index < width + height + width + 3) {
        index -= (width + height + 2);
        add_file_tf(start_row + height + 1, start_col + width + 1 - index, '=');
        mvaddch(start_row + height + 1, start_col + width + 1 - index, '=');
    } else {
        index -= (width + height + width + 3);
        add_file_tf(start_row + height + 1 - index, start_col, '=');
        mvaddch(start_row + height + 1 - index, start_col, '=');
    }
}

void fill_room_out_door_tf(int start_row, int start_col, int height, int width) {
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
            add_file_tf(start_row, start_col + index, '?');
            mvaddch(start_row, start_col + index, '?');
            put_corridor_tf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_tf(start_row + index, start_col + width + 1, '?');
            mvaddch(start_row + index, start_col + width + 1, '?');
            put_corridor_tf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_tf(start_row + height + 1, start_col + width + 1 - index, '?');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '?');
            put_corridor_tf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_tf(start_row + height + 1 - index, start_col, '?');
            mvaddch(start_row + height + 1 - index, start_col, '?');
            put_corridor_tf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat; ++i) {
        int index = rand() % (2 * (height + 1 + width + 1));
        if (index < width + 1) {
            add_file_tf(start_row, start_col + index, '+');
            mvaddch(start_row, start_col + index, '+');
            put_corridor_tf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_tf(start_row + index, start_col + width + 1, '+');
            mvaddch(start_row + index, start_col + width + 1, '+');
            put_corridor_tf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_tf(start_row + height + 1, start_col + width + 1 - index, '+');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '+');
            put_corridor_tf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_tf(start_row + height + 1 - index, start_col, '+');
            mvaddch(start_row + height + 1 - index, start_col, '+');
            put_corridor_tf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat - 1; ++i) {
        if (index < width + 1) {
            add_file_tf(start_row, start_col + index, '@');
            mvaddch(start_row, start_col + index, '@');
            put_corridor_tf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_tf(start_row + index, start_col + width + 1, '@');
            mvaddch(start_row + index, start_col + width + 1, '@');
            put_corridor_tf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_tf(start_row + height + 1, start_col + width + 1 - index, '@');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '@');
            put_corridor_tf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_tf(start_row + height + 1 - index, start_col, '@');
            mvaddch(start_row + height + 1 - index, start_col, '@');
            put_corridor_tf(start_row + height + 1 - index, start_col);
        }
    }
    if (repeat == 2) {
        start_row++;
        start_col++;
        width -= 2;
        height -= 2;
        if (index < width + 1) {
            add_file_tf(start_row, start_col, '&');
            mvaddch(start_row, start_col, '&');
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_tf(start_row, start_col + width + 1, '&');
            mvaddch(start_row, start_col + width + 1, '&');
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_tf(start_row + height + 1, start_col + width + 1, '&');
            mvaddch(start_row + height + 1, start_col + width + 1, '&');
        } else {
            index -= (width + height + width + 3);
            add_file_tf(start_row + height + 1, start_col, '&');
            mvaddch(start_row + height + 1, start_col, '&');
        }
    }
}

void put_corridor_tf(int start_row, int start_col) {
    int flag = 0;
    while ((mvinch(start_row + 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_tf(start_row + 1, start_col, '#');
        mvaddch(start_row + 1, start_col, '#');
        start_row++;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col + 1) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_tf(start_row, start_col + 1, '#');
        mvaddch(start_row, start_col + 1, '#');
        start_col++;
    }
    if (flag)
        return;
    while ((mvinch(start_row - 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_tf(start_row - 1, start_col, '#');
        mvaddch(start_row - 1, start_col, '#');
        start_row--;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col - 1) & A_CHARTEXT) == ' ') {
        add_file_tf(start_row, start_col - 1, '#');
        mvaddch(start_row, start_col - 1, '#');
        start_col--;
        if (start_col == 3) {
            add_file_tf(start_row, 2, '#');
            mvaddch(start_row, 2, '#');
        }
    }

}


#include <stdio.h>

int add_file_tf(int row, int col, char character) {
    FILE *file;

    file = fopen(str3, "r+");
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

int generate_pass_key_tf(int y, int x) {
    int pos = get_part_tf(y, x);
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
    key_pair_tf[pos] = password;
//    endwin();
    return password;
}

int get_part_tf(int y, int x) {
    int part_width = 156 / 3;
    int part_height = 33 / 3;

    int row = (y - 2) / part_height;
    int col = (x - 2) / part_width;

    return row * 3 + col; // 0-based part index
}

int check_key_tf(int pos) {
    int i = 0;   // Wrong attempt count
    while (1) {
        mvprintw(0, 2, "Enter key: ");
        int password;
        scanw("%d", &password);
        int r_key = reverse_number_tf(key_pair_tf[pos]);
        if ((key_pair_tf[pos] != password) && (r_key != password)) {
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

int reverse_number_tf(int num) {
    int reversed = 0;

    while (num != 0) {
        int digit = num % 10;         // Extract the last digit
        reversed = reversed * 10 + digit; // Add the digit to the reversed number
        num /= 10;                    // Remove the last digit from the original number
    }

    return reversed;
}

void *change_able_pass_tf() {
    while (1) {
        key_pair_tf[2] = 1000 + rand() % 9000;
        key_pair_tf[5] = 1000 + rand() % 9000;
        key_pair_tf[8] = 1000 + rand() % 9000;
        sleep(5);
    }
    return NULL;
}

void *draw_health_bar_tf() {
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    while (!stop_thread_tf) {
        //  move(0, 0);  // Move to line 5, column 0
        // clrtoeol();
        // refresh();
        //  move(1, 0);  // Move to line 5, column 0
        // clrtoeol();
        //  refresh();
        int width = 50; // Width of the health_tf bar
        int bar_length = (health_tf * width) / MAX_HEALTH_tf;
        health_tf--;
        // Draw the label and open the health_tf bar
        mvprintw(0, 2, "Health: [");

        // Draw the green part of the health_tf bar
        attron(COLOR_PAIR(1));
        for (int i = 0; i < bar_length; i++) {
            mvaddch(0, 10 + i, ' ');
        }
        attroff(COLOR_PAIR(1));

        // Draw the empty part of the health_tf bar
        for (int i = bar_length; i < width; i++) {
            mvaddch(0, 10 + i, ' ');
        }

        // Close the health_tf bar
        mvaddch(0, 10 + width, ']');

        // Display the current health_tf value below the bar
        mvprintw(1, 2, "Current Health: %d%%", health_tf);
        if (health_tf == 0) {
            clear();
            refresh();
            mvprintw(1, 2, "Health finished , you died!");
            exit(1);
        }
        move(y_tf, x_tf);
        refresh();
        sleep(1);
    }
    return NULL;
}


