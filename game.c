#include <ncurses.h>
#include "database.c"

Game my_game;

void return_to_bag();

int calc_gold(char g_char) {
    if (g_char == 'G') {
        my_game.gold += 3;
        return 3;
    } else if (g_char == '$') {
        my_game.black_gold += 10;
        return 10;
    }
}

void calc_gun(char g_char, int num) {
    if (g_char == 'M') {
        my_game.Mace += num;
    } else if (g_char == 'D') {
        my_game.Dagger += num;
    } else if (g_char == 'W') {
        my_game.Magic_Wand += num;
    } else if (g_char == 'A') {
        my_game.Normal_Arrow += num;
    } else if (g_char == 'S') {
        my_game.Sword += num;
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
        if (my_game.Corrupted_Food)
            my_game.Health -= 10;
        else
            my_game.Health -= 10;

    } else if (g_char == 'm') {
        my_game.Magic_Food++;
        my_game.Health += 10;
    } else if (g_char == 'b') {
        my_game.Best_Food++;
        my_game.Health += 10;
    } else if (g_char == 'c') {
//        my_game.Corrupted_Food++;
    }
    return 1;
}

void *food_change() {
    while (1) {
        if (my_game.Best_Food > 0) {
            my_game.Normal_Food++;
            my_game.Best_Food--;
        }
        if (my_game.Magic_Food > 0) {
            my_game.Normal_Food++;
            my_game.Magic_Food--;
        }
        if (my_game.Normal_Food > 0) {
            my_game.Normal_Food--;
            my_game.Corrupted_Food++;
        }
        sleep(60);
    }
}

int is_hungry() {
    if ((my_game.Normal_Food + my_game.Magic_Food + my_game.Best_Food + my_game.Corrupted_Food) > 4)
        return 0;
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
            switch (i) {
                case 0:
                    mvwprintw(stdscr, i + 2, j * 10 + 2, "🥍 %s -> %d short range, power:5, distance:1", headers[i],
                              data[i][j]);
                    break;
                case 1:
                    mvwprintw(stdscr, i + 2, j * 10 + 2, " 🔪 %s -> %d long range, power:12, distance:5", headers[i],
                              data[i][j]);
                    break;
                case 2:
                    mvwprintw(stdscr, i + 2, j * 10 + 2, "🦯  %s -> %d long range, power:15, distance:10", headers[i],
                              data[i][j]);
                    break;
                case 3:
                    mvwprintw(stdscr, i + 2, j * 10 + 2, "🏹 %s -> %d long range, power:5, distance:5", headers[i],
                              data[i][j]);
                    break;
                case 4:
                    mvwprintw(stdscr, i + 2, j * 10 + 2, "🗡 %s -> %d short range, power:10, distance:1", headers[i],
                              data[i][j]);
                    break;
            }
        }
    }

    // Refresh the window
    refresh();
    // Wait for user input before closing the window
    mvwprintw(stdscr, win_height - 1, 2, "change gun or Press other keys to close...");
    refresh();
    char gun = getch();
    switch (gun) {
        case 'm':
            if (data[0][0] == 0)
                mvwprintw(stdscr, win_height - 2, 2, "Mace isn't available in guns!");
            else if (my_game.current_gun != -1) {
                mvwprintw(stdscr, win_height - 2, 2, "first put your gun in the bag!");
                if (getch() == 'w') {
                    return_to_bag();
                    my_game.current_gun = 0;
                    my_game.Mace--;
                    mvwprintw(stdscr, win_height - 2, 2, "your gun is Mace now!");
                }
            } else {
                my_game.current_gun = 0;
                my_game.Mace--;
                mvwprintw(stdscr, win_height - 2, 2, "your gun is Mace now!");
            }
            break;
        case 'd':
            if (data[1][0] == 0)
                mvwprintw(stdscr, win_height - 2, 2, "Dagger isn't available in guns!");
            else if (my_game.current_gun != -1) {
                mvwprintw(stdscr, win_height - 2, 2, "first put your gun in the bag!");
                if (getch() == 'w') {
                    return_to_bag();
                    my_game.current_gun = 1;
                    my_game.Dagger--;
                    mvwprintw(stdscr, win_height - 2, 2, "your gun is Dagger now!");
                }
            } else {
                my_game.current_gun = 1;
                my_game.Dagger--;
                mvwprintw(stdscr, win_height - 2, 2, "your gun is Dagger now!");
            }
            break;
        case 'w':
            if (data[2][0] == 0)
                mvwprintw(stdscr, win_height - 2, 2, "Magic_Wand isn't available in guns!");
            else if (my_game.current_gun != -1) {
                mvwprintw(stdscr, win_height - 2, 2, "first put your gun in the bag!");
                if (getch() == 'w') {
                    return_to_bag();
                    my_game.current_gun = 2;
                    my_game.Magic_Wand--;
                    mvwprintw(stdscr, win_height - 2, 2, "your gun is Magic_Wand now!");
                }
            } else {
                my_game.current_gun = 2;
                my_game.Magic_Wand--;
                mvwprintw(stdscr, win_height - 2, 2, "your gun is Magic_Wand now!");
            }
            break;
        case 'a':
            if (data[3][0] == 0)
                mvwprintw(stdscr, win_height - 2, 2, "Normal_Arrow isn't available in guns!");
            else if (my_game.current_gun != -1) {
                mvwprintw(stdscr, win_height - 2, 2, "first put your gun in the bag!");
                if (getch() == 'w') {
                    return_to_bag();
                    my_game.current_gun = 3;
                    my_game.Normal_Arrow--;
                    mvwprintw(stdscr, win_height - 2, 2, "your gun is Normal_Arrow now!");
                }

            } else {
                my_game.current_gun = 3;
                my_game.Normal_Arrow--;
                mvwprintw(stdscr, win_height - 2, 2, "your gun is Normal_Arrow now!");
            }
            break;
        case 's':
            if (data[4][0] == 0)
                mvwprintw(stdscr, win_height - 2, 2, "Sword isn't available in guns!");
            else if (my_game.current_gun != -1) {
                mvwprintw(stdscr, win_height - 2, 2, "first put your gun in the bag!");
                if (getch() == 'w') {
                    return_to_bag();
                    my_game.current_gun = 4;
                    my_game.Sword--;
                    mvwprintw(stdscr, win_height - 2, 2, "your gun is Sword now!");
                }

            } else {
                my_game.current_gun = 4;
                my_game.Sword--;
                mvwprintw(stdscr, win_height - 2, 2, "your gun is Sword now!");
            }
            break;
        default:
            mvwprintw(stdscr, win_height - 2, 2, "There isn't such that gun!");
    }
    refresh();
    getch();
    // Delete the window
}

void return_to_bag() {
    switch (my_game.current_gun) {
        case 0:
            my_game.Mace++;
            break;
        case 1:
            my_game.Dagger++;
            break;
        case 2:
            my_game.Magic_Wand++;
            break;
        case 3:
            my_game.Normal_Arrow++;
            break;
        case 4:
            my_game.Sword++;
            break;
    }
}

void eat_food(int *speed, int *power) {
    int rows = 4, cols = 1; // Table dimensions
    char *headers[] = {"Normal_Food", "Magic_Food", "Best_Food", "Corrupted_Food"};
    int data[4][1];
    data[0][0] = my_game.Normal_Food;
    data[1][0] = my_game.Magic_Food;
    data[2][0] = my_game.Best_Food;
    data[3][0] = my_game.Corrupted_Food;
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
        my_game.Health += 20;
        mvprintw(0, 2, "Your Health is %d now!", my_game.Health);
    }
    if (inp == 'm') {
        if (my_game.Magic_Food < 1) {
            mvprintw(0, 2, "Not enough Magic_food!");
            refresh();
            return;
        }
        (*speed) = 2;
        my_game.Magic_Food--;
        my_game.Health += 20;
        mvprintw(0, 2, "Your Health is %d now!", my_game.Health);
    }
    if (inp == 'b') {
        if (my_game.Best_Food < 1) {
            mvprintw(0, 2, "Not enough Best_food!");
            refresh();
            return;
        }
        (*power) = 2;
        my_game.Best_Food--;
        my_game.Health += 20;
        mvprintw(0, 2, "Your Health is %d now!", my_game.Health);
    }
    // Wait for user input before closing the window
    mvwprintw(stdscr, win_height - 2, 2, "Press any key to close...");
    refresh();
    getch();

}


#define MAX_FOOD 4

void draw_hunger_bar() {
    init_pair(1, COLOR_RED, COLOR_RED);

    int food_count = my_game.Best_Food + my_game.Normal_Food + my_game.Magic_Food;
    int hunger = 4 - food_count;
    int bar_width = 20;  // Width of the hunger bar
    int fill_amount = (hunger * bar_width) / MAX_FOOD;  // Calculate fill percentage

    mvprintw(0, 40, "Hunger: [");
    attron(COLOR_PAIR(1));

    // Draw filled portion with green color
    attron(COLOR_PAIR(1));
    for (int i = 0; i < fill_amount; i++) {
        mvaddch(0, 49 + i, ' ' | A_REVERSE);  // Use reverse attribute for visibility
    }
    attroff(COLOR_PAIR(1));

    // Draw empty portion
    for (int i = fill_amount; i < bar_width; i++) {
        mvaddch(0, 49 + i, ' ');
    }

    mvprintw(0, 49 + bar_width, "] %d%%", (hunger * 100) / MAX_FOOD);
    refresh();
}


void update_game_in_database(char *username, int floor) {
    update_game(my_game, username, floor);
    update_user_total_score(my_game, username);
}