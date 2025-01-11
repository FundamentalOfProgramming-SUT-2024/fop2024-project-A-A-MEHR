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
#include "game.c"
#include <locale.h>

char str4[50] = "4";
typedef struct {
    int y;
    int x;
    int size;
} room_lf;

typedef struct {
    char symbol;   // Symbol to display (e.g., 'p')
    bool visible;  // Visibility state
} Cell_lf;
room_lf rooms_lf[6];
int health_lf = 101;
int room_num_lf = 0;
int show_map_lf = 0;
#define MAX_HEALTH_lf 100
volatile bool stop_thread_lf = false;
#define MAP_ROWS_lf 36
#define MAP_COLS_lf 160
int room_type_lf = 0;
int treasure = 1;
Cell_lf map_lf[MAP_ROWS_lf][MAP_COLS_lf];
int key_pair_lf[9];
int x_lf = 0, y_lf = 0;
int show_health_bar_lf = 0;

void get_parts_lf(int *rands);

void draw_room_lf();

void *draw_health_bar_lf();

void *change_able_pass_lf();

void update_visibility_lf(int player_y, int player_x);

int rands_lf[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

void fill_room_lf(int start_row, int start_col, int height, int width, int tmp, int tmp2);

void fill_room_out_window_lf(int start_row, int start_col, int height, int width);

void fill_room_out_door_lf(int start_row, int start_col, int height, int width);

void put_corridor_lf(int start_row, int start_col);

int add_file_lf(int row, int col, char character);

void draw_rectangle_lf(int start_row, int start_col, int height, int width);

int reverse_number_lf(int num);

int generate_pass_key_lf(int y, int x);

int check_key_lf(int pos);

void free_map_lf();

int get_part_lf(int y, int x);

void move_to_second_line_lf(FILE *file);

void load_map_from_file_lf();

void draw_map_to_terminal_lf();

void move_to_second_line_lf(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}

void load_map_from_file_lf(char *filename) {
    FILE *file = fopen(str4, "r");
    if (!file) {
        FILE *file = fopen(str4, "w");
        fclose(file);
        file = fopen(str4, "r");
        if (!file) {
            perror("Error opening file after creation");
            return;
        }
    }
    move_to_second_line_lf(file);
    fseek(file, 2, SEEK_CUR);
    if (!file) {
        perror("Error opening file");
        // Initialize map_ff with default values if file doesn't exist
        for (int i = 0; i < MAP_ROWS_lf; i++) {
            memset(map_lf[i], '.', MAP_COLS_lf);
            map_lf[i][MAP_COLS_lf].symbol = '\0';
        }
        return;
    }

    for (int i = 0; i < MAP_ROWS_lf; i++) {
        for (int j = 0; j < MAP_COLS_lf; j++) {
            int ch = fgetc(file); // Read character by character
            if (ch == EOF) {
                map_lf[i][j].symbol = ' '; // Fill remaining cells with dots if EOF is reached
            } else if (ch == '\0') {
                map_lf[i][j].symbol = ' '; // Replace null character with space
            } else if (ch == '\n') {
                map_lf[i][j].symbol = '\n'; // Replace newline with dot (or another filler)
            } else {
                map_lf[i][j].symbol = (char) ch;
            }
            map_lf[i][j].visible = true;
            if ((map_lf[i][j].symbol == '^') || (map_lf[i][j].symbol == '?'))
                map_lf[i][j].visible = false;
        }
    }

    fclose(file);
}


void update_visibility_lf(int player_y, int player_x) {
    if ((map_lf[player_y - 2][player_x].symbol == '?' && !map_lf[player_y - 2][player_x].visible) ||
        (map_lf[player_y - 2][player_x].symbol == '^' && !map_lf[player_y - 2][player_x].visible)) {
        map_lf[player_y - 2][player_x].visible = true; // Make the hidden 'p' visible
    }
}

void draw_map_to_terminal_lf() {
    int first = 0;
    for (int i = 0; i < 5; ++i) {
        if (rands_lf[i] == 1) {
            first = i;
            break;
        }
    }
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
        init_pair(8, COLOR_WHITE, COLOR_YELLOW);
    }// Clear the screen
    for (int i = 0; i < MAP_ROWS_lf; i++) {
        for (int j = 0; j < MAP_COLS_lf; j++) {
            int c = 0;
            int pos = get_part_lf(i, j);
            int pos2 = get_part_lf(y_lf, x_lf);
            int tmp = 0;
            for (int k = 0; k < pos2; ++k) {
                if (rands_lf[k] == 1)
                    tmp++;
            }
            pos2 = tmp;
            for (int i = 0; i <= pos; ++i) {
                if (rands_lf[i] == 1) {
                    c++;
                }
            }
            if (first == pos) {
                attron(COLOR_PAIR(8));
            } else if (c % 2 == 1 && rands_lf[pos] == 1) {
                attron(COLOR_PAIR(7));
                show_health_bar_lf = 1;
            }
//            || map_lf[i][j] == '*' || map_lf[i][j] == '=' || map_lf[i][j] == 's' ||
//               map_lf[i][j] == '+'
            if (map_lf[i][j].symbol == '#' || map_lf[i][j].symbol == ' ')
                attroff(COLOR_PAIR(7));
            if (map_lf[i][j].symbol == '@')
                attron(COLOR_PAIR(2));
            if (map_lf[i][j].symbol == 'M')
                attron(COLOR_PAIR(3));
            if (map_lf[i][j].symbol == '&')
                attron(COLOR_PAIR(6));
            if (map_lf[i][j].symbol == 'j')
                attron(COLOR_PAIR(5));
            if (map_lf[i][j].symbol == 'G')
                attron(COLOR_PAIR(4));
            int tar = show_map_lf % 2 == 1;
            if ((tar) ||
                (i >= rooms_lf[pos2].y - 2 && (i <= rooms_lf[pos2].y + rooms_lf[pos2].size - 2) &&
                 j >= rooms_lf[pos2].x &&
                 (j <= rooms_lf[pos2].x + rooms_lf[pos2].size))) {
                if (map_lf[i][j].symbol == '^' && map_lf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_lf[i][j].symbol == '?' && map_lf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_lf[i][j].symbol); }
            } else if ((i <= y_lf + 1) && (i >= y_lf - 5) && (j <= x_lf + 3) && (j >= x_lf - 3)) {
                if (map_lf[i][j].symbol == '^' && map_lf[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map_lf[i][j].symbol == '?' && map_lf[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map_lf[i][j].symbol); }
            } else {
                attroff(COLOR_PAIR(2));
                attroff(COLOR_PAIR(6));
                attroff(COLOR_PAIR(3));
                attroff(COLOR_PAIR(5));
                attroff(COLOR_PAIR(4));
                attroff(COLOR_PAIR(7));
                attroff(COLOR_PAIR(8));
                mvaddch(i + 2, j, ' ');
            }
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(6));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(5));
            attroff(COLOR_PAIR(4));
            attroff(COLOR_PAIR(7));
            attroff(COLOR_PAIR(8));
            show_health_bar_lf = 0;
            if (map_lf[i][j].symbol == '\n') {

            }
            refresh();// Print each character individually
        }
    }
    move(map_lf[0][0].symbol, map_lf[0][2].symbol);
    refresh();
}


void free_map_lf() {
    // Open the file in write mode to truncate it
    FILE *file = fopen(str4, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // No need to write anything, file is now empty
    fclose(file);  // Close the file

    return;
}


void draw_rectangle_lf(int start_row, int start_col, int height, int width) {
    height++;
    width++;
    rooms_lf[room_num_lf].y = start_row;
    rooms_lf[room_num_lf].x = start_col;
    rooms_lf[room_num_lf].size = height;

    for (int i = start_col; i <= start_col + width; ++i) {
        add_file_lf(start_row, i, '*');
        mvaddch(start_row, i, '*');
        refresh();
        add_file_lf(start_row + height, i, '*');
        mvaddch(start_row + height, i, '*');
        refresh();
    }
    for (int i = start_row; i <= start_row + height; ++i) {
        add_file_lf(i, start_col, '*');
        mvaddch(i, start_col, '*');
        refresh();
        add_file_lf(i, start_col + width, '*');
        mvaddch(i, start_col + width, '*');
        refresh();
    }
    room_num_lf++;
}

void draw_border_lf() {
    free_map_lf();
    add_file_lf(2, 1, '#');
    mvaddch(2, 1, '#');
    for (int i = 2; i < 36; ++i) {
        add_file_lf(i, 2, '#');
        mvaddch(i, 3 - 1, '#');
        refresh();
        add_file_lf(i, 54, '#');
        mvaddch(i, 55 - 1, '#');
        refresh();
        add_file_lf(i, 106, '#');
        mvaddch(i, 107 - 1, '#');
        refresh();
        add_file_lf(i, 158, '#');
        mvaddch(i, 159 - 1, '#');
        refresh();
    }

    for (int i = 2; i < 159; ++i) {
        add_file_lf(2, i, '#');
        mvaddch(3 - 1, i, '#');
        refresh();
        add_file_lf(13, i, '#');
        mvaddch(14 - 1, i, '#');
        refresh();
        add_file_lf(24, i, '#');
        mvaddch(25 - 1, i, '#');
        refresh();
        add_file_lf(35, i, '#');
        mvaddch(36 - 1, i, '#');
        refresh();
    }

}

int last_floor(char *username, int new) {

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
    curs_set(1);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_GREEN);

    }
    FILE *file = fopen(str4, "r");
    if (!file) {
        strcat(str4, username);
        strcat(str4, ".txt");
    }
    if (new) {
        draw_border_lf();
        get_parts_lf(rands_lf);
        draw_room_lf();
    }
    clear();
    refresh();
    load_map_from_file_lf(username);
    draw_map_to_terminal_lf();
    move(y_lf, x_lf);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, change_able_pass_lf, NULL);
    while ((ch = getch()) != 'q') {
        if (ch == 'f') {
            ch = getch();
            switch (ch) {
                case KEY_UP:
                    while (y_lf > 0) {
                        y_lf--;
                        if ((mvinch(y_lf, x_lf) & A_CHARTEXT) != '.' && (mvinch(y_lf, x_lf) & A_CHARTEXT) != '#') {
                            y_lf++;
                            break;
                        }
                    }
                    break;
                case KEY_DOWN:
                    while (y_lf < MAP_ROWS_lf - 1) {
                        y_lf++;
                        if ((mvinch(y_lf, x_lf) & A_CHARTEXT) != '.' && (mvinch(y_lf, x_lf) & A_CHARTEXT) != '#') {
                            y_lf--;
                            break;
                        }
                    }
                    break;
                case KEY_LEFT:
                    while (x_lf > 0) {
                        x_lf--;
                        if ((mvinch(y_lf, x_lf) & A_CHARTEXT) != '.' && (mvinch(y_lf, x_lf) & A_CHARTEXT) != '#') {
                            x_lf++;
                            break;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    while (x_lf < MAP_COLS_lf - 1) {
                        x_lf++;
                        if ((mvinch(y_lf, x_lf) & A_CHARTEXT) != '.' && (mvinch(y_lf, x_lf) & A_CHARTEXT) != '#') {
                            x_lf--;
                            break;
                        }
                    }
                    break;
            }
        }
        switch (ch) {
            case 'e':
                clear();
                eat_food();
                load_map_from_file_lf(username);
                draw_map_to_terminal_lf();
                break;
            case 'j':
                clear();
                display_spell();
                load_map_from_file_lf(username);
                draw_map_to_terminal_lf();
                break;
            case 'i':
                clear();
                display_guns();
                load_map_from_file_lf(username);
                draw_map_to_terminal_lf();
                break;
            case 'v':
                show_map_lf++;
                draw_map_to_terminal_lf();
                break;
            case KEY_UP:
                if (y_lf > 0) y_lf--;
                update_visibility_lf(y_lf, x_lf);
                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == '=') {
                    y_lf++;
                }
                break;
            case KEY_DOWN:
                if (y_lf < MAP_ROWS_lf - 1) y_lf++;
                update_visibility_lf(y_lf, x_lf);
                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == '=') {
                    y_lf--;
                }
                break;
            case KEY_LEFT:
                if (x_lf > 0) x_lf--;
                update_visibility_lf(y_lf, x_lf);
                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == '=') {
                    x_lf++;
                }
                break;
            case KEY_RIGHT:
                if (x_lf < MAP_COLS_lf - 1) x_lf++;
                update_visibility_lf(y_lf, x_lf);

                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == '=') {
                    x_lf--;
                }
                break;
            case 'p':
                if (y_lf > 0) y_lf--;
                if (x_lf < MAP_COLS_lf - 1) x_lf++;
                update_visibility_lf(y_lf, x_lf);

                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o') {
                    y_lf++;
                    x_lf--;
                }
                break;
            case 'w':
                if (y_lf > 0) y_lf--;
                if (x_lf > 0) x_lf--;
                update_visibility_lf(y_lf, x_lf);

                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o') {
                    y_lf++;
                    x_lf++;
                }
                break;
            case ',':
                if (y_lf < MAP_ROWS_lf - 1) y_lf++;
                if (x_lf < MAP_COLS_lf - 1) x_lf++;
                update_visibility_lf(y_lf, x_lf);
                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o') {
                    y_lf--;
                    x_lf--;
                }
                break;
            case 'z':
                if (x_lf > 0) x_lf--;
                if (y_lf < MAP_COLS_lf - 1) y_lf++;
                update_visibility_lf(y_lf, x_lf);

                if ((mvinch(y_lf, x_lf) & A_CHARTEXT) == '*' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == ' ' ||
                    (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'o') {
                    y_lf--;
                    x_lf++;
                }
                break;
            case ' ':
                map_lf[y_lf][x_lf].symbol = (map_lf[y_lf][x_lf].symbol == '.') ? '#' : '.';
                break;
        }
        update_visibility_lf(y_lf, x_lf);
        draw_map_to_terminal_lf();
        if (x_lf < 1) x_lf = 1;
        if (y_lf >= 35) y_lf = 35;
        if (y_lf < 2) y_lf = 2;
        if (x_lf >= 161) x_lf = 161;
        move(y_lf, x_lf);
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == '&')) {
            generate_pass_key_lf(y_lf, x_lf);
        }
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == '@')) {
            int pos = get_part_lf(y_lf, x_lf);
            check_key_lf(pos);
            attron(COLOR_PAIR(3));
            mvaddch(y_lf, x_lf, '@');
            attroff(COLOR_PAIR(3));
        }
        chtype ch = mvinch(y_lf, x_lf);
        chtype attributes = ch & A_ATTRIBUTES;
        int color_pair = PAIR_NUMBER(attributes);
        if (color_pair == 7) {
            stop_thread_lf = false;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, draw_health_bar_lf, NULL);
        }
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == '#')) {
            stop_thread_lf = true;
        }

        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == 'G') || (mvinch(y_lf, x_lf) & A_CHARTEXT) == '$') {
            int achived_gold = calc_gold((mvinch(y_lf, x_lf) & A_CHARTEXT));
            add_file_lf(y_lf, x_lf, '.');
            mvaddch(y_lf, x_lf, '.');
            mvprintw(1, 2, "you achived %d golds", achived_gold);
            getch();
            load_map_from_file_lf(username);
            draw_map_to_terminal_lf();
            refresh();
        }
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == 'M') || (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'D' ||
            (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'W' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'A' ||
            (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'S') {
            char inp = getch();
            if (inp == 'g') {
                calc_gun(mvinch(y_lf, x_lf) & A_CHARTEXT);
                add_file_lf(y_lf, x_lf, '.');
                mvaddch(y_lf, x_lf, '.');
                load_map_from_file_lf(username);
                draw_map_to_terminal_lf();
            }
        }
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == 'h') || (mvinch(y_lf, x_lf) & A_CHARTEXT) == 's' ||
            (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'd') {
            char inp = getch();
            if (inp == 'g') {
                calc_spell(mvinch(y_lf, x_lf) & A_CHARTEXT);
                add_file_lf(y_lf, x_lf, '.');
                mvaddch(y_lf, x_lf, '.');
                load_map_from_file_lf(username);
                draw_map_to_terminal_lf();
            }
        }
        if (((mvinch(y_lf, x_lf) & A_CHARTEXT) == 'n') || (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'm' ||
            (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'b' || (mvinch(y_lf, x_lf) & A_CHARTEXT) == 'c') {
            char inp = getch();
            if (inp == 'g') {
                int food_res = calc_food(mvinch(y_lf, x_lf) & A_CHARTEXT);
                if (food_res) {
                    add_file_lf(y_lf, x_lf, '.');
                    mvaddch(y_lf, x_lf, '.');
                    load_map_from_file_lf(username);
                    draw_map_to_terminal_lf();
                } else {
                    mvprintw(1, 2, "you already have maximum(5) food");
                    getch();
                    load_map_from_file_lf(username);
                    draw_map_to_terminal_lf();
                    refresh();
                }
            }
        }

        move(y_lf, x_lf);
//        getch();
//        mvaddch(10,3,'T');
//        refresh();
    }
    update_game_in_database(username, 4);
    exit(1);

    // save_map_to_file(y_lf,x_lf);

    endwin();
    return 0;
}

void get_parts_lf(int *rands) {
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

void draw_room_lf() {
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
        if (rands_lf[i] == 1) {
            room_type_lf++;
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
            draw_rectangle_lf(start_row, start_col, size, size);
            fill_room_lf(start_row, start_col, size, size, tmp, tmp2);
            fill_room_out_window_lf(start_row, start_col, size, size);
            fill_room_out_door_lf(start_row, start_col, size, size);
            tmp = 0;
            tmp2 = 0;
        }
    }
}

void fill_room_lf(int start_row, int start_col, int height, int width, int tmp, int tmp2) {
    int o_num = rand() % 3;
    for (int i = start_col + 1; i <= start_col + width; ++i) {
        for (int j = start_row + 1; j <= start_row + height; ++j) {
            add_file_lf(j, i, '.');
            if (tmp) {
                y_lf = j;
                x_lf = i;
            }
            mvaddch(j, i, '.');
            refresh();
        }
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'o');
        mvaddch(row, col, 'o');
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, '^');
        mvaddch(row, col, '^');
    }

    if (room_type_lf % 2 != 1) {
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'd');
            mvaddch(row, col, 'd');
        }
    }
    if (room_type_lf % 2 == 1 && !treasure) {
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'd');
            mvaddch(row, col, 'd');
        }
    }
    if (room_type_lf % 2 == 1 && treasure) {
        treasure = 0;
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'h');
            mvaddch(row, col, 'h');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 's');
            mvaddch(row, col, 's');
        }
        for (int i = 0; i < o_num + 1; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, 'd');
            mvaddch(row, col, 'd');
        }
        for (int i = 0; i < o_num + 10; ++i) {
            int row = rand() % (height - 1) + start_row + 1;
            int col = rand() % (width - 1) + start_col + 1;
            add_file_lf(row, col, '^');
            mvaddch(row, col, '^');
        }

    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'G');
        mvaddch(row, col, 'G');
    }
    int r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, '$');
        mvaddch(row, col, '$');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'M');
        mvaddch(row, col, 'M');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'D');
        mvaddch(row, col, 'D');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'W');
        mvaddch(row, col, 'W');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'A');
        mvaddch(row, col, 'A');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'S');
        mvaddch(row, col, 'S');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'n');
        mvaddch(row, col, 'n');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'm');
        mvaddch(row, col, 'm');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'b');
        mvaddch(row, col, 'b');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, 'c');
        mvaddch(row, col, 'c');
    }
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_lf(row, col, '<');
        mvaddch(row, col, '<');
    }
}

void fill_room_out_window_lf(int start_row, int start_col, int height, int width) {
    int index = rand() % (2 * (height + 1 + width + 1));
    if (index < width + 1) {
        add_file_lf(start_row, start_col + index, '=');
        mvaddch(start_row, start_col + index, '=');
    } else if (width < index && index < width + height + 2) {
        index -= (width + 1);
        add_file_lf(start_row + index, start_col + width + 1, '=');
        mvaddch(start_row + index, start_col + width + 1, '=');
    } else if (width + height + 1 < index && index < width + height + width + 3) {
        index -= (width + height + 2);
        add_file_lf(start_row + height + 1, start_col + width + 1 - index, '=');
        mvaddch(start_row + height + 1, start_col + width + 1 - index, '=');
    } else {
        index -= (width + height + width + 3);
        add_file_lf(start_row + height + 1 - index, start_col, '=');
        mvaddch(start_row + height + 1 - index, start_col, '=');
    }
}

void fill_room_out_door_lf(int start_row, int start_col, int height, int width) {
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
            add_file_lf(start_row, start_col + index, '?');
            mvaddch(start_row, start_col + index, '?');
            put_corridor_lf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_lf(start_row + index, start_col + width + 1, '?');
            mvaddch(start_row + index, start_col + width + 1, '?');
            put_corridor_lf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_lf(start_row + height + 1, start_col + width + 1 - index, '?');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '?');
            put_corridor_lf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_lf(start_row + height + 1 - index, start_col, '?');
            mvaddch(start_row + height + 1 - index, start_col, '?');
            put_corridor_lf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat; ++i) {
        int index = rand() % (2 * (height + 1 + width + 1));
        if (index < width + 1) {
            add_file_lf(start_row, start_col + index, '+');
            mvaddch(start_row, start_col + index, '+');
            put_corridor_lf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_lf(start_row + index, start_col + width + 1, '+');
            mvaddch(start_row + index, start_col + width + 1, '+');
            put_corridor_lf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_lf(start_row + height + 1, start_col + width + 1 - index, '+');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '+');
            put_corridor_lf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_lf(start_row + height + 1 - index, start_col, '+');
            mvaddch(start_row + height + 1 - index, start_col, '+');
            put_corridor_lf(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat - 1; ++i) {
        if (index < width + 1) {
            add_file_lf(start_row, start_col + index, '@');
            mvaddch(start_row, start_col + index, '@');
            put_corridor_lf(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_lf(start_row + index, start_col + width + 1, '@');
            mvaddch(start_row + index, start_col + width + 1, '@');
            put_corridor_lf(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_lf(start_row + height + 1, start_col + width + 1 - index, '@');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '@');
            put_corridor_lf(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file_lf(start_row + height + 1 - index, start_col, '@');
            mvaddch(start_row + height + 1 - index, start_col, '@');
            put_corridor_lf(start_row + height + 1 - index, start_col);
        }
    }
    if (repeat == 2) {
        start_row++;
        start_col++;
        width -= 2;
        height -= 2;
        if (index < width + 1) {
            add_file_lf(start_row, start_col, '&');
            mvaddch(start_row, start_col, '&');
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file_lf(start_row, start_col + width + 1, '&');
            mvaddch(start_row, start_col + width + 1, '&');
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file_lf(start_row + height + 1, start_col + width + 1, '&');
            mvaddch(start_row + height + 1, start_col + width + 1, '&');
        } else {
            index -= (width + height + width + 3);
            add_file_lf(start_row + height + 1, start_col, '&');
            mvaddch(start_row + height + 1, start_col, '&');
        }
    }
}

void put_corridor_lf(int start_row, int start_col) {
    int flag = 0;
    while ((mvinch(start_row + 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_lf(start_row + 1, start_col, '#');
        mvaddch(start_row + 1, start_col, '#');
        start_row++;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col + 1) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_lf(start_row, start_col + 1, '#');
        mvaddch(start_row, start_col + 1, '#');
        start_col++;
    }
    if (flag)
        return;
    while ((mvinch(start_row - 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file_lf(start_row - 1, start_col, '#');
        mvaddch(start_row - 1, start_col, '#');
        start_row--;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col - 1) & A_CHARTEXT) == ' ') {
        add_file_lf(start_row, start_col - 1, '#');
        mvaddch(start_row, start_col - 1, '#');
        start_col--;
        if (start_col == 3) {
            add_file_lf(start_row, 2, '#');
            mvaddch(start_row, 2, '#');
        }
    }

}


#include <stdio.h>

int add_file_lf(int row, int col, char character) {
    FILE *file;

    file = fopen(str4, "r+");
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

int generate_pass_key_lf(int y, int x) {
    int pos = get_part_lf(y, x);
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
    key_pair_lf[pos] = password;
//    endwin();
    return password;
}

int get_part_lf(int y, int x) {
    int part_width = 156 / 3;
    int part_height = 33 / 3;

    int row = (y - 2) / part_height;
    int col = (x - 2) / part_width;

    return row * 3 + col; // 0-based part index
}

int check_key_lf(int pos) {
    int i = 0;   // Wrong attempt count
    while (1) {
        mvprintw(0, 2, "Enter key: ");
        int password;
        scanw("%d", &password);
        int r_key = reverse_number_lf(key_pair_lf[pos]);
        if ((key_pair_lf[pos] != password) && (r_key != password)) {
            if (i == 0) {
                mvprintw(1, 2, "warning you entered first wrong password!        ");
            } else if (i == 1) {
                mvprintw(1, 2, "warning you entered second wrong password!       ");
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

int reverse_number_lf(int num) {
    int reversed = 0;

    while (num != 0) {
        int digit = num % 10;         // Extract the last digit
        reversed = reversed * 10 + digit; // Add the digit to the reversed number
        num /= 10;                    // Remove the last digit from the original number
    }

    return reversed;
}

void *change_able_pass_lf() {
    while (1) {
        key_pair_lf[2] = 1000 + rand() % 9000;
        key_pair_lf[5] = 1000 + rand() % 9000;
        key_pair_lf[8] = 1000 + rand() % 9000;
        sleep(5);
    }
    return NULL;
}

void *draw_health_bar_lf() {
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    while (!stop_thread_lf) {
        //  move(0, 0);  // Move to line 5, column 0
        // clrtoeol();
        // refresh();
        //  move(1, 0);  // Move to line 5, column 0
        // clrtoeol();
        //  refresh();
        int width = 50; // Width of the health_lf bar
        int bar_length = (health_lf * width) / MAX_HEALTH_lf;
        health_lf--;
        // Draw the label and open the health_lf bar
        mvprintw(0, 2, "Health: [");

        // Draw the green part of the health_lf bar
        attron(COLOR_PAIR(1));
        for (int i = 0; i < bar_length; i++) {
            mvaddch(0, 10 + i, ' ');
        }
        attroff(COLOR_PAIR(1));

        // Draw the empty part of the health_lf bar
        for (int i = bar_length; i < width; i++) {
            mvaddch(0, 10 + i, ' ');
        }

        // Close the health_lf bar
        mvaddch(0, 10 + width, ']');

        // Display the current health_lf value below the bar
        mvprintw(1, 2, "Current Health: %d%%", health_lf);
        move(y_lf, x_lf);
        refresh();
        sleep(1);
    }
    return NULL;
}


