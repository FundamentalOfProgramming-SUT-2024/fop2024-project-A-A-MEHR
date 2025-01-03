#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define MAP_ROWS 36
#define MAP_COLS 160

char map[MAP_ROWS][MAP_COLS];
void move_to_second_line(FILE *file) {
    char buffer[1024]; // Buffer to hold the first line

    // Read and discard the first line
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        printf("Error or EOF: Could not move to the second line.\n");
    }
}
void load_map_from_file() {
    FILE *file = fopen("example.txt", "r");
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
                map[i][j] = (char)ch; // Keep other characters as they are
            }
        }
    }

    fclose(file);
}



void save_map_to_file() {
    FILE *file = fopen("example.txt", "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    for (int i = 0; i < MAP_ROWS; i++) {
        fprintf(file, "%s\n", map[i]);
    }

    fclose(file);
}

void draw_map_to_terminal() {
    clear(); // Clear the screen
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 2; j < MAP_COLS; j++) {
            mvaddch(i+2, j, map[i][j]);
            if(map[i][j]=='\n'){

            }
            refresh();// Print each character individually
        }
    }
    refresh();
}


