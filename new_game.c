#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "move.c"

int DELAY = 100000;

void get_parts(int *rands);

void draw_room();

int rands[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

void fill_room(int start_row, int start_col, int height, int width);

void fill_room_out_window(int start_row, int start_col, int height, int width);

void fill_room_out_door(int start_row, int start_col, int height, int width);

void put_corridor(int start_row, int start_col);


int add_file(int row, int col, char character);

void new_game();

void free_map();

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
    int x = 158, y = 5; // Cursor starting position
    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    resize_term(37, 162);
    keypad(stdscr, TRUE);
    curs_set(1);
    draw_border();
    get_parts(rands);
    draw_room();
    clear();
    refresh();
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

    save_map_to_file();

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
    for (int i = 0; i < 9; ++i) {
        int start_row = rand() % 3;
        int start_col = rand() % 40;
        int size = (rand() % 3) + 4;
        if (rands[i] == 1) {
            start_row += 3 + 11 * (i / 3);
            start_col += 3 + 52 * (i % 3);
            draw_rectangle(start_row, start_col, size, size);
            fill_room(start_row, start_col, size, size);
            fill_room_out_window(start_row, start_col, size, size);
            fill_room_out_door(start_row, start_col, size, size);
        }
    }
}

void fill_room(int start_row, int start_col, int height, int width) {
    int o_num = rand() % 3;
    for (int i = start_col + 1; i <= start_col + width; ++i) {
        for (int j = start_row + 1; j <= start_row + height; ++j) {
            add_file(j, i, '.');
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
        refresh();
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
        refresh();
        start_col--;
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

void free_map() {
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