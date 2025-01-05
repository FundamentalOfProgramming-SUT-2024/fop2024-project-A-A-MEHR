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

typedef struct {
    int y;
    int x;
    int size;
} room;

typedef struct {
    char symbol;   // Symbol to display (e.g., 'p')
    bool visible;  // Visibility state
} Cell;
room rooms[6];
int health=101;
int room_num = 0;
int show_map = 0;
#define MAX_HEALTH 100
volatile bool stop_thread = false;
#define MAP_ROWS 36
#define MAP_COLS 160
int room_type = 0;
Cell map[MAP_ROWS][MAP_COLS];
int key_pair[9];
int DELAY = 100000;
int x = 0, y = 0;
int show_health_bar = 0;

void get_parts_sf(int *rands);

void draw_room_sf();

void *draw_health_bar();

void *change_able_pass_sf();

void update_visibility(int player_y, int player_x);

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

void move_to_second_line_sf(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}

void load_map_from_file_sf() {
    FILE *file = fopen("example.txt", "r");
    move_to_second_line_sf(file);
    fseek(file, 2, SEEK_CUR);
    if (!file) {
        perror("Error opening file");
        // Initialize map with default values if file doesn't exist
        for (int i = 0; i < MAP_ROWS; i++) {
            memset(map[i], '.', MAP_COLS);
            map[i][MAP_COLS].symbol = '\0';
        }
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            int ch = fgetc(file); // Read character by character
            if (ch == EOF) {
                map[i][j].symbol = ' '; // Fill remaining cells with dots if EOF is reached
            } else if (ch == '\0') {
                map[i][j].symbol = ' '; // Replace null character with space
            } else if (ch == '\n') {
                map[i][j].symbol = '\n'; // Replace newline with dot (or another filler)
            } else {
                map[i][j].symbol = (char) ch;
            }
            map[i][j].visible = true;
            if ((map[i][j].symbol == '^') || (map[i][j].symbol == '?'))
                map[i][j].visible = false;
        }
    }

    fclose(file);
}


void save_map_to_file(int y, int x) {
    free_map_sf();
    FILE *file = fopen("example.txt", "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    for (int i = 1; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; ++j) {
            //fputc(map[i][j], file);
            fprintf(file, "%s\n", map[i]);
        }
    }
//    fseek(file, 0, SEEK_SET);
//    fprintf(file, "%d %d\n", y, x);

    fclose(file);
}

void update_visibility(int player_y, int player_x) {
    if ((map[player_y - 2][player_x].symbol == '?' && !map[player_y - 2][player_x].visible) ||
        (map[player_y - 2][player_x].symbol == '^' && !map[player_y - 2][player_x].visible)) {
        map[player_y - 2][player_x].visible = true; // Make the hidden 'p' visible
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
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            int c = 0;
            int pos = get_part_sf(i, j);
            int pos2 = get_part_sf(y, x);
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
                show_health_bar = 1;
            }
//            || map[i][j] == '*' || map[i][j] == '=' || map[i][j] == 's' ||
//               map[i][j] == '+'
            if (map[i][j].symbol == '#' || map[i][j].symbol == ' ')
                attroff(COLOR_PAIR(7));
            if (map[i][j].symbol == '@')
                attron(COLOR_PAIR(2));
            if (map[i][j].symbol == 'M')
                attron(COLOR_PAIR(3));
            if (map[i][j].symbol == '&')
                attron(COLOR_PAIR(6));
            if (map[i][j].symbol == 'j')
                attron(COLOR_PAIR(5));
            if (map[i][j].symbol == 'G')
                attron(COLOR_PAIR(4));
            int tar = show_map % 2 == 1;
            if ((tar) ||
                (i >= rooms[pos2].y - 2 && (i <= rooms[pos2].y + rooms[pos2].size - 2) && j >= rooms[pos2].x &&
                 (j <= rooms[pos2].x + rooms[pos2].size))) {
                if (map[i][j].symbol == '^' && map[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map[i][j].symbol == '?' && map[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map[i][j].symbol); }
            } else if ((i <= y + 1) && (i >= y - 5) && (j <= x + 3) && (j >= x - 3)) {
                if (map[i][j].symbol == '^' && map[i][j].visible == false) {
                    mvaddch(i + 2, j, '.');
                } else if (map[i][j].symbol == '?' && map[i][j].visible == false) {
                    mvaddch(i + 2, j, '*');
                } else { mvaddch(i + 2, j, map[i][j].symbol); }
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
            show_health_bar = 0;
            if (map[i][j].symbol == '\n') {

            }
            refresh();// Print each character individually
        }
    }
    move(map[0][0].symbol, map[0][2].symbol);
    refresh();
}


void free_map_sf() {
    // Open the file in write mode to truncate it
    FILE *file = fopen("example.txt", "w");
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
    rooms[room_num].y = start_row;
    rooms[room_num].x = start_col;
    rooms[room_num].size = height;

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
    room_num++;
}

void draw_border() {
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

int new_game() {

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
    curs_set(1);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
        init_pair(7, COLOR_WHITE, COLOR_GREEN);

    }
    draw_border();
    get_parts_sf(rands_sf);
    draw_room_sf();
    clear();
    refresh();
    load_map_from_file_sf();
    draw_map_to_terminal_sf();
    move(y, x);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, change_able_pass_sf, NULL);
    while ((ch = getch()) != 'q') {
        if (ch == 'f') {
            ch = getch();
            switch (ch) {
                case KEY_UP:
                    while (y > 0) {
                        y--;
                        if ((mvinch(y, x) & A_CHARTEXT) != '.' && (mvinch(y, x) & A_CHARTEXT) != '#') {
                            y++;
                            break;
                        }
                    }
                    break;
                case KEY_DOWN:
                    while (y < MAP_ROWS - 1) {
                        y++;
                        if ((mvinch(y, x) & A_CHARTEXT) != '.' && (mvinch(y, x) & A_CHARTEXT) != '#') {
                            y--;
                            break;
                        }
                    }
                    break;
                case KEY_LEFT:
                    while (x > 0) {
                        x--;
                        if ((mvinch(y, x) & A_CHARTEXT) != '.' && (mvinch(y, x) & A_CHARTEXT) != '#') {
                            x++;
                            break;
                        }
                    }
                    break;
                case KEY_RIGHT:
                    while (x < MAP_COLS - 1) {
                        x++;
                        if ((mvinch(y, x) & A_CHARTEXT) != '.' && (mvinch(y, x) & A_CHARTEXT) != '#') {
                            x--;
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
                load_map_from_file_sf();
                draw_map_to_terminal_sf();
                break;
            case 'j':
                clear();
                display_spell();
                load_map_from_file_sf();
                draw_map_to_terminal_sf();
                break;
            case 'i':
                clear();
                display_guns();
                load_map_from_file_sf();
                draw_map_to_terminal_sf();
                break;
            case 'v':
                show_map++;
                draw_map_to_terminal_sf();
                break;
            case KEY_UP:
                if (y > 0) y--;
                update_visibility(y, x);
                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o' || (mvinch(y, x) & A_CHARTEXT) == '=') {
                    y++;
                }
                break;
            case KEY_DOWN:
                if (y < MAP_ROWS - 1) y++;
                update_visibility(y, x);
                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o' || (mvinch(y, x) & A_CHARTEXT) == '=') {
                    y--;
                }
                break;
            case KEY_LEFT:
                if (x > 0) x--;
                update_visibility(y, x);
                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o' || (mvinch(y, x) & A_CHARTEXT) == '=') {
                    x++;
                }
                break;
            case KEY_RIGHT:
                if (x < MAP_COLS - 1) x++;
                update_visibility(y, x);

                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o' || (mvinch(y, x) & A_CHARTEXT) == '=') {
                    x--;
                }
                break;
            case 'p':
                if (y > 0) y--;
                if (x < MAP_COLS - 1) x++;
                update_visibility(y, x);

                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o') {
                    y++;
                    x--;
                }
                break;
            case 'w':
                if (y > 0) y--;
                if (x > 0) x--;
                update_visibility(y, x);

                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o') {
                    y++;
                    x++;
                }
                break;
            case ',':
                if (y < MAP_ROWS - 1) y++;
                if (x < MAP_COLS - 1) x++;
                update_visibility(y, x);
                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o') {
                    y--;
                    x--;
                }
                break;
            case 'z':
                if (x > 0) x--;
                if (y < MAP_COLS - 1) y++;
                update_visibility(y, x);

                if ((mvinch(y, x) & A_CHARTEXT) == '*' || (mvinch(y, x) & A_CHARTEXT) == ' ' ||
                    (mvinch(y, x) & A_CHARTEXT) == 'o') {
                    y--;
                    x++;
                }
                break;
            case ' ':
                map[y][x].symbol = (map[y][x].symbol == '.') ? '#' : '.';
                break;
        }
        update_visibility(y, x);
        draw_map_to_terminal_sf();
        if (x < 1) x = 1;
        if (y >= 35) y = 35;
        if (y < 2) y = 2;
        if (x >= 161) x = 161;
        move(y, x);
        if (((mvinch(y, x) & A_CHARTEXT) == '&')) {
            generate_pass_key_sf(y, x);
        }
        if (((mvinch(y, x) & A_CHARTEXT) == '@')) {
            int pos = get_part_sf(y, x);
            check_key_sf(pos);
            attron(COLOR_PAIR(3));
            mvaddch(y, x, '@');
            attroff(COLOR_PAIR(3));
        }
        chtype ch = mvinch(y, x);
        chtype attributes = ch & A_ATTRIBUTES;
        int color_pair = PAIR_NUMBER(attributes);
        if (color_pair == 7) {
            stop_thread = false;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, draw_health_bar, NULL);
        }
        if (((mvinch(y, x) & A_CHARTEXT) == '#')) {
            stop_thread = true;
        }

        if (((mvinch(y, x) & A_CHARTEXT) == 'G') || (mvinch(y, x) & A_CHARTEXT) == '$') {
            int achived_gold = calc_gold((mvinch(y, x) & A_CHARTEXT));
            add_file_sf(y, x, '.');
            mvaddch(y, x, '.');
            mvprintw(1, 2, "you achived %d golds", achived_gold);
            getch();
            load_map_from_file_sf();
            draw_map_to_terminal_sf();
            refresh();
        }
        if (((mvinch(y, x) & A_CHARTEXT) == 'M') || (mvinch(y, x) & A_CHARTEXT) == 'D' ||
            (mvinch(y, x) & A_CHARTEXT) == 'W' || (mvinch(y, x) & A_CHARTEXT) == 'A' ||
            (mvinch(y, x) & A_CHARTEXT) == 'S') {
            char inp = getch();
            if (inp == 'g') {
                calc_gun(mvinch(y, x) & A_CHARTEXT);
                add_file_sf(y, x, '.');
                mvaddch(y, x, '.');
                load_map_from_file_sf();
                draw_map_to_terminal_sf();
            }
        }
        if (((mvinch(y, x) & A_CHARTEXT) == 'h') || (mvinch(y, x) & A_CHARTEXT) == 's' ||
            (mvinch(y, x) & A_CHARTEXT) == 'd') {
            char inp = getch();
            if (inp == 'g') {
                calc_spell(mvinch(y, x) & A_CHARTEXT);
                add_file_sf(y, x, '.');
                mvaddch(y, x, '.');
                load_map_from_file_sf();
                draw_map_to_terminal_sf();
            }
        }
        if (((mvinch(y, x) & A_CHARTEXT) == 'n') || (mvinch(y, x) & A_CHARTEXT) == 'm' ||
            (mvinch(y, x) & A_CHARTEXT) == 'b' || (mvinch(y, x) & A_CHARTEXT) == 'c') {
            char inp = getch();
            if (inp == 'g') {
                int food_res = calc_food(mvinch(y, x) & A_CHARTEXT);
                if(food_res) {
                    add_file_sf(y, x, '.');
                    mvaddch(y, x, '.');
                    load_map_from_file_sf();
                    draw_map_to_terminal_sf();
                }
                else{
                    mvprintw(1, 2, "you already have maximum(5) food");
                    getch();
                    load_map_from_file_sf();
                    draw_map_to_terminal_sf();
                    refresh();
                }
            }
        }

        move(y, x);
//        getch();
//        mvaddch(10,3,'T');
//        refresh();
    }

    // save_map_to_file(y,x);

    endwin();
    return 0;
}

void previous_game() {
    int ch;// Cursor starting position
    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    resize_term(37, 162);
    keypad(stdscr, TRUE);
    curs_set(1);
    load_map_from_file_sf();

    draw_map_to_terminal_sf();
    move(y, x);

    while ((ch = getch()) != 'q') { // Press 'q' to quit
        switch (ch) {
            case KEY_UP:
                if (y > 0) y--;
                break;
            case KEY_DOWN:
                if (y < MAP_ROWS - 1) y++;
                break;
            case KEY_LEFT:
                if (x > 0) x--;
                break;
            case KEY_RIGHT:
                if (x < MAP_COLS - 1) x++;
                break;
            case ' ':
                map[y][x].symbol = (map[y][x].symbol == '.') ? '#' : '.';
                break;
        }

        draw_map_to_terminal_sf();
        if (x < 3) x = 3;
        if (y >= 34) y = 34;
        if (y < 3) y = 3;
        if (x >= 158) x = 158;
        move(y, x);
    }

    //save_map_to_file(y,x);

    endwin();
    return;
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
            room_type++;
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
                y = j;
                x = i;
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
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, '<');
        mvaddch(row, col, '<');
    }
    if (room_type % 2 != 1) {
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
    if (room_type % 2 == 1) {
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
        add_file_sf(row, col, 'M');
        mvaddch(row, col, 'M');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'D');
        mvaddch(row, col, 'D');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'W');
        mvaddch(row, col, 'W');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'A');
        mvaddch(row, col, 'A');
    }
    r = rand() % 2;
    for (int i = 0; i < r; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file_sf(row, col, 'S');
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

    file = fopen("example.txt", "r+");
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
    key_pair[pos] = password;
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
        int r_key = reverse_number_sf(key_pair[pos]);
        if ((key_pair[pos] != password) && (r_key != password)) {
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
        key_pair[2] = 1000 + rand() % 9000;
        key_pair[5] = 1000 + rand() % 9000;
        key_pair[8] = 1000 + rand() % 9000;
        sleep(5);
    }
    return NULL;
}

void *draw_health_bar() {
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    while (!stop_thread) {
        //  move(0, 0);  // Move to line 5, column 0
        // clrtoeol();
        // refresh();
        //  move(1, 0);  // Move to line 5, column 0
        // clrtoeol();
        //  refresh();
        int width = 50; // Width of the health bar
        int bar_length = (health * width) / MAX_HEALTH;
        health--;
        // Draw the label and open the health bar
        mvprintw(0, 2, "Health: [");

        // Draw the green part of the health bar
        attron(COLOR_PAIR(1));
        for (int i = 0; i < bar_length; i++) {
            mvaddch(0, 10 + i, ' ');
        }
        attroff(COLOR_PAIR(1));

        // Draw the empty part of the health bar
        for (int i = bar_length; i < width; i++) {
            mvaddch(0, 10 + i, ' ');
        }

        // Close the health bar
        mvaddch(0, 10 + width, ']');

        // Display the current health value below the bar
        mvprintw(1, 2, "Current Health: %d%%", health);
        move(y, x);
        refresh();
        sleep(1);
    }
    return NULL;
}


