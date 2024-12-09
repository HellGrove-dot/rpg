#include "../libs/azhypa_rpg.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include<string.h>

void load_game(const char *filename, int *screeny, int *screenx, char map[*screeny][*screenx], int *lvl, int *mapcount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        erase();
        mvprintw(*screeny / 2, *screenx / 2 - 10, "ERROR LOADING");
        refresh();
        napms(1000);
        clear();
        return;
    }

    char line[256];
    int temp_screeny = 0, temp_screenx = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "LEVEL:", 6) == 0) {
            sscanf(line, "LEVEL: %d", lvl);
        } else if (strncmp(line, "MAP:", 4) == 0) {
            sscanf(line, "MAP: %d", mapcount);
        } else if (strncmp(line, "MAX_Y:", 6) == 0) {
            sscanf(line, "MAX_Y: %d", &temp_screeny);
        } else if (strncmp(line, "MAX_X:", 6) == 0) {
            sscanf(line, "MAX_X: %d", &temp_screenx);
        } else if (strncmp(line, "MAP_DATA:", 9) == 0) {
            for (int i = 0; i < temp_screeny; i++) {
                fgets(line, sizeof(line), file);
                for (int j = 0; j < temp_screenx; j++) {
                    map[i][j] = line[j];
                }
            }
        }
    }

    fclose(file);

    *screeny = temp_screeny;
    *screenx = temp_screenx;

    erase();
    mvprintw(*screeny / 2, *screenx / 2 - 5, "GAME LOADED");
    refresh();
    napms(1000);
    clear();
}


void save_game(const char *filename, const int screeny, const int screenx, char map[screeny][screenx], int lvl, int mapcount) {
    FILE *file = fopen(filename, "w");
    if (!file) 
    {
        erase();
        mvprintw(screeny/2, screenx/2-10, "ERROR SAVING");
        refresh();
        napms(1000);
        clear();
        return;
    }

    time_t now = time(NULL);
    char *time_now = ctime(&now);

    fprintf(file, "LEVEL: %d\n", lvl);
    fprintf(file, "MAP: %d\n", mapcount);
    fprintf(file, "MAX_Y: %d\n", screeny);
    fprintf(file, "MAX_X: %d\n", screenx);
    fprintf(file, "MAP_DATA:\n");
    for (int i = 0; i < screeny; i++) {
        for (int j = 0; j < screenx; j++) {
            fputc(map[i][j], file);
        }
        fputc('\n', file);
    }
    fprintf(file, "Game saved at: %s", time_now);
    fclose(file);
    erase();
    mvprintw(screeny/2, screenx/2-5, "GAME IS SAVED");
    refresh();
    napms(1000);
    print_map(screeny, screenx, map);
}

void spawn_zombie(int screeny, int screenx, char screen[screeny][screenx], const int count)
{
    int x,y;
    for (int  i = 0; i < count; i++)
    {
        do
        {
            x = 1 + rand() % (screenx  - 1);
            y = 2 + rand() % (screeny - 3);

        } while (screen[y][x]=='#');
        screen[y][x]='@';
    }
    
}

void attack(int x, int y, int screeny, int screenx, char screen[screeny][screenx]) 
{
    int radius = 3;
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            int new_x = x + dx;
            int new_y = y + dy;
            if (new_x >= 0 && new_x < screenx && new_y >= 0 && new_y < screeny && (dx * dx + dy * dy <= radius * radius)) 
            {
                if (screen[new_y][new_x] == '*') 
                {
                    bool random = rand()%2;
                    attron(COLOR_PAIR(1));  
                    mvaddch(new_y, new_x, 'X'); 
                    refresh();              
                    napms(100);             
                    attroff(COLOR_PAIR(1));
                    if(random)
                    {
                        screen[new_y][new_x] = '+'; 
                        mvaddch(new_y, new_x, '+'); 
                    } 
                    else
                    {
                        screen[new_y][new_x] = ' '; 
                        mvaddch(new_y, new_x, ' '); 
                    }
                    refresh();
                }
            }
        }
    }
}


void spawn_fly(int screeny, int screenx, char screen[screeny][screenx])
{
    int x, y;
    do
    {
        x = rand() % screenx;
        y = rand() %screeny;
    } while (y< 1 || screen[y][x]=='#');
    screen[y][x] = '*';
    print_map(screeny, screenx, screen);
}

void move_alive(int screeny, int screenx, char screen[screeny][screenx], int x, int y) {
    char temp_screen[screeny][screenx];
    int is_move;
    for (int i = 0; i < screeny; i++) 
    {
        for (int j = 0; j < screenx; j++) 
        {
            temp_screen[i][j] = screen[i][j];
        }
    }

    for (int i = 0; i < screeny; i++) 
    {
        for (int j = 0; j < screenx; j++) 
        {
            if (screen[i][j] == '@') 
            {
                is_move = rand()%4;
                if(is_move==3)
                {
                    int new_i = i, new_j = j;
                    if (i < y) new_i++;
                    else if (i > y) new_i--;
                    else if (j < x) new_j++;
                    else if (j > x) new_j--;

                    if (temp_screen[new_i][new_j] == ' ') 
                    {
                        temp_screen[new_i][new_j] = '@';
                        temp_screen[i][j] = ' ';
                    }
                }
            }
        }
    }

    for (int i = 0; i < screeny; i++) {
        for (int j = 0; j < screenx; j++) {
            if (screen[i][j] == '*') {
                int direction = rand() % 4, new_i = i, new_j = j;

                switch (direction) {
                    case 0: new_i--; break;
                    case 1: new_i++; break;
                    case 2: new_j--; break;
                    case 3: new_j++; break;
                }

                if (new_i >= 0 && new_i < screeny && new_j >= 0 && new_j < screenx &&
                    temp_screen[new_i][new_j] == ' ') {
                    temp_screen[new_i][new_j] = '*';
                    temp_screen[i][j] = ' ';
                }
            }
        }
    }

    for (int i = 0; i < screeny; i++) {
        for (int j = 0; j < screenx; j++) {
            screen[i][j] = temp_screen[i][j];
        }
    }
}



void print_button(int screeny, int screenx) 
{
    int sizey = 4;
    int sizex = 10;
    int x = screenx / 2 - sizex / 2;
    int y = screeny / 2 - sizey / 2;

    for (int i = 0; i < sizey; i++) 
    {
        for (int j = 0; j < sizex; j++) 
        {
            if (i == 0 || i == sizey - 1 || j == 0 || j == sizex - 1)
            {
                attron(COLOR_PAIR(3));
                mvaddch(y + i, x + j, '#'); 
                attroff(COLOR_PAIR(3)); 
            }
            else mvaddch(y + i, x + j, ' '); 
        }
    }
    attron(COLOR_PAIR(3));
    for(int i = 1; i<9; i++ ) mvprintw(y+2, x+i, " ");
    mvprintw(y + 1, x + 1, "NEW GAME"); 
    attroff(COLOR_PAIR(3));
    getch();
}


void random_spawn_rock(const int screeny, const int screenx, char map[screeny][screenx], const int count) {
    if (count <= 0) return;
    int sizey = 4;
    int sizex = 3;
    int x, y;
    srand(time(NULL));
    for (int i = 0; i < count; i++) {
        do {
            x = 1 + rand() % (screenx - sizex - 1);
            y = 2 + rand() % (screeny - sizey - 3);

            int invalid = 0;
            for (int yy = 0; yy < sizey; yy++) 
            {
                for (int xx = 0; xx < sizex; xx++) 
                {
                    if (map[y + yy][x + xx] != ' ' || ((y+yy == screeny/2) && (x+xx == screenx/2))) 
                    {
                        invalid = 1;
                        break;
                    }
                }
                if (invalid) break;
            }
            if (!invalid) break;
        } while (1);

        for (int ty = 0; ty < sizey; ty++) 
        {
            for (int tx = 0; tx < sizex; tx++) 
            {
                map[y + ty][x + tx] = '#';
            }
        }
    }
}

void initialize_map(const int screen_max_y, const int screen_max_x, char map[screen_max_y][screen_max_x], const int rand_rock) {
    for (int i = 0; i < screen_max_y; i++) {
        for (int j = 0; j < screen_max_x; j++) {
            if (i == 2 || i == screen_max_y - 1 || j == 0 || j == screen_max_x - 1) 
                map[i][j] = '#';
            else
                map[i][j] = ' ';
        }
    }
    random_spawn_rock(screen_max_y, screen_max_x, map, rand_rock);
}

void print_map(const int screeny, const int screenx, char map[screeny][screenx]) {
    for (int i = 2; i < screeny; i++)
     { 
        for (int j = 0; j < screenx; j++) 
        {
            switch (map[i][j]) {
                case '@':
                    attron(COLOR_PAIR(6));
                    mvaddch(i, j, '@');
                    attroff(COLOR_PAIR(6));
                    break;
                case '*':
                    attron(COLOR_PAIR(5));
                    mvaddch(i, j, '*');
                    attroff(COLOR_PAIR(5));
                    break;
                case '#':
                    attron(COLOR_PAIR(3));
                    mvaddch(i, j, '#');
                    attroff(COLOR_PAIR(3));
                    break;
                default:
                    mvaddch(i, j, map[i][j]);
            }
        }
    }
}


void print_interface(const int screeny, const int screenx, const int y, const int x, const int level, const int map) {
    mvprintw(0, 2, "SIZE: %dx%d\t Level: %d", screenx, screeny, level); 
    mvprintw(1, 2, "X:Y = %d:%d\t Map: %d", x, y, map);            
    mvprintw(0, screenx - 25, "Press Backspace to exit"); 
    mvprintw(1, screenx-25, "Enter 'U' to open menu");
}

void cheatcode(int screeny, int screenx, char map[screeny][screenx], int *lvl, int *mapcount, bool *is_win)
{
    char cheatcode[20];
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(screeny / 2, screenx / 2 - 15, "Enter cheatcode: ");
    curs_set(1);
    echo();
    getnstr(cheatcode, 20);
    attroff(COLOR_PAIR(1));
    noecho();
    curs_set(0);
    if (strcmp(cheatcode, "reset") == 0) 
    {
        initialize_map(screeny, screenx, map, ROCK_COUNT);
        clear();
        print_map(screeny, screenx, map);
    } 
    else 
        if (strcmp(cheatcode, "lvlup") == 0) (*lvl)++;
        else if (strcmp(cheatcode, "nextmap") == 0) 
        { 
            (*mapcount)++; 
            (*lvl)=1;
            initialize_map(screeny, screenx, map, ROCK_COUNT); 
        }
        else if(strcmp(cheatcode, "restart")==0) 
        { 
            start_azhypa_rpg(); 
            exit(1); 
        }
        else if(strcmp(cheatcode, "ggwp")==0) (*is_win) =true;
        else
        {
            erase();
            attron(COLOR_PAIR(1));
            mvprintw(screeny / 2, screenx / 2 - 10, "Invalid cheatcode!");
            attroff(COLOR_PAIR(1));
            refresh(); 
            napms(1000); 
        }
    erase(); 
    print_map(screeny, screenx, map); 
}

bool is_screen_correct(int screeny, int screenx)
{
    if(screeny<24 || screenx<70)
    {
        erase();
        attron(COLOR_PAIR(1));
        mvprintw(screeny/2, screenx/2-7, "Min size: 80:24\n");
        attroff(COLOR_PAIR(1));
        getch();
        endwin();
        return 0;
    }
    else return 1;
}

void print_menu(int screeny, int screenx)
{
    clear();
    int x,y;
    x = screenx/2/2;
    y = screeny/2-2;
    attron(COLOR_PAIR(4));
    mvprintw(y,x,"SAVE - 'S'\t\tLOAD - 'D'");
    mvprintw(y+2,x,"CHEATCODE - '`'*");
    mvprintw(y+4,x, "Code:\t 'reset' - generate new map");
    mvprintw(y+5,x, "\t\t 'lvlup' - You increase your lvl by 1");
    mvprintw(y+6,x, "\t\t 'restart' - restart game");
    attroff(COLOR_PAIR(4));
    getch();
    erase();
}

void initialization_screen()
{
    srand(time(NULL));
    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, 1);
    clear();

    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
}

void start_azhypa_rpg()
{
    initialization_screen();

    int screenx, screeny;
    getmaxyx(stdscr, screeny, screenx);
    char map[screeny][screenx];
    if(!is_screen_correct(screeny, screenx)) return;

    print_button(screeny,screenx);

    initialize_map(screeny, screenx, map, ROCK_COUNT);
    spawn_zombie(screeny, screenx, map, ZOMBIE_COUNT);

    int button;
    char symbol = '&';

    int x = screenx / 2;
    int y = screeny / 2;

    int time=0;

    int lvl=1, mapcount=1, monstercount = 0;
    bool key_is_picked=false, is_win = false;
    print_map(screeny, screenx, map);
    do 
    {
        if(map[y][x]=='@') break;
        time++;
        if(time==100) time = 0;

        if(map[y][x]=='+') { map[y][x]=' '; lvl++; }
        print_interface(screeny, screenx, y, x, lvl, mapcount);
        if(time%20==0) { spawn_fly(screeny, screenx, map); monstercount++; }
        if(monstercount>0) { move_alive(screeny, screenx, map, x, y); print_map(screeny, screenx, map); }
        attron(COLOR_PAIR(2));
        mvaddch(y, x, symbol); 
        attroff(COLOR_PAIR(2));


        button = getch();

        mvaddch(y,x,' ');

        if (button == KEY_RIGHT && map[y][x + 1] != '#') x++;
        else if (button == KEY_LEFT && map[y][x - 1] != '#') x--;
        else if (button == KEY_UP && map[y - 1][x] != '#') y--;
        else if (button == KEY_DOWN && map[y + 1][x] != '#') y++;
        else if(button == ' ') attack(x,y,screeny,screenx, map);
        else if (button == '`') cheatcode(screeny, screenx, map, &lvl, &mapcount, &is_win);
        else if(button=='u'||button=='U') { print_menu(screeny,screenx); print_map(screeny,screenx, map); }
        else if(button == 's' || button == 'S') save_game("log.txt", screeny, screenx, map, lvl, mapcount);
        else if(button=='d' || button=='D') { load_game("log.txt", &screeny, &screenx, map, &lvl, &mapcount); print_map(screeny, screenx, map); }

        if(lvl>5) 
        { 
            lvl = 1; 
            mapcount++; 
            if(mapcount<3)
            {
                erase(); 
                attron(COLOR_PAIR(2));
                mvprintw(screeny/2, screenx/2-4, "NEXT MAP <3");
                refresh();
                napms(1000);
                attroff(COLOR_PAIR(2));
                erase();
                initialize_map(screeny, screenx, map, ROCK_COUNT); 
                spawn_zombie(screeny, screenx, map, ZOMBIE_COUNT);
                print_map(screeny,screenx, map); 
            }
        };
        if(mapcount>2) is_win=true;
        if(is_win) break;
        refresh();
    } while (button != KEY_BACKSPACE);

    if(is_win)
    {
        attron(COLOR_PAIR(4));
        clear();
        mvprintw(screeny / 2, screenx / 2 - 5, "You win!!!"); 
        attroff(COLOR_PAIR(4));
    }
    else
    {
        attron(COLOR_PAIR(1));
        clear();
        mvprintw(screeny / 2, screenx / 2 - 5, "Game over!"); 
        attroff(COLOR_PAIR(1));
    }
    getch();
    endwin();
    return;
}