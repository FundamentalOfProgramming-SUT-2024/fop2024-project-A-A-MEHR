
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "secondmenu.c"

//typedef struct {
//    char username[100];
//    char password[100];
//    char email[100];
//} user;

void create_input_box(int y, int x, const char *prompt, char *input);

int check_correct_password(char *password);

int check_correct_email(char *email);

void entry_menu();

void draw_menu_border();

void signin();

void login();

int password_forget_or_not(user *user1);

void generate_password(int length, user *user1);

int password_op(user *user1);

void get_password_with_username(user *user1);

void show_error(char *str) {
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
    }
    attron(COLOR_PAIR(1));
    move(0, 0);
    printw("%s", str);
    attroff(COLOR_PAIR(1));
    refresh();
}

int main(void) {
    init_db_connection();
    setlocale(LC_ALL, "");
    initscr();
    resize_term(100, 100);
    curs_set(FALSE);
    keypad(stdscr, TRUE);
//    noecho();

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        init_pair(3, COLOR_GREEN, COLOR_BLACK);
    }
    getch();
    entry_menu();

    // endwin();
    return 0;
}

void guest() {

}

void entry_menu() {
    const char *entry_type[] = {"signIn", "guest", "logIn"};
    int choice = 0;
    while (1) {
        move(LINES / 2 - 1, COLS / 2 - 1);
        int x = getcurx(stdscr);
        int y = getcury(stdscr);
        draw_menu_border();
        mvprintw(1, 1, "Select Enter Type:");
        for (int i = 0; i < 3; ++i) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(y + i - 1, x, "%s", entry_type[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 2 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 2) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    clear();
    switch (choice) {
        case 0:
            signin();
            break;
        case 1:
            second_menu("guest");
            break;
        case 2:
            login();
            break;
    }
    refresh();
}

void draw_menu_border(int x1, int y1, int x2, int y2) {

}


void signin() {
    user user;
    int y = LINES / 2 - 1;
    int x = COLS / 2 - 1;
    create_input_box(y + 1, x, "username", user.username);
    if (check_username_exists(user)) {
        show_error("username already exists!");
        return;
    }
    if (password_op(&user))
        create_input_box(y + 3, x, "password", user.password);
//    printw("%s", user.password);
    sleep(1);
    int in_correct_input = 0;
    in_correct_input = check_correct_password(user.password);
    if (!in_correct_input) {
        show_error("password not valid");
        return;
    }
    create_input_box(y + 5, x, "email", user.email);
    in_correct_input = check_correct_email(user.email);
    if (!in_correct_input) {
        show_error("email not valid");
        return;
    }
//    printw("***%s***", user.password);
    save_user(user);
    second_menu(user.username);
}

void login() {
    user user;
    int y = LINES / 2 - 1;
    int x = COLS / 2 - 1;
    create_input_box(y, x, "username", user.username);
    if (!check_username_exists(user)) {
        show_error("user not found");
        return;
    }
    if (!password_forget_or_not(&user))
        create_input_box(y + 2, x, "password", user.password);
    sleep(2);
    if (!check_username_with_password_exists(user)) {
        show_error("password invalid");
        return;
    }
    second_menu(user.username);
}


int check_correct_password(char *password) {
    int has_lower = 0;
    int has_upper = 0;
    int has_digit = 0;
//    printw("++%d++", strlen(password));
    if (strlen(password) < 7)
        return 0;
    for (int i = 0; i < strlen(password); ++i) {
        if (password[i] >= 'a' && password[i] <= 'z') {
            has_lower = 1;
            break;
        }
    }
    for (int i = 0; i < strlen(password); ++i) {
        if (password[i] >= 'A' && password[i] <= 'Z') {
            has_upper = 1;
            break;
        }
    }
    for (int i = 0; i < strlen(password); ++i) {
        if (password[i] >= '0' && password[i] <= '9') {
            has_digit = 1;
            break;
        }
    }
    if (!(has_upper && has_lower && has_digit)) {
        return 0;
    }
    return 1;
}

int check_correct_email(char *email) {
    int first_index = 0;
    int second_index = 0;
    for (int i = 0; i < strlen(email); ++i) {
        if (email[i] == '@') {
            first_index = i;

        }
    }
    for (int i = 0; i < strlen(email); ++i) {
        if (email[i] == '.') {
            second_index = i;
        }
    }
    if (second_index > first_index + 1) {
        for (int i = first_index + 1; i < second_index; ++i) {
            if (email[i] != ' ' && email[i] != '\r' && email[i] != '\t' && email[i] != '\n') {
                return (first_index > 0 && second_index < (strlen(email) - 1));
            }
        }
    }
    return 0;
}

int password_op(user *user1) {
    const char *entry_type[] = {"my_own", "generate_random"};
    int choice = 0;
    clear();
    while (1) {
        move(LINES / 2 - 1, COLS / 2 - 1);
        int x = getcurx(stdscr);
        int y = getcury(stdscr);
        mvprintw(1, 1, "Type Of having password: ");
        for (int i = 0; i < 2; ++i) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(y + i - 1, x, "%s", entry_type[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 1 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 1) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    clear();
    switch (choice) {
        case 0:
            return 1;
        case 1:
            int length = rand() % 5 + 7;
            generate_password(length, user1);
            break;
    }
    refresh();
    return 0;
}

void generate_password(int length, user *user1) {
    char password[length];
    const char lower[] = "abcdefghijklmnopqrstuvwxyz";
    const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[] = "0123456789";
    const char all[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    srand(time(NULL));

    // Ensure the password contains at least one of each type
    password[0] = lower[rand() % (sizeof(lower) - 1)];
    password[1] = upper[rand() % (sizeof(upper) - 1)];
    password[2] = digits[rand() % (sizeof(digits) - 1)];

    // Fill the remaining slots randomly from all possible characters
    for (int i = 3; i < length; i++) {
        password[i] = all[rand() % (sizeof(all) - 1)];
    }

    // Shuffle the password for randomness
    for (int i = 0; i < length; i++) {
        int randomIndex = rand() % length;
        char temp = password[i];
        password[i] = password[randomIndex];
        password[randomIndex] = temp;
    }

    // Null-terminate the password string
    password[length] = '\0';

    // Print the generated password
    clear();
    int y = LINES / 2 - 171;
    int x = COLS / 2 - 1;
    move(y, x);
    strcpy((*user1).password, password);
    printw("**%s**", password);
}

int password_forget_or_not(user *user1) {
    const char *entry_type[] = {"enter", "forget_password"};
    int choice = 0;
    clear();
    while (1) {
        move(LINES / 2 - 1, COLS / 2 - 1);
        int x = getcurx(stdscr);
        int y = getcury(stdscr);
        mvprintw(1, 1, "Type Of entering password: ");
        for (int i = 0; i < 2; ++i) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(y + i - 1, x, "%s", entry_type[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 1 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 1) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    clear();
    switch (choice) {
        case 0:
            return 0;
        case 1:
            get_password_with_username(user1);
            clear();
            sleep(1);
            printw("your password is : %s", user1->password);
            break;
    }
    refresh();
    return 1;
}
