///
/// Exercise: O6
///
/// file: PacMan_OOP.cpp
///
/// PacMan project with a Ghost class.
///
/// ---
/// Prof. Dr.-Ing. J?rgen Brauer
///

//To compile with G++:
	//g++ -o pacman pacman.cpp -lcurses
	
//TODO:
	//fixed window size
	//refreshes for ghost/player movement?
	//pellet refreshes?

#include <stdio.h> // printf()
//#include <conio.h> // getch()
#define _WIN32_WINNT 0x0500
//#include <windows.h>
#include <stdbool.h>
#include <vector>
#include <unistd.h>
//#include <ncurses.h>
#include <stdlib.h>
#include "mpi.h"

#define H 30
#define W 60
#define NR_GHOSTS_START 5
#define PACMAN_SYMBOL 'P'
#define GHOST_SYMBOL  'G'
#define FOOD_SYMBOL '.'
#define WALL_SYMBOL '#'
#define DOOR_SYMBOL '-'
#define EMPTY_SYMBOL ' '
#define NO_SPAWN_SYMBOL 'X'

int N = 64;

//ncurses stuff
int px, py;
//WINDOW *wui;

bool collision_with_ghost = false;
bool game_over = false;
int foodToWin = 0;

void resetGhosts();

struct coord
{
	int x;
	int y;
};

struct PacMan {
	struct coord position;
	int vx;
	int vy;
	int lives;
	bool chasing;
	int food_collected;
};


struct PacMan myPacMan;
//Saves field from inital startup.
char resetField[H + 1][W + 1];
//Pac-man is on a separate layer.
char playerField[H + 1][W + 1];

//Will hold food and display all characters
char playfield[H + 1][W + 1] =
{
{ "############################################################" },
{ "#                       ############                       #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "#                                                          #" },
{ "# ######### ## ############################## ## ######### #" },
{ "#           ## ############################## ##           #" },
{ "########### ##              ####              ## ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ##                                ## ###########" },
{ "########### ## #---#########----#########---# ## ###########" },
{ "#              #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#              X" },
{ "X              #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#              X" },
{ "X              #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#              #" },
{ "########### ## ############################## ## ###########" },
{ "########### ##                                ## ###########" },
{ "########### ## ############################## ## ###########" },
{ "########### ## ############################## ## ###########" },
{ "#                           ####                           #" },
{ "# ######### ############### #### ############### ######### #" },
{ "# ######### ############### #### ############### ######### #" },
{ "#        ##                                      ##        #" },
{ "######## ## ## ############################## ## ## ########" },
{ "######## ## ## ############################## ## ## ########" },
{ "#           ##              ####              ##           #" },
{ "# ######################### #### ######################### #" },
{ "# ######################### #### ######################### #" },
{ "#                                                          #" },
{ "############################################################" }
}; // <-- CAUTION! Semicolon necessary!

//Ghost layer
char ghostField[H + 1][W + 1] =
{
{ "############################################################" },
{ "#                       ############                       #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "#                                                          #" },
{ "# ######### ## ############################## ## ######### #" },
{ "#           ## ############################## ##           #" },
{ "########### ##              ####              ## ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ##                                ## ###########" },
{ "########### ## #---#########----#########---# ## ###########" },
{ "##             #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#             ##" },
{ "##             #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#             ##" },
{ "##             #XXXXXXXXXXXXXXXXXXXXXXXXXXXX#             ##" },
{ "########### ## ############################## ## ###########" },
{ "########### ##                                ## ###########" },
{ "########### ## ############################## ## ###########" },
{ "########### ## ############################## ## ###########" },
{ "#                           ####                           #" },
{ "# ######### ############### #### ############### ######### #" },
{ "# ######### ############### #### ############### ######### #" },
{ "#        ##                                      ##        #" },
{ "######## ## ## ############################## ## ## ########" },
{ "######## ## ## ############################## ## ## ########" },
{ "#           ##              ####              ##           #" },
{ "# ######################### #### ######################### #" },
{ "# ######################### #### ######################### #" },
{ "#                                                          #" },
{ "############################################################" }
}; // <-- CAUTION! Semicolon necessary!
/*
{ "############################################################" },
{ "#                                                          #" },
{ "# ########  ######## #            ###############          #" },
{ "# ########         # #            #                        #" },
{ "#              ### # #            # ####### #####          #" },
{ "# #            # # # #            #                        #" },
{ "# # ########   # # # #            ### ###########          #" },
{ "# # # #    #   # # # #            # # #         #          #" },
{ "# # # #### #   # # # #            # # # ####### #          #" },
{ "# #        #   #   # #            # # #       # #          #" },
{ "# ##########  ###### #            # # ######### #          #" },
{ "#                    #            #             #          #" },
{ "#########  ###########            ######  #######          #" },
{ "#                                                          #" },
{ "#                     ######   ######                      #" },
{ "#                     #             #                      #" },
{ "#                     #             #                      #" },
{ "#                     #             #                      #" },
{ "#                     ###############                      #" },
{ "#                                                          #" },
{ "# ########    ############    ############  ############   #" },
{ "# ########                                             #   #" },
{ "#        #########  ######    ######################## #   #" },
{ "######## ###             #    #                      # #   #" },
{ "######## ###             #    #  #                   # #   #" },
{ "#        ###             #    #  ##################### #   #" },
{ "# ################            #                        #   #" },
{ "# ################                     #################   #" },
{ "#                            #                             #" },
{ "#############################M##############################" }
*/
//class Ghost {
//
//public:
//
//    Ghost(int start_x, int start_y)
//    {
//        this->position.x = start_x;
//        this->position.y = start_y;
//        this->chasing = true;
//        //this->lastChar = ' ';
//        choose_random_direction();
//    }
//
//    void move()
//    {
//        // 1. where does the Ghost want to move to?
//        int dx = position.x + vx;
//        int dy = position.y + vy;
//
//        // 2. what is at this place where we plan to move the Ghost to?
//        char whats_there = playfield[dy][dx];
//        char whats_there2 = ghostField[dy][dx];
//        char whats_there3 = playerField[dy][dx];
//        // 3. is it a PacMan?
//        if (whats_there3 == PACMAN_SYMBOL)
//        {
//            collision_with_ghost = true;
//            playerField[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
//        }
//
//        // 4. is the field free to move there?
//        if (whats_there2 != WALL_SYMBOL)
//        {
//            /*
//            //Ghost won't eat spaces
//            if (playfield[dy][dx] != ' ' && playfield[dy][dx] != PACMAN_SYMBOL && playfield[dy][dx] != GHOST_SYMBOL)
//            {
//                //playfield[position.y][position.x] = lastChar;
//            }
//            else {
//                playfield[position.y][position.x] = EMPTY_SYMBOL;
//            }
//            */
//            ghostField[position.y][position.x] = EMPTY_SYMBOL;
//
//            position.x = dx;
//            position.y = dy;
//            //lastChar = playfield[position.y][position.x];
//            ghostField[position.y][position.x] = GHOST_SYMBOL;
//        }
//        else
//        {
//            // field is not free, i.e. a wall:
//            // so do not move Ghost at all in this step, but
//            // randomly choose a new direction to move
//            choose_random_direction();
//            //choose_smart_path();
//        }
//
//        // randomize behavior of the Ghost
//        if (rand() % 30 == 0)
//        {
//
//            choose_random_direction();
//            //choose_smart_path();
//        }
//
//    } // move
//
//
//    void choose_random_direction()
//    {
//        int r = rand() % 4;
//        switch (r)
//        {
//        case 0: vx = +1; vy = 0;  break;
//        case 1: vx = 0;  vy = 1;  break;
//        case 2: vx = -1; vy = 0;  break;
//        case 3: vx = 0;  vy = -1; break;
//        }
//    }
//
//    void smart_direction() {
//        bool canRight = false;
//        bool canLeft = false;
//        bool canDown = false;
//        bool canUp = false;
//
//        if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
//        {
//            canUp = true;
//        }
//        if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
//        {
//            canDown = true;
//        }
//        if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
//        {
//            canRight = true;
//        }
//        if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
//        {
//            canLeft = true;
//        }
//
//        if (canUp && !canDown && !canRight && !canLeft) {
//            //up
//            vx = 0; vy = 1;
//        }
//        else if (!canUp && canDown && !canRight && !canLeft) {
//            //down
//            vx = 0; vy = -1;
//        }
//        else if (!canUp && !canDown && canRight && !canLeft) {
//            //right
//            vx = 1; vy = 0;
//        }
//        else if (!canUp && !canDown && !canRight && canLeft) {
//            //left
//            vx = -1; vy = 0;
//        }
//        else if (canUp && canDown && !canRight && !canLeft) {
//            //up and down
//
//            //random int % 2
//            //if int == 0
//                //chase pacman (need to pass pacman's position into this function)
//            //else
//                //random direction out of the available directions
//        }
//        else if (!canUp && !canDown && canRight && canLeft) {
//            //right and left
//        }
//        else if (!canUp && canDown && !canRight && canLeft) {
//            //down and left
//        }
//        else if (!canUp && canDown && canRight && !canLeft) {
//            //down and right
//        }
//        else if (canUp && !canDown && !canRight && canLeft) {
//            //up and left
//        }
//        else if (canUp && !canDown && canRight && !canLeft) {
//            //up and right
//        }
//        else if (!canUp && canDown && canRight && canLeft) {
//            //all but up
//        }
//        else if (canUp && !canDown && canRight && canLeft) {
//            //all but down
//        }
//        else if (canUp && canDown && !canRight && canLeft) {
//            //all but right
//        }
//        else if (canUp && canDown && canRight && !canLeft) {
//            //all but left
//        }
//        else if (canUp && canDown && canRight && canLeft) {
//            //all directions
//        }
//        else {
//            choose_random_direction();
//        }
//    }
//
//    void choose_smart_path()
//    {
//        /*
//        int r = rand() % 2;
//        //If Right
//        if (vx == 1 || vx == -1)
//        {
//            switch (r)
//            {
//            case 0: vx = 0; vy = 1;  break;
//            case 1: vx = 0;  vy = -1;  break;
//            }
//        }
//        else if (vy == 1 || vy == -1)
//        {
//            switch (r)
//            {
//            case 0: vx = 1; vy = 0;  break;
//            case 1: vx = -1;  vy = 0;  break;
//            }
//        }
//        */
//        bool canRight = false;
//        bool canLeft = false;
//        bool canDown = false;
//        bool canUp = false;
//        //printf("      (%d,%d)       ",vx, vy);
//        if (vx == 1)
//        {
//            if (ghostField[position.y+1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = 1;
//                canUp = true;
//            }
//            if (ghostField[position.y-1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = -1;
//                canDown = true;
//            }
//            if (ghostField[position.y][position.x-1] != WALL_SYMBOL)
//            {
//                //vx = -1; vy = 0;
//                canLeft = true;
//            }
//        }
//        else if (vy == 1)
//        {
//            if (ghostField[position.y][position.x+1] != WALL_SYMBOL)
//            {
//                //vx = 1; vy = 0;
//                canRight = true;
//            }
//            if (ghostField[position.y][position.x-1] != WALL_SYMBOL)
//            {
//                //vx = -1; vy = 0;
//                canLeft = true;
//            }
//            if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = -1;
//                canDown = true;
//            }
//        }
//        else if (vy == -1)
//        {
//            if (ghostField[position.y][position.x-1] != WALL_SYMBOL)
//            {
//                //vx = -1; vy = 0;
//                canLeft = true;
//            }
//            if (ghostField[position.y][position.x+1] != WALL_SYMBOL)
//            {
//                //vx = 1; vy = 0;
//                canRight = true;
//            }
//            if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = 1;
//                canUp = true;
//            }
//        }
//        else
//        {
//            if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = 1;
//                canUp = true;
//            }
//            if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
//            {
//                //vx = 0; vy = -1;
//                canDown = true;
//            }
//            if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
//            {
//                //vx = 1; vy = 0;
//                canRight = true;
//            }
//        }
//        bool chosePath = false;
//        int r = rand() % 4;
//        while (!chosePath)
//        {
//            int r = rand() % 4;
//            switch (r)
//            {
//            case 0:
//                if (canRight)
//                {
//                    vx = 1; vy = 0;
//                    chosePath = true;
//                }
//                break;
//            case 1:
//                if (canUp)
//                {
//                    vx = 0;  vy = 1;
//                    chosePath = true;
//                }
//                break;
//            case 2:
//                if (canLeft)
//                {
//                    vx = -1; vy = 0;
//                    chosePath = true;
//                }
//                break;
//            case 3:
//                if (canLeft)
//                {
//                    vx = 0; vy = -1;
//                    chosePath = true;
//                }
//                break;
//            }
//        }
//    }
//
//
//
//private:
//    struct coord position;
//    int vx;
//    int vy;
//    bool chasing;
//    //char lastChar;
//};
//
//
//std::vector<Ghost*> allGhosts;    //What does this do?
//
//
//// Set cursor position in console
//// see http://www.dreamincode.net/forums/topic/153240-console-cursor-coordinates/
//void set_cursor_position(int x, int y)
//{
//    /*//Initialize the coordinates
//    COORD coord = { x, y };
//    //Set the position
//    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);*/
//    wmove(wui, x, y);
//    wrefresh(wui);
//    refresh();
//
//} // set_cursor_position
//
//
//void hidecursor()
//{
//    /*HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//    CONSOLE_CURSOR_INFO info;
//    info.dwSize = 100;
//    info.bVisible = FALSE;
//    SetConsoleCursorInfo(consoleHandle, &info);*/
//    curs_set(0);
//    wrefresh(wui);
//    refresh();
//}
//
//
//void add_new_ghost()
//{
//
//    int x, y;
//    // try to find a (x,y) coordinate randomly where a food piece is
//    /*
//    do
//    {
//        x = 1 + rand() % (W - 1);
//        y = 1 + rand() % (H - 1);
//    } while (playfield[y][x] != FOOD_SYMBOL);
//    */
//    //Spawn inside the Box
//    x = rand() % (42 - 17 + 1) + 17;
//    y = rand() % (14 - 12 + 1) + 12;
//    allGhosts.push_back(new Ghost(x, y));
//}
//
//void initialize()
//{
//    //Starting position
//    myPacMan.position.x = 29;
//    myPacMan.position.y = 16;
//    myPacMan.vx = 0;
//    myPacMan.vy = 0;
//    myPacMan.food_collected = 0;
//    myPacMan.lives = 3;
//    myPacMan.chasing = false;
//
//    // 1. replace each empty field in the playfield
//    //    with a food field
//    for (int i = 0; i < H; i++)
//    {
//        for (int j = 0; j < W; j++)
//        {
//            resetField[i][j] = playfield[i][j];
//            if (playfield[i][j] == EMPTY_SYMBOL)
//            {
//                playfield[i][j] = FOOD_SYMBOL;
//                foodToWin++;
//            }
//            //Replace X with ' '
//            else if (playfield[i][j] == NO_SPAWN_SYMBOL)
//            {
//                playfield[i][j] = EMPTY_SYMBOL;
//                ghostField[i][j] = EMPTY_SYMBOL;
//            }
//        }
//    }
//
//    // 2. create some ghosts
//    for (int i = 0; i < NR_GHOSTS_START; i++)
//    {
//        add_new_ghost();
//    }
//
//    // 3. initialize ncurses
//    initscr();
//    clear();
//    cbreak();
//    noecho();
//    wui = newwin(33, 62, 0, 0);
//    nodelay(wui, TRUE);
//    keypad(wui, TRUE);
//    wmove(wui, 0, 0);
//    wrefresh(wui);
//    refresh();
//} // initialize
//
////Made my own kbhit() method to replace _kbhit() from conio.h
//int kbhit(){
//    int ch = wgetch(wui);
//
//    if (ch != ERR) {
//        ungetch(ch);
//        return 1;
//    } else {
//        return 0;
//    }
//}
//
//void user_input()
//{
//    if (kbhit())
//    {
//        char c1 = wgetch(wui);
//        if (c1 == ' ')
//        {
//            // stop moving
//            myPacMan.vx = 0;
//            myPacMan.vy = 0;
//        }
//
//        if (c1 == 27)
//        {
//            exit(0);
//        }
//        if (c1 == 'g')
//        {
//            add_new_ghost();
//        }
//        if (c1 == 'k')
//        {
//            myPacMan.lives--;
//        }
//        if (c1 == 'r')
//        {
//            resetGhosts();
//        }
//        if (c1 == 'w')
//        {
//            myPacMan.food_collected = foodToWin;
//        }
//        if (c1 == -32)
//        {
//            char c2 = wgetch(wui);
//
//            //printf("c1=%d c2=%d\n", c1, c2);
//
//            myPacMan.vx = 0;
//            myPacMan.vy = 0;
//
//            switch (c2)
//            {
//            case 72: myPacMan.vy = -1; break; // cursor up
//            case 80: myPacMan.vy = +1; break; // cursor down
//            case 75: myPacMan.vx = -1; break; // cursor left
//            case 77: myPacMan.vx = +1; break; // cursor right
//            }
//        }
//
//    }
//}
//
//
//void move_ghosts()
//{
//    // 1. move all Ghosts
//    for (int i = 0; i < allGhosts.size(); i++)
//    {
//        allGhosts[i]->move();
//    }
//}
//
//void move_pacman()
//{
//
//    // 1. compute new desired coordinate (nx,ny)
//    int nx = myPacMan.vx + myPacMan.position.x;
//    int ny = myPacMan.vy + myPacMan.position.y;
//
//    // 2. delete PacMan from old position, but only remove food.
//    playerField[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
//
//    // 3. test whether there is a wall at (nx,ny)
//    if (playfield[ny][nx] == WALL_SYMBOL || playfield[ny][nx] == DOOR_SYMBOL)
//    {
//        // Damn! There is a wall! Stop PacMan!
//        myPacMan.vx = 0;
//        myPacMan.vy = 0;
//    }
//
//    // 4. update PacMan's coordinate
//    myPacMan.position.x += myPacMan.vx;
//    myPacMan.position.y += myPacMan.vy;
//
//    if (ghostField[ny][nx] == GHOST_SYMBOL) {
//        collision_with_ghost = true;
//        playerField[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
//    }
//    else
//    {
//        if (playfield[ny][nx] == FOOD_SYMBOL)
//        {
//            myPacMan.food_collected++;
//            playfield[ny][nx] = EMPTY_SYMBOL;
//        }
//
//        // put PacMan back again to playfield
//        playerField[myPacMan.position.y][myPacMan.position.x] = PACMAN_SYMBOL;
//    }
//
//} // move_pacman
//
//
//void show_playfield()
//{
//    set_cursor_position(0, 0);
//    for (int i = 0; i < H; i++)
//    {
//        for (int j = 0; j < W; j++)
//        {
//            set_cursor_position(i, j);
//            if(ghostField[i][j] == GHOST_SYMBOL)
//            {
//                printw("%c", ghostField[i][j]);
//                wrefresh(wui);
//                refresh();
//            }
//            else if (playerField[i][j] == PACMAN_SYMBOL)
//            {
//                printw("%c", playerField[i][j]);
//                wrefresh(wui);
//                refresh();
//            }
//            else
//            {
//                printw("%c", playfield[i][j]);
//                wrefresh(wui);
//                refresh();
//            }
//        }
//        printw("\n");
//    }
//    printw("Score:%3d - Remaining:%3d - Lives:%2d\n", myPacMan.food_collected, foodToWin - myPacMan.food_collected, myPacMan.lives);
//    wrefresh(wui);
//    refresh();
//}
//
//void check_collisions()
//{
//    // did a collision between PacMan and Ghost happen?
//    if (collision_with_ghost)
//    {
//        set_cursor_position(W / 2 - 9, H / 2 - 3);
//        printw("   GAME START   ");
//        set_cursor_position(W / 2 - 8, H / 2 - 2);
//        myPacMan.lives--;
//        printw("   Lifes:%d   ", myPacMan.lives );
//        set_cursor_position(W / 2 - 8, H / 2 - 1);
//        printw("   Score:%d   ", myPacMan.food_collected);
//        collision_with_ghost = false;
//        sleep(1);
//        resetGhosts();
//
//        int x, y;
//        x = 29;
//        y = 16;
//        // try to find a new (x,y) PacMan start coordinate randomly
//        /*
//        do
//        {
//            x = 1 + rand() % (W - 1);
//            y = 1 + rand() % (H - 1);
//
//        } while ((playfield[y][x] != FOOD_SYMBOL) && (playfield[y][x] != EMPTY_SYMBOL));
//        */
//        myPacMan.position.x = x;
//        myPacMan.position.y = y;
//        myPacMan.vx = 0;
//        myPacMan.vy = 0;
//
//        if (myPacMan.lives <= 0)
//        {
//            game_over = true;
//        }
//    }
//} // check_collisions
//
//void resetGame()
//{
//    //Remove characters
//    for (int i = 0; i < H; i++)
//    {
//        for (int j = 0; j < W; j++)
//        {
//            playfield[i][j] = resetField[i][j];
//            if (ghostField[i][j] == GHOST_SYMBOL) {
//                ghostField[i][j] = EMPTY_SYMBOL;
//            }
//        }
//    }
//    collision_with_ghost = false;
//    game_over = false;
//    allGhosts.clear();
//    foodToWin = 0;
//    main();
//}
//
//void resetGhosts() {
//
//    allGhosts.clear();
//    //Remove characters
//    for (int i = 0; i < H; i++)
//    {
//        for (int j = 0; j < W; j++)
//        {
//            if (ghostField[i][j] == GHOST_SYMBOL) {
//                ghostField[i][j] = EMPTY_SYMBOL;
//            }
//        }
//    }
//
//    //Creates Ghosts
//    for (int i = 0; i < NR_GHOSTS_START; i++)
//    {
//        add_new_ghost();
//    }
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MPI CODE
//
// cpu holds my processor number, cpu=0 is master, rest are slaves
// numcpus is the total number of processors
int cpu, numcpus;

// MPI function
void mpi () {
    printf("MPI was called\n");
    
//    int i, slave;
//    MPI_Status status;

    if (cpu == 0) {

    } else {

    }
}

int main(int argc, char** argv)
{
	// set console to code page 437 https://en.wikipedia.org/wiki/Code_page_437
	// and set font to "raster font"
	/*
	system("chcp 437");
	for (int i=0; i<255; i++)
	{
	printf("%c", i);
	if (i%25==0) printf("\n");
	}
	_getch();
	*/

	//system("cls"); 
//    clear();
//    wrefresh(wui);
//    refresh();
//
//    //Resizes cmd window
//    //system("MODE 62,33");
//
//
//    //Disables resize and full screen.
//    //HWND consoleWindow = GetConsoleWindow();
//    //SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
//
//    hidecursor();
//    initialize();
//    show_playfield();
//    set_cursor_position(W / 2 - 7, H / 2 - 3);
//    printw("   PAC-MAN   ");
//    set_cursor_position(W / 2 - 10, H / 2 - 1);
//    printw("   Press Any Key   ");
//    wrefresh(wui);
//    refresh();
//    wgetch(wui);
//
//
//    set_cursor_position(W / 2 - 9, H / 2 - 3);
//    printw("                ");
//    set_cursor_position(W / 2 - 8, H / 2 - 2);
//    printw("   GAME START   ");
//    set_cursor_position(W / 2 - 8, H / 2 - 1);
//    printw("                ");
//    wrefresh(wui);
//    refresh();
//    sleep(1);
//
//    int steps = 0;
//    while (!game_over)
//    {
//        move_ghosts();
//        show_playfield();
//        check_collisions();
//
//        user_input();
//        move_pacman();
//        show_playfield();
//        check_collisions();
//
//        //Sleep(5);
//        usleep(5000);
//
//        if (++steps % 100 == 0)
//        {
//            add_new_ghost();
//            show_playfield();
//        }
//
//        if (myPacMan.food_collected >= foodToWin)
//        {
//            set_cursor_position(W / 2 - 9, H / 2 - 3);
//            printw("                ");
//            set_cursor_position(W / 2 - 7, H / 2 - 2);
//            printw("   YOU WON!   ");
//            set_cursor_position(W / 2 - 8, H / 2 - 1);
//            printw("                ");
//            wrefresh(wui);
//            refresh();
//            sleep(2);
//            wgetch(wui);
//            resetGame();
//        }
//    }
//
//    clear();
//    set_cursor_position(W / 2 - 9, H / 2 - 3);
//    printw("                ");
//    set_cursor_position(W / 2 - 8, H / 2 - 2);
//    printw("   GAME OVER!   ");
//    wrefresh(wui);
//    refresh();
//    sleep(3);
//    resetGame();
    

    MPI_Init(&argc, &argv);                     // Initialize the MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &cpu);        // Get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &numcpus);    // Get the number of processes
    
    mpi();
    
    // MPI Finish Code
    MPI_Finalize();
    
} // main
