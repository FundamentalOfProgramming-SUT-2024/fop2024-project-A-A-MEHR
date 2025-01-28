#include "game.c"

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

#define ROOM_HEIGHT 10
#define ROOM_WIDTH 20

// Function to create a treasure room
void treasure_room(char *username, int new) {
    clear();
    refresh;
    // Initialize ncurses
    initscr();
    noecho();
    curs_set(0);

    // Get screen dimensions
    int screen_height, screen_width;
    getmaxyx(stdscr, screen_height, screen_width);

    // Calculate room position to center it
    int start_y = (screen_height - ROOM_HEIGHT) / 2;
    int start_x = (screen_width - ROOM_WIDTH) / 2;

    // Draw the room's walls
    for (int i = 0; i < ROOM_HEIGHT; i++) {
        for (int j = 0; j < ROOM_WIDTH; j++) {
            if (i == 0 || i == ROOM_HEIGHT - 1 || j == 0 || j == ROOM_WIDTH - 1) {
                mvaddch(start_y + i, start_x + j, '*'); // Walls
            } else {
                mvaddch(start_y + i, start_x + j, ' '); // Empty space
            }
        }
    }

    // Place traps (`^`) randomly inside the room
    srand(time(NULL));
    int trap_count = 5; // Number of traps
    while (trap_count > 0) {
        int trap_y = rand() % (ROOM_HEIGHT - 2) + 1; // Inside the walls
        int trap_x = rand() % (ROOM_WIDTH - 2) + 1;

        // Place the trap if the spot is empty
        if (mvinch(start_y + trap_y, start_x + trap_x) == ' ') {
            mvaddch(start_y + trap_y, start_x + trap_x, '^');
            trap_count--;
        }
    }

    // Display the player's username at the top of the room
    mvprintw(start_y - 1, start_x, "Player: %s", username);

    // Refresh the screen to show the room
    refresh();

    // Wait for a key press before exiting
    getch();

    // End ncurses mode
    endwin();
}

