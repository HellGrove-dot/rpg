#ifndef AZHYPA_RPG_H
#define AZHYPA_RPG_H

#define ROCK_COUNT 15
#define ZOMBIE_COUNT 10

#include<stdbool.h>

void start_azhypa_rpg();
void print_button(int screeny, int screenx);
void random_spawn_rock(const int screeny, const int screenx, char map[screeny][screenx], const int count);
void initialize_map(const int screen_max_y, const int screen_max_x, char map[screen_max_y][screen_max_x], const int rand_rock);
void print_map(const int screeny, const int screenx, char map[screeny][screenx]);
void cheatcode(int screeny, int screenx, char map[screeny][screenx], int *lvl, int *mapcount, bool *is_win);
bool is_screen_correct(int screeny, int screenx);
void print_menu(int screeny, int screenx);
void spawn_fly(int screeny, int screenx, char screen[screeny][screenx]);
void move_alive(int screeny, int screenx, char screen[screeny][screenx], int x, int y);
void spawn_zombie(int screeny, int screenx, char screen[screeny][screenx], const int count);
void initialization_screen();
void attack(int x, int y, int screeny, int screenx, char screen[screeny][screenx]);
void save_game(const char *filename, const int screeny, const int screenx, char map[screeny][screenx], int lvl, int mapcount);
void load_game(const char *filename, int *screeny, int *screenx, char map[*(screeny)][*(screenx)], int *lvl, int *mapcount);

#endif