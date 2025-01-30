#include <ncurses.h>
#include <time.h>
//#include "database.c"
#include <stdlib.h>
#include <string.h>
#include "wchar.h"
#include "locale.h"
#include "init_game.c"
#include <stdbool.h>

void *play_music();

volatile bool is_paused = false; // Tracks whether the music is paused

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
    const char *entry_type[] = {"new_game", "previous_game_sf", "scores_table", "settings", "my_profile"};
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
            insert_new_game(username);
            first_floor(username, 1);
            break;
        case 1:
            previous_game_ff(username);
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
        mvprintw(y + 0 + 7 * j - 1, x + 2, "Rank:%d", i + 1);
        mvprintw(y + 1 + 7 * j - 1, x + 2, "username : %s", (dtos + i)->username);
        mvprintw(y + 2 + 7 * j - 1, x + 2, "play_count : %d", (dtos + i)->pla_count);
        mvprintw(y + 3 + 7 * j - 1, x + 2, "Golds : %d", (dtos + i)->gold);
        mvprintw(y + 4 + 7 * j - 1, x + 2, "total_score : %d", (dtos + i)->score);
        mvprintw(y + 5 + 7 * j - 1, x + 2, "play_experience : %s", (dtos + i)->earliest_game_time);
        refresh();
        attroff(A_BOLD);
        attroff(COLOR_PAIR(1));
        attroff(COLOR_PAIR(2));
        attroff(COLOR_PAIR(3));
//        mvprintw(y_sf + 5 + 6 * i - 1, x_sf + 2, "Time difference: %d days, %d hours, %d minutes, %d seconds",
//                 days, hours, minutes, seconds);
    }
    char ch = getch();
    if (ch == 'n') {
        clear();
        scores_table_func(username, start + 4);
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

void setings() {
    char hard_ness[10];
    char hero_color[10];
    char song_choice[10];
    create_input_box(LINES / 2 - 5, COLS / 2 - 10, "Choose difficulty (1, 2, 3):", hard_ness);
    int difficulty = atoi(hard_ness);
    if (difficulty < 1 || difficulty > 3) {
        printw("Invalid difficulty! Defaulting to 1.\n");
        difficulty = 1;
    }
    // Ask the user to choose between two songs
    create_input_box(LINES / 2 - 3, COLS / 2 - 1, "Choose song (1 or 2):", song_choice);

    int song_number = atoi(song_choice);
    if (song_number != 1 && song_number != 2) {
        printw("Invalid choice! Defaulting to song 1.\n");
        song_number = 1;
    }

    // Determine the song to play based on user input
    const char *song_file = (song_number == 1) ? "God_of_War.wav" : "audio_file.wav";

    load_audio(song_file, &settings);
    if (settings.audio) {
        printw("Audio is loaded and ready to use: %s\n", song_file);
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, play_music, (void *) song_file);

    // Listen for user input to pause/resume
    while (1) {
        int ch = getch();
        if (ch == 'p') {
            is_paused = true;
            printw("Music paused.\n");
        } else if (ch == 'r') {
            is_paused = false;
            printw("Music resumed.\n");
        } else if (ch == 'q') {
            break; // Quit the settings menu
        }
    }

    // Wait for the music thread to finish
    pthread_join(thread_id, NULL);
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
    //  printf("Audio loaded, size: %zu bytes\n", settings->audio_size);

}

#include <stdio.h>
#include <SDL2/SDL.h>

#include <pthread.h>

void *play_music(void *arg) {
    const char *file_path = (const char *) arg;

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return NULL;
    }

    // Load the WAV file
    SDL_AudioSpec wav_spec;
    Uint8 *wav_buffer;
    Uint32 wav_length;

    if (SDL_LoadWAV(file_path, &wav_spec, &wav_buffer, &wav_length) == NULL) {
        printf("SDL_LoadWAV Error: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    // Open the audio device
    if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
        printf("SDL_OpenAudio Error: %s\n", SDL_GetError());
        SDL_FreeWAV(wav_buffer);
        SDL_Quit();
        return NULL;
    }

    // Queue and play the WAV file
    SDL_QueueAudio(1, wav_buffer, wav_length);
    SDL_PauseAudio(0); // Start playback

    // Monitor the pause/resume state
    while (SDL_GetQueuedAudioSize(1) > 0) { // While audio is still playing
        if (is_paused) {
            SDL_PauseAudio(1); // Pause playback
        } else {
            SDL_PauseAudio(0); // Resume playback
        }
        SDL_Delay(100); // Check the state periodically
    }

    // Clean up
    SDL_FreeWAV(wav_buffer);
    SDL_CloseAudio();
    SDL_Quit();

    return NULL;
}


