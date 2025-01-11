#include <ncurses.h>
#include "database.c"

Game my_game;

int calc_gold(char g_char) {
    if (g_char == 'G') {
        my_game.gold += 3;
        return 3;
    } else if (g_char == '$') {
        my_game.black_gold += 10;
        return 10;
    }
}

void calc_gun(char g_char) {
    if (g_char == 'M') {
        my_game.Mace++;
    } else if (g_char == 'D') {
        my_game.Dagger++;
    } else if (g_char == 'W') {
        my_game.Magic_Wand++;
    } else if (g_char == 'A') {
        my_game.Normal_Arrow++;
    } else if (g_char == 'S') {
        my_game.Sword++;
    }
}

void calc_spell(char g_char) {
    if (g_char == 'h') {
        my_game.Health++;
    } else if (g_char == 's') {
        my_game.Speed++;
    } else if (g_char == 'd') {
        my_game.Damage++;
    }
}

int calc_food(char g_char) {
    if ((my_game.Normal_Food + my_game.Magic_Food + my_game.Best_Food + my_game.Corrupted_Food) > 4)
        return 0;

    if (g_char == 'n') {
        my_game.Normal_Food++;
    } else if (g_char == 'm') {
        my_game.Magic_Food++;
    } else if (g_char == 'b') {
        my_game.Best_Food++;
    } else if (g_char == 'c') {
        my_game.Corrupted_Food++;
    }
    return 1;
}

void display_spell() {
    int rows = 3, cols = 1; // Table dimensions
    char *headers[] = {"Health", "Speed", "Damage"};
    int data[3][1] = {my_game.Health, my_game.Speed, my_game.Damage};

    int win_height = rows + 4; // Extra space for headers and borders
    int win_width = cols * 10; // Column width times number of columns
    int starty = 3, startx = 5; // Window position

    // Draw a border around the window

    // Print the headers
    for (int i = 0; i < cols; i++) {
        mvwprintw(stdscr, 1, i * 10 + 2, "%s", headers[i]);
    }

    // Print the data
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mvwprintw(stdscr, i + 2, j * 10 + 2, "%s -> %d", headers[i], data[i][j]);
        }
    }

    // Refresh the window
    refresh();
    // Wait for user input before closing the window
    mvwprintw(stdscr, win_height - 2, 2, "Press any key to close...");
    refresh();
    getch();

    // Delete the window
}

void display_guns() {
    int rows = 5, cols = 1; // Table dimensions
    char *headers[] = {"Mace", "Dagger", "Magic_Wand", "Normal_Arrow", "Sword"};
    int data[5][1] = {my_game.Mace, my_game.Dagger, my_game.Magic_Wand, my_game.Normal_Arrow, my_game.Sword};

    int win_height = rows + 4; // Extra space for headers and borders


    for (int i = 0; i < cols; i++) {
        mvwprintw(stdscr, 1, i * 10 + 2, "%s", headers[i]);
    }

    // Print the data
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mvwprintw(stdscr, i + 2, j * 10 + 2, "%s -> %d", headers[i], data[i][j]);
        }
    }

    // Refresh the window
    refresh();
    // Wait for user input before closing the window
    mvwprintw(stdscr, win_height - 2, 2, "Press any key to close...");
    refresh();
    getch();

    // Delete the window
}

void eat_food() {
    int rows = 4, cols = 1; // Table dimensions
    char *headers[] = {"Normal_Food", "Magic_Food", "Best_Food", "Corrupted_Food"};
    int data[4][1] = {my_game.Normal_Food, my_game.Magic_Food, my_game.Best_Food, my_game.Corrupted_Food};

    int win_height = rows + 4; // Extra space for headers and borders

    mvprintw(0, 2, "Enter e if you want to eat food!");
    for (int i = 0; i < cols; i++) {
        mvwprintw(stdscr, 1, i * 10 + 2, "%s", headers[i]);
    }

    // Print the data
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mvwprintw(stdscr, i + 2, j * 10 + 2, "%s -> %d", headers[i], data[i][j]);
        }
    }

    // Refresh the window
    refresh();
    int inp = getch();
    if (inp == 'e') {
        if (my_game.Normal_Food < 1) {
            mvprintw(0, 2, "Not enough food!");
            refresh();
            return;
        }
        my_game.Normal_Food--;
        my_game.Health += 50;
        mvprintw(0, 2, "Your Health is %d now!", my_game.Health);
    }
    // Wait for user input before closing the window
    mvwprintw(stdscr, win_height - 2, 2, "Press any key to close...");
    refresh();
    getch();

}

void update_game_in_database(char *username, int floor) {
    update_game(my_game, username, floor);
    update_user_total_score(my_game,username);
}