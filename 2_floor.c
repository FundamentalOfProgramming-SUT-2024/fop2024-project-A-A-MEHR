#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "move.c"

int key_pair[9];
int DELAY = 100000;
int x = 0, y = 0;

void get_parts(int *rands);

void draw_room();

void *change_able_pass();

int rands[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

void fill_room(int start_row, int start_col, int height, int width, int tmp, int tmp2);

void fill_room_out_window(int start_row, int start_col, int height, int width);

void fill_room_out_door(int start_row, int start_col, int height, int width);

void put_corridor(int start_row, int start_col);

int get_part(int y, int x);

int add_file(int row, int col, char character);

void new_game();

int reverse_number(int num);

int generate_pass_key(int y, int x);

int check_key(int pos);

void draw_rectangle(int start_row, int start_col, int height, int width) {
    height++;
    width++;
    for (int i = start_col; i <= start_col + width; ++i) {
        add_file(start_row, i, '*');
        mvaddch(start_row, i, '*');
        refresh();
        add_file(start_row + height, i, '*');
        mvaddch(start_row + height, i, '*');
        refresh();
    }
    for (int i = start_row; i <= start_row + height; ++i) {
        add_file(i, start_col, '*');
        mvaddch(i, start_col, '*');
        refresh();
        add_file(i, start_col + width, '*');
        mvaddch(i, start_col + width, '*');
        refresh();
    }
}

void draw_border() {
    free_map();
    add_file(2, 1, '#');
    mvaddch(2, 1, '#');
    for (int i = 2; i < 36; ++i) {
        add_file(i, 2, '#');
        mvaddch(i, 3 - 1, '#');
        refresh();
        add_file(i, 54, '#');
        mvaddch(i, 55 - 1, '#');
        refresh();
        add_file(i, 106, '#');
        mvaddch(i, 107 - 1, '#');
        refresh();
        add_file(i, 158, '#');
        mvaddch(i, 159 - 1, '#');
        refresh();
    }

    for (int i = 2; i < 159; ++i) {
        add_file(2, i, '#');
        mvaddch(3 - 1, i, '#');
        refresh();
        add_file(13, i, '#');
        mvaddch(14 - 1, i, '#');
        refresh();
        add_file(24, i, '#');
        mvaddch(25 - 1, i, '#');
        refresh();
        add_file(35, i, '#');
        mvaddch(36 - 1, i, '#');
        refresh();
    }

}

void new_game() {

    int ch;
    // Cursor starting position
    srand(time(NULL));
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
    }
    draw_border();
    get_parts(rands);
    draw_room();
    clear();
    refresh();
    load_map_from_file();

    draw_map_to_terminal();
    move(y, x);
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, change_able_pass, NULL);
    while ((ch = getch()) != 'q') { // Press 'q' to quit
        switch (ch) {
            case KEY_UP:
                if (y > 0) y--;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y++;
                }
                break;
            case KEY_DOWN:
                if (y < MAP_ROWS - 1) y++;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y--;
                }
                break;
            case KEY_LEFT:
                if (x > 0) x--;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    x++;
                }
                break;
            case KEY_RIGHT:
                if (x < MAP_COLS - 1) x++;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    x--;
                }
                break;
            case 'p':
                if (y > 0) y--;
                if (x < MAP_COLS - 1) x++;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y++;
                    x--;
                }
                break;
            case 'w':
                if (y > 0) y--;
                if (x > 0) x--;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y++;
                    x++;
                }
                break;
            case 'm':
                if (y < MAP_ROWS - 1) y++;
                if (x < MAP_COLS - 1) x++;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y--;
                    x--;
                }
                break;
            case 'z':
                if (x > 0) x--;
                if (y < MAP_COLS - 1) y++;
                if (mvinch(y, x) == '*' || mvinch(y, x) == ' ' || mvinch(y, x) == 'o') {
                    y--;
                    x++;
                }
                break;
            case ' ':
                map[y][x] = (map[y][x] == '.') ? '#' : '.';
                break;
        }
        draw_map_to_terminal();
        if (x < 2) x = 2;
        if (y >= 35) y = 35;
        if (y < 2) y = 2;
        if (x >= 161) x = 161;
        move(y, x);
        if (((mvinch(y, x) & A_CHARTEXT) == '&')) {
            generate_pass_key(y, x);
        }
        if (((mvinch(y, x) & A_CHARTEXT) == '@')) {
            int pos = get_part(y, x);
            check_key(pos);
            attron(COLOR_PAIR(3));
            mvaddch(y, x, '@');
            attroff(COLOR_PAIR(3));
        }
        if (((mvinch(y, x) & A_CHARTEXT) == 'P')) {
            second_floor(y,x);
        }


        move(y, x);
//        getch();
//        mvaddch(10,3,'T');
//        refresh();
    }

    // save_map_to_file(y,x);

    endwin();
    return;
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
    load_map_from_file();

    draw_map_to_terminal();
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
                map[y][x] = (map[y][x] == '.') ? '#' : '.';
                break;
        }

        draw_map_to_terminal();
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


void get_parts(int *rands) {
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

void draw_room() {
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
        if (rands[i] == 1) {
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
            draw_rectangle(start_row, start_col, size, size);
            fill_room(start_row, start_col, size, size, tmp, tmp2);
            fill_room_out_window(start_row, start_col, size, size);
            fill_room_out_door(start_row, start_col, size, size);
            tmp = 0;
            tmp2 = 0;
        }
    }
}

void fill_room(int start_row, int start_col, int height, int width, int tmp, int tmp2) {
    int o_num = rand() % 3;
    for (int i = start_col + 1; i <= start_col + width; ++i) {
        for (int j = start_row + 1; j <= start_row + height; ++j) {
            add_file(j, i, '.');
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
        add_file(row, col, 'o');
        mvaddch(row, col, 'o');
    }
    for (int i = 0; i < o_num; ++i) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file(row, col, 'T');
        mvaddch(row, col, 'T');
    }
    if (tmp2) {
        int row = rand() % (height - 1) + start_row + 1;
        int col = rand() % (width - 1) + start_col + 1;
        add_file(row, col, 'P');
        mvaddch(row, col, 'P');
    }

}

void fill_room_out_window(int start_row, int start_col, int height, int width) {
    int index = rand() % (2 * (height + 1 + width + 1));
    if (index < width + 1) {
        add_file(start_row, start_col + index, '=');
        mvaddch(start_row, start_col + index, '=');
    } else if (width < index && index < width + height + 2) {
        index -= (width + 1);
        add_file(start_row + index, start_col + width + 1, '=');
        mvaddch(start_row + index, start_col + width + 1, '=');
    } else if (width + height + 1 < index && index < width + height + width + 3) {
        index -= (width + height + 2);
        add_file(start_row + height + 1, start_col + width + 1 - index, '=');
        mvaddch(start_row + height + 1, start_col + width + 1 - index, '=');
    } else {
        index -= (width + height + width + 3);
        add_file(start_row + height + 1 - index, start_col, '=');
        mvaddch(start_row + height + 1 - index, start_col, '=');
    }
}

void fill_room_out_door(int start_row, int start_col, int height, int width) {
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
            add_file(start_row, start_col + index, 's');
            mvaddch(start_row, start_col + index, 's');
            put_corridor(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file(start_row + index, start_col + width + 1, 's');
            mvaddch(start_row + index, start_col + width + 1, 's');
            put_corridor(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file(start_row + height + 1, start_col + width + 1 - index, 's');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, 's');
            put_corridor(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file(start_row + height + 1 - index, start_col, 's');
            mvaddch(start_row + height + 1 - index, start_col, 's');
            put_corridor(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat; ++i) {
        int index = rand() % (2 * (height + 1 + width + 1));
        if (index < width + 1) {
            add_file(start_row, start_col + index, '+');
            mvaddch(start_row, start_col + index, '+');
            put_corridor(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file(start_row + index, start_col + width + 1, '+');
            mvaddch(start_row + index, start_col + width + 1, '+');
            put_corridor(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file(start_row + height + 1, start_col + width + 1 - index, '+');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '+');
            put_corridor(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file(start_row + height + 1 - index, start_col, '+');
            mvaddch(start_row + height + 1 - index, start_col, '+');
            put_corridor(start_row + height + 1 - index, start_col);
        }
    }
    for (int i = 0; i < repeat - 1; ++i) {
        if (index < width + 1) {
            add_file(start_row, start_col + index, '@');
            mvaddch(start_row, start_col + index, '@');
            put_corridor(start_row, start_col + index);
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file(start_row + index, start_col + width + 1, '@');
            mvaddch(start_row + index, start_col + width + 1, '@');
            put_corridor(start_row + index, start_col + width + 1);
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file(start_row + height + 1, start_col + width + 1 - index, '@');
            mvaddch(start_row + height + 1, start_col + width + 1 - index, '@');
            put_corridor(start_row + height + 1, start_col + width + 1 - index);
        } else {
            index -= (width + height + width + 3);
            add_file(start_row + height + 1 - index, start_col, '@');
            mvaddch(start_row + height + 1 - index, start_col, '@');
            put_corridor(start_row + height + 1 - index, start_col);
        }
    }
    if (repeat == 2) {
        start_row++;
        start_col++;
        width -= 2;
        height -= 2;
        if (index < width + 1) {
            add_file(start_row, start_col, '&');
            mvaddch(start_row, start_col, '&');
        } else if (width < index && index < width + height + 2) {
            index -= (width + 1);
            add_file(start_row, start_col + width + 1, '&');
            mvaddch(start_row, start_col + width + 1, '&');
        } else if (width + height + 1 < index && index < width + height + width + 3) {
            index -= (width + height + 2);
            add_file(start_row + height + 1, start_col + width + 1, '&');
            mvaddch(start_row + height + 1, start_col + width + 1, '&');
        } else {
            index -= (width + height + width + 3);
            add_file(start_row + height + 1, start_col, '&');
            mvaddch(start_row + height + 1, start_col, '&');
        }
    }
}

void put_corridor(int start_row, int start_col) {
    int flag = 0;
    while ((mvinch(start_row + 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file(start_row + 1, start_col, '#');
        mvaddch(start_row + 1, start_col, '#');
        start_row++;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col + 1) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file(start_row, start_col + 1, '#');
        mvaddch(start_row, start_col + 1, '#');
        start_col++;
    }
    if (flag)
        return;
    while ((mvinch(start_row - 1, start_col) & A_CHARTEXT) == ' ') {
        flag = 1;
        add_file(start_row - 1, start_col, '#');
        mvaddch(start_row - 1, start_col, '#');
        start_row--;
    }
    if (flag)
        return;
    while ((mvinch(start_row, start_col - 1) & A_CHARTEXT) == ' ') {
        add_file(start_row, start_col - 1, '#');
        mvaddch(start_row, start_col - 1, '#');
        start_col--;
        if (start_col == 3) {
            add_file(start_row, 2, '#');
            mvaddch(start_row, 2, '#');
        }
    }

}


#include <stdio.h>

int add_file(int row, int col, char character) {
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

int generate_pass_key(int y, int x) {
    int pos = get_part(y, x);
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

int get_part(int y, int x) {
    int part_width = 156 / 3;
    int part_height = 33 / 3;

    int row = (y - 2) / part_height;
    int col = (x - 2) / part_width;

    return row * 3 + col; // 0-based part index
}

int check_key(int pos) {
    int i = 0;   // Wrong attempt count
    while (1) {
        mvprintw(0, 2, "Enter key: ");
        int password;
        scanw("%d", &password);
        int r_key = reverse_number(key_pair[pos]);
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

int reverse_number(int num) {
    int reversed = 0;

    while (num != 0) {
        int digit = num % 10;         // Extract the last digit
        reversed = reversed * 10 + digit; // Add the digit to the reversed number
        num /= 10;                    // Remove the last digit from the original number
    }

    return reversed;
}

void *change_able_pass() {
    while (1) {
        key_pair[2] = 1000 + rand() % 9000;
        key_pair[5] = 1000 + rand() % 9000;
        key_pair[8] = 1000 + rand() % 9000;
        sleep(5);
    }
    return NULL;
}