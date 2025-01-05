#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define MAP_ROWS 36
#define MAP_COLS 160

char map[MAP_ROWS][MAP_COLS];

void free_map();

void move_to_second_line(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}

void load_map_from_file() {
    FILE *file = fopen("first_floor.txt", "r");
    move_to_second_line(file);
    fseek(file, 2, SEEK_CUR);
    if (!file) {
        perror("Error opening file");
        // Initialize map with default values if file doesn't exist
        for (int i = 0; i < MAP_ROWS; i++) {
            memset(map[i], '.', MAP_COLS);
            map[i][MAP_COLS] = '\0';
        }
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            int ch = fgetc(file); // Read character by character
            if (ch == EOF) {
                map[i][j] = ' '; // Fill remaining cells with dots if EOF is reached
            } else if (ch == '\0') {
                map[i][j] = ' '; // Replace null character with space
            } else if (ch == '\n') {
                map[i][j] = '\n'; // Replace newline with dot (or another filler)
            } else {
                map[i][j] = (char) ch; // Keep other characters as they are
            }
        }
    }

    fclose(file);
}


void save_map_to_file(int y, int x) {
    free_map();
    FILE *file = fopen("first_floor.txt", "w");
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

void draw_map_to_terminal() {
    clear();
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
        init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    }// Clear the screen
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (map[i][j] == '@')
                attron(COLOR_PAIR(2));
            if (map[i][j] == '&')
                attron(COLOR_PAIR(4));
            mvaddch(i + 2, j, map[i][j]);
            attroff(COLOR_PAIR(2));
           attroff(COLOR_PAIR(4));

            if (map[i][j] == '\n') {

            }
            refresh();// Print each character individually
        }
    }
    move(map[0][0], map[0][2]);
    refresh();
}


void free_map() {
    // Open the file in write mode to truncate it
    FILE *file = fopen("first_floor.txt", "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // No need to write anything, file is now empty
    fclose(file);  // Close the file

    return;
}

