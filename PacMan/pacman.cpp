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

#include <curses.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <signal.h>

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

bool collision_with_ghost = false;
bool game_over = false;
bool ghostMsg = false;
bool firstTime = true;
bool displayRight = true;
int foodToWin = 0;
int xs = 120;
int ys = 32;

int main();
void resetGhosts();
void game(WINDOW *win);
void art();
void playerDeath();
using namespace std;

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
class Ghost {

public:

	Ghost(int start_x, int start_y)
	{
		this->position.x = start_x;
		this->position.y = start_y;
		this->chasing = true;
		//this->lastChar = ' ';
		choose_random_direction();
	}

	void move()
	{
		// 1. where does the Ghost want to move to?
		int dx = position.x + vx;
		int dy = position.y + vy;

		// 2. what is at this place where we plan to move the Ghost to?
		//char whats_there = playfield[dy][dx];
		char whats_there2 = ghostField[dy][dx];
		char whats_there3 = playerField[dy][dx];
		// 3. is it a PacMan?
		if (whats_there3 == PACMAN_SYMBOL)
		{
			collision_with_ghost = true;
			playerDeath();
		}

		// 4. is the field free to move there?
		if (whats_there2 != WALL_SYMBOL)
		{
			/*
			//Ghost won't eat spaces
			if (playfield[dy][dx] != ' ' && playfield[dy][dx] != PACMAN_SYMBOL && playfield[dy][dx] != GHOST_SYMBOL)
			{
				//playfield[position.y][position.x] = lastChar;
			}
			else {
				playfield[position.y][position.x] = EMPTY_SYMBOL;
			}
			*/
			ghostField[position.y][position.x] = EMPTY_SYMBOL;

			position.x = dx;
			position.y = dy;
			//lastChar = playfield[position.y][position.x];
			ghostField[position.y][position.x] = GHOST_SYMBOL;
		}
		else
		{
			// field is not free, i.e. a wall:
			// so do not move Ghost at all in this step, but
			// randomly choose a new direction to move
			choose_random_direction();
			//choose_smart_path();
		}

		// randomize behavior of the Ghost
		if (rand() % 20 == 0)
		{

			choose_random_direction();
			//choose_smart_path();
		}

	} // move


	void choose_random_direction()
	{
		int r = rand() % 4;
		switch (r)
		{
		case 0: vx = +1; vy = 0;  break;
		case 1: vx = 0;  vy = 1;  break;
		case 2: vx = -1; vy = 0;  break;
		case 3: vx = 0;  vy = -1; break;
		}
	}


	void choose_smart_path()
	{

		/*
		int r = rand() % 2;
		//If Right
		if (vx == 1 || vx == -1)
		{
			switch (r)
			{
			case 0: vx = 0; vy = 1;  break;
			case 1: vx = 0;  vy = -1;  break;
			}
		}
		else if (vy == 1 || vy == -1)
		{
			switch (r)
			{
			case 0: vx = 1; vy = 0;  break;
			case 1: vx = -1;  vy = 0;  break;
			}
		}
		*/
		bool canRight = false;
		bool canLeft = false;
		bool canDown = false;
		bool canUp = false;
		//printw("      (%d,%d)       ",vx, vy);
		if (vx == 1)
		{
			if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = 1;
				canUp = true;
			}
			if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = -1;
				canDown = true;
			}
			if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
			{
				//vx = -1; vy = 0;
				canLeft = true;
			}
		}
		else if (vy == 1)
		{
			if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
			{
				//vx = 1; vy = 0;
				canRight = true;
			}
			if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
			{
				//vx = -1; vy = 0;
				canLeft = true;
			}
			if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = -1;
				canDown = true;
			}
		}
		else if (vy == -1)
		{
			if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
			{
				//vx = -1; vy = 0;
				canLeft = true;
			}
			if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
			{
				//vx = 1; vy = 0;
				canRight = true;
			}
			if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = 1;
				canUp = true;
			}
		}
		else
		{
			if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = 1;
				canUp = true;
			}
			if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
			{
				//vx = 0; vy = -1;
				canDown = true;
			}
			if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
			{
				//vx = 1; vy = 0;
				canRight = true;
			}
		}
		bool chosePath = false;
		while (!chosePath)
		{
			int r = rand() % 4;
			switch (r)
			{
			case 0:
				if (canRight)
				{
					vx = 1; vy = 0;
					chosePath = true;
				}
				break;
			case 1:
				if (canUp)
				{
					vx = 0;  vy = 1;
					chosePath = true;
				}
				break;
			case 2:
				if (canLeft)
				{
					vx = -1; vy = 0;
					chosePath = true;
				}
				break;
			case 3:
				if (canDown)
				{
					vx = 0; vy = -1;
					chosePath = true;
				}
				break;
			}
		}
	}



private:
	struct coord position;
	int vx;
	int vy;
	bool chasing;
	//char lastChar;
};


std::vector<Ghost*> allGhosts;

void set_cursor_position(int col, int row)
{
	//os << "\033[" << col << ";" << row << "H";
		//printw("\033[%dl;%dH", col, row);
	move(row, col);
	//wmove(win, col row);
	refresh();
} // set_cursor_position


void hidecursor()
{
	//system("setterm -cursor off");
	curs_set(0);
}


void add_new_ghost()
{

	int x, y;
	// try to find a (x,y) coordinate randomly where a food piece is
	/*
	do
	{
		x = 1 + rand() % (W - 1);
		y = 1 + rand() % (H - 1);
	} while (playfield[y][x] != FOOD_SYMBOL);
	*/
	//Spawn inside the Box
	x = rand() % (42 - 17 + 1) + 17;
	y = rand() % (14 - 12 + 1) + 12;
	allGhosts.push_back(new Ghost(x, y));
	ghostMsg = true;
}

void initialize()
{
	//Starting position
	myPacMan.position.x = 29;
	myPacMan.position.y = 16;
	myPacMan.vx = 0;
	myPacMan.vy = 0;
	myPacMan.food_collected = 0;
	myPacMan.lives = 3;
	myPacMan.chasing = false;

	// 1. replace each empty field in the playfield
	//    with a food field
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			resetField[i][j] = playfield[i][j];
			if (playfield[i][j] == EMPTY_SYMBOL)
			{
				playfield[i][j] = FOOD_SYMBOL;
				foodToWin++;
			}
			//Replace X with ' '
			else if (playfield[i][j] == NO_SPAWN_SYMBOL)
			{
				playfield[i][j] = EMPTY_SYMBOL;
				ghostField[i][j] = EMPTY_SYMBOL;
			}
		}
	}

	// 2. create some ghosts
	for (int i = 0; i < NR_GHOSTS_START; i++)
	{
		add_new_ghost();
	}
	refresh();

} // initialize

int kbhit(void)
{
	int ch = getch();

	if (ch != ERR) {
		ungetch(ch);
		return 1;
	}
	else {
		return 0;
	}
}

void user_input()
{
	if (kbhit())
	{
		char c1 = getch();
		if (c1 == ' ')
		{
			// stop moving
			myPacMan.vx = 0;
			myPacMan.vy = 0;
		}

		if (c1 == 27)
		{
			endwin();
			exit(0);
		}
		if (c1 == 'g')
		{
			add_new_ghost();
		}
		if (c1 == 'k')
		{
			myPacMan.lives--;
		}
		if (c1 == 'r')
		{
			resetGhosts();
		}
		if (c1 == 'p')
		{
			myPacMan.food_collected = foodToWin;
		}
		if (c1 != ERR) {
			myPacMan.vx = 0;
			myPacMan.vy = 0;
			if (c1 == 3) {
				myPacMan.vy = -1; // cursor up
			}
			else if (c1 == 2) {
				myPacMan.vy = +1;  // cursor down
			}
			else if (c1 == 4) {
				myPacMan.vx = -1;// cursor left
			}
			else if (c1 == 5) {
				myPacMan.vx = +1; // cursor right
			}

		}

	}
}


void move_ghosts()
{
	// 1. move all Ghosts
	for (int i = 0; (unsigned)i < allGhosts.size(); i++)
	{
		allGhosts[i]->move();
	}
}

void move_pacman()
{

	// 1. compute new desired coordinate (nx,ny)
	int nx = myPacMan.vx + myPacMan.position.x;
	int ny = myPacMan.vy + myPacMan.position.y;

	// 2. delete PacMan from old position, but only remove food.
	playerField[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;

	// 3. test whether there is a wall at (nx,ny)
	if (playfield[ny][nx] == WALL_SYMBOL || playfield[ny][nx] == DOOR_SYMBOL)
	{
		// Damn! There is a wall! Stop PacMan!
		myPacMan.vx = 0;
		myPacMan.vy = 0;
	}

	// 4. update PacMan's coordinate
	myPacMan.position.x += myPacMan.vx;
	myPacMan.position.y += myPacMan.vy;

	if (ghostField[ny][nx] == GHOST_SYMBOL) {
		collision_with_ghost = true;
		playerDeath();
	}
	else
	{
		if (playfield[ny][nx] == FOOD_SYMBOL)
		{
			myPacMan.food_collected++;
			playfield[ny][nx] = EMPTY_SYMBOL;
		}

		// put PacMan back again to playfield
		playerField[myPacMan.position.y][myPacMan.position.x] = PACMAN_SYMBOL;
	}

} // move_pacman
void infoDisplay() {
	art();
	mvprintw(12, 62, "Score:%3d ", myPacMan.food_collected);
	mvprintw(13, 62, "Remaining:%3d ", foodToWin - myPacMan.food_collected);
	mvprintw(14, 62, "Lives:%2d", myPacMan.lives);
	//mvprintw(15, 62, "Displays:%2d %d %d", displayRight, LINES, COLS);
	if (ghostMsg && !firstTime) {
		mvprintw(17, 61, "New Ghost Added!");
	}
}

void show_playfield(int steps)
{
	set_cursor_position(0, 0);
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if (ghostField[i][j] == GHOST_SYMBOL)
			{
				printw("%c", ghostField[i][j]);
			}
			else if (playerField[i][j] == PACMAN_SYMBOL)
			{
				printw("%c", playerField[i][j]);
			}
			else
			{
				printw("%c", playfield[i][j]);
			}
		}
		printw("\n");
	}
	if (displayRight) {
		infoDisplay();
	}
	if (steps % 10 == 0)
	{
		ghostMsg = false;
		mvprintw(17, 61, "                 ");
	}
	refresh();
}

void check_collisions()
{
	// did a collision between PacMan and Ghost happen?
	if (collision_with_ghost)
	{
		mvprintw(myPacMan.position.y, myPacMan.position.x - 1, "(O)");
		set_cursor_position(W / 2 - 9, H / 2 - 3);
		printw("   GAME START   ");
		set_cursor_position(W / 2 - 8, H / 2 - 2);
		myPacMan.lives--;
		printw("   Lifes:%d   ", myPacMan.lives);
		set_cursor_position(W / 2 - 8, H / 2 - 1);
		printw("   Score:%d   ", myPacMan.food_collected);
		refresh();
		collision_with_ghost = false;
		resetGhosts();

		int x, y;
		x = 29;
		y = 16;

		myPacMan.position.x = x;
		myPacMan.position.y = y;
		myPacMan.vx = 0;
		myPacMan.vy = 0;

		sleep(3);
		if (myPacMan.lives <= 0)
		{
			game_over = true;
		}
	}
} // check_collisions

void resetGame(WINDOW *win)
{
	//Remove characters
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			playfield[i][j] = resetField[i][j];
			if (ghostField[i][j] == GHOST_SYMBOL) {
				ghostField[i][j] = EMPTY_SYMBOL;
			}
		}
	}
	collision_with_ghost = false;
	game_over = false;
	ghostMsg = false;
	firstTime = true;
	allGhosts.clear();
	foodToWin = 0;
	initialize(); //Call last, before game
	game(win);
}

void resetGhosts() {

	allGhosts.clear();
	//Remove characters
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if (ghostField[i][j] == GHOST_SYMBOL) {
				ghostField[i][j] = EMPTY_SYMBOL;
			}
		}
	}
	firstTime = true;
	//Creates Ghosts
	for (int i = 0; i < NR_GHOSTS_START; i++)
	{
		add_new_ghost();
	}
}
void resetWindow() {
	endwin();
	erase();
	mvprintw(15, 20, "Game is still running! ");
	mvprintw(16, 20, "Please Resize Window...");
	refresh();
}

void do_resize()
{
	getmaxyx(stdscr, ys, xs);
	if (ys < 32) {
		resetWindow();
		if (xs < 114) {
			//resize_term(32, 61);
			displayRight = false;
		}
		else {
			//Fix for Windows OS
			//resize_term(32, 120);
			//displayRight = true;
		}
	}
	else if (xs < 61) {
		resetWindow();
		//resize_term(32, 61);
		displayRight = false;
	}
	else if (61 < xs && xs < 114) {
		if (displayRight) {
			resetWindow();
			//resize_term(32, 61);
			displayRight = false;
		}
	}
	else {
		//resize_term(32, 120);
		displayRight = true;
	}
}

void game(WINDOW *win) {
	show_playfield(-1);
	nodelay(stdscr, FALSE);
	set_cursor_position(W / 2 - 7, H / 2 - 3);
	printw("   PAC-MAN   ");
	set_cursor_position(W / 2 - 10, H / 2 - 1);
	printw("   Press Any Key   ");
	refresh();
	getch();
	nodelay(stdscr, TRUE);
	set_cursor_position(W / 2 - 9, H / 2 - 3);
	printw("                ");
	set_cursor_position(W / 2 - 8, H / 2 - 2);
	printw("   GAME START   ");
	set_cursor_position(W / 2 - 8, H / 2 - 1);
	printw("                ");
	refresh();
	sleep(3);

	int steps = 0;
	while (!game_over)
	{
		move_ghosts();
		check_collisions();

		user_input();
		move_pacman();
		show_playfield(-1);
		check_collisions();
		do_resize();

		usleep(100000);

		if (++steps % 100 == 0)
		{
			firstTime = false;
			add_new_ghost();
			show_playfield(steps);
		}

		if (myPacMan.food_collected >= foodToWin)
		{
			set_cursor_position(W / 2 - 9, H / 2 - 3);
			printw("                ");
			set_cursor_position(W / 2 - 7, H / 2 - 2);
			printw("   YOU WON!   ");
			set_cursor_position(W / 2 - 8, H / 2 - 1);
			printw("                ");
			sleep(2);
			getch();
			resetGame(win);
		}
	}
	erase();
	set_cursor_position(W / 2 - 8, H / 2 - 2);
	printw("   GAME OVER!   ");
	refresh();
	sleep(5);
	resetGame(win);
}

void art() {
	mvprintw(0, 61, "  _ __   __ _  ___ _ __ ___   __ _ _ __  ");
	mvprintw(1, 61, " | '_ \\ / _` |/ __| '_ ` _ \\ / _` | '_ \\ ");
	mvprintw(2, 61, " | |_) | (_| | (__| | | | | | (_| | | | |");
	mvprintw(3, 61, " | .__/ \\__,_|\\___|_| |_| |_|\\__,_|_| |_|");
	mvprintw(4, 61, " |_|                                     ");
	mvprintw(5, 67, "CDA 4104 - Rams");
	mvprintw(6, 61, "================================================.");
	mvprintw(7, 61, "     .-.   .-.     .--.                         |");
	mvprintw(8, 61, "    | OO| | OO|   / _.-' .-.   .-.  .-.   .''.  |");
	mvprintw(9, 61, "    |   | |   |   \\  '-. '-'   '-'  '-'   '..'  |");
	mvprintw(10, 61, "    '^^^' '^^^'    '--'                         |");
	mvprintw(11, 61, "===============.  .-.  .================.  .-.  |");
	mvprintw(12, 61, "               | |   | |                |  '-'  |");
	mvprintw(13, 61, "               | |   | |                |       |");
	mvprintw(14, 61, "               | ':-:' |                |  .-.  |");
	mvprintw(15, 61, "               |  '-'  |                |  '-'  |");
	mvprintw(16, 61, "==============='       '================'       |");
	mvprintw(20, 61, "Code Base by Prof. Dr.-Ing. Jurgen Brauer");
	mvprintw(21, 61, "Art Credit to '142' on asciiart.eu/video-games/pacman");
	refresh();

}

void playerDeath() {
	playerField[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
}
int main()
{
	WINDOW * win;
	if ((win = initscr()) == NULL) {
		printf("Can't load Curses!\n");
		exit(EXIT_FAILURE);
	}
	win = newwin(120, 33, 0, 0);
	//Resizes cmd window
	//system("MODE 62,33"); //Windows
	//wresize(win, 62, 33); //Curses
	hidecursor();
	initialize();
	cbreak();
	noecho();
	raw();
	keypad(win, true);
	keypad(stdscr, true);
	game(win);
	endwin();

	return 0;
}
