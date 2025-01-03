#include <ncurses.h>
#include <time.h>
#include "database.c"
#include <stdlib.h>
#include <string.h>
#include "wchar.h"
#include "locale.h"
#include "new_game.c"

#define MAX_AUDIO_SIZE 1048576 // Maximum audio size (1 MB for example)

typedef struct {
    int hard_ness;
    int hero_color_pair;
    unsigned char *audio; // Pointer to hold binary audio data
    size_t audio_size;    // Size of the audio data
} Settings;


Settings settings = {
        .hard_ness = 1,
        .hero_color_pair = 0,
        .audio = NULL,
        .audio_size = 0
};

#define MAX_INPUT_LENGTH 100

void load_audio(const char *file_path, Settings *settings);

void previous_game();

void scores_table_func(char *username, int start);

void setings();

void my_profile();

void create_input_box(int y, int x, const char *prompt, char *input) {
    int len = strlen(prompt);
    int box_width = MAX_INPUT_LENGTH + len + 2;

    mvprintw(y + 1, x, "%s", prompt);


    move(y + 2, x + 1);

    wgetstr(stdscr, input);
    printw("%s", input);
    refresh();
}

void second_menu(char *username) {
    const char *entry_type[] = {"new_game", "previous_game", "scores_table", "settings", "my_profile"};
    int choice = 0;
    while (1) {
        move(LINES / 2 - 1, COLS / 2 - 1);
        int x = getcurx(stdscr);
        int y = getcury(stdscr);
        mvprintw(1, 1, "which one do you want :");
        for (int i = 0; i < 5; ++i) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(y + i - 1, x, "%s", entry_type[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP)
            choice = (choice == 0) ? 4 : choice - 1;
        else if (ch == KEY_DOWN)
            choice = (choice == 4) ? 0 : choice + 1;
        else if (ch == 10)
            break;
    }
    clear();
    switch (choice) {
        case 0:
            new_game();
            break;
        case 1:
            previous_game();
            break;
        case 2:
            scores_table_func(username, 0);
            break;
        case 3:
            setings();
            break;
        case 4:
            my_profile(username);
            break;
    }
    refresh();
}

void previous_game() {

}

void scores_table_func(char *username, int start) {
    raw();
    scores_table dto = get_scores_table();
    table_dto *dtos = dto.users;
    mvprintw(1, 1, "scores table :");
    mvprintw(2, 1, "press n to go next page :");
    move(5, 2);
    int x = getcurx(stdscr);
    int y = getcury(stdscr);
    for (int i = start; i < start + 4; ++i) {
        if (i >= dto.size)
            return;
        int j = i - start;

//        const char *earliest_game_time = (dtos + i)->earliest_game_time;
//        struct tm game_time = {0};
//        sscanf(earliest_game_time, "%4d-%2d-%2d %2d:%2d:%2d",
//               &game_time.tm_year, &game_time.tm_mon, &game_time.tm_mday,
//               &game_time.tm_hour, &game_time.tm_min, &game_time.tm_sec);
//        time_t game_time_epoch = mktime(&game_time);
//
//        time_t now = time(NULL);
//
//        double difference_in_seconds = difftime(now, game_time_epoch);
//
//        int days = (int) difference_in_seconds / (24 * 3600);
//        int hours = (int) (difference_in_seconds / 3600) % 24;
//        int minutes = (int) (difference_in_seconds / 60) % 60;
//        int seconds = (int) difference_in_seconds % 60;

        if (!strcmp((dtos + i)->username, username))
            attron(A_BOLD);
        if (i == 0) {

            strcat((dtos + i)->username, " first 🥇");
            attron(A_BOLD);
            start_color();
            init_pair(1, COLOR_BLUE, COLOR_BLACK);
            attron(COLOR_PAIR(1));
        } else if (i == 1) {
            strcat((dtos + i)->username, " second 🥈");
            attron(A_BOLD);
            start_color();
            init_pair(2, COLOR_GREEN, COLOR_BLACK);
            attron(COLOR_PAIR(2));
        } else if (i == 2) {
            strcat((dtos + i)->username, " third 🥉");
            attron(A_BOLD);
            start_color();
            init_pair(3, COLOR_YELLOW, COLOR_BLACK);
            attron(COLOR_PAIR(3));
        }
        mvprintw(y + 0 + 6 * j - 1, x + 2, "Rank:%d", i + 1);
        mvprintw(y + 1 + 6 * j - 1, x + 2, "username : %s", (dtos + i)->username);
        mvprintw(y + 2 + 6 * j - 1, x + 2, "play_count : %d", (dtos + i)->pla_count);
        mvprintw(y + 3 + 6 * j - 1, x + 2, "Golds : %d", (dtos + i)->gold);
        mvprintw(y + 4 + 6 * j - 1, x + 2, "total_score : %d", (dtos + i)->score);
        mvprintw(y + 4 + 6 * j - 1, x + 2, "total_score : %s", (dtos + i)->earliest_game_time);
        refresh();
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(3));
//        mvprintw(y + 5 + 6 * i - 1, x + 2, "Time difference: %d days, %d hours, %d minutes, %d seconds",
//                 days, hours, minutes, seconds);
    }
    char ch = getch();
    if (ch == 'n') {
        clear();
        scores_table_func(username, start + 4);
    }
}

void setings() {
    char hard_ness[10];
    char hero_color[10];
    create_input_box(LINES / 2 - 1, COLS / 2 - 1, "hard_ness{1,2,3}", hard_ness);
    create_input_box(LINES / 2 + 2, COLS / 2 - 1, "hero_color{1,2,3}", hero_color);
    load_audio("audio_file.wav", &settings);
    settings.hard_ness = atoi(hard_ness);
    settings.hero_color_pair = atoi(hero_color);
    if (settings.audio) {
        printw("Audio is loaded and ready to use.\n");
    }

}

void my_profile(char *username) {
    dto *dto = get_user_with_username(username);
    move(LINES / 2 - 1, COLS / 2 - 1);
    int x = getcurx(stdscr);
    int y = getcury(stdscr);
    mvprintw(1, 1, "Your profile:");
    mvprintw(y + 0 - 1, x, "username : %s", dto->username);
    mvprintw(y + 1 - 1, x, "email : %s", dto->email);
    mvprintw(y + 2 - 1, x, "Golds : %d", dto->gold);
    mvprintw(y + 3 - 1, x, "total_score : %d", dto->score);
}

void load_audio(const char *file_path, Settings *settings) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open audio file");
        return;
    }

    // Allocate memory for the audio buffer
    settings->audio = malloc(MAX_AUDIO_SIZE);
    if (!settings->audio) {
        fprintf(stderr, "Failed to allocate memory for audio\n");
        fclose(file);
        return;
    }

    // Read the audio data
    settings->audio_size = fread(settings->audio, 1, MAX_AUDIO_SIZE, file);
    if (settings->audio_size == 0) {
        fprintf(stderr, "Failed to read audio file\n");
        free(settings->audio);
        settings->audio = NULL;
    }

    fclose(file);
    printf("Audio loaded, size: %zu bytes\n", settings->audio_size);
}
