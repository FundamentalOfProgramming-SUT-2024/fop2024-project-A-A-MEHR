#ifndef FILE_B_H
#define FILE_B_H
#include "init_game.c"
#include "2_floor.c"
#include "3_floor.c"
#include "4_floor.c"
//#include "treasure_room.c"
#endif

void first(char *username) {
    first_floor(username, 0);
}

void second(char *username) {
   second_floor(username, 0);
}

void third(char *username) {
    third_floor(username, 0);
}
