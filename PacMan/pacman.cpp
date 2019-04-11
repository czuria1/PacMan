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

#include <stdio.h> // printf()
#include <conio.h> // getch()
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdbool.h>
#include <vector>

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
#define FOOD_TO_WIN 200

bool collision_with_ghost = false;
bool game_over = false;

int main();
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
char resetField[H + 1][W + 1];

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
char playfield2[H + 1][W + 1] =
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
		char whats_there = playfield[dy][dx];
		char whats_there2 = playfield2[dy][dx];
		// 3. is it a PacMan?
		if (whats_there == PACMAN_SYMBOL)
		{
			collision_with_ghost = true;
			playfield[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
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
			playfield2[position.y][position.x] = EMPTY_SYMBOL;

			position.x = dx;
			position.y = dy;
			//lastChar = playfield[position.y][position.x];
			playfield2[position.y][position.x] = GHOST_SYMBOL;
		}
		else
		{
			// field is not free, i.e. a wall:
			// so do not move Ghost at all in this step, but
			// randomly choose a new direction to move
			choose_random_direction();
		}

		// randomize behavior of the Ghost
		if (rand() % 30 == 0)
			choose_random_direction();

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


private:
	struct coord position;
	int vx;
	int vy;
	bool chasing;
	//char lastChar;
};


std::vector<Ghost*> allGhosts;



// Set cursor position in console
// see http://www.dreamincode.net/forums/topic/153240-console-cursor-coordinates/ 
void set_cursor_position(int x, int y)
{
	//Initialize the coordinates
	COORD coord = { x, y };
	//Set the position
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

} // set_cursor_position


void hidecursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
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
			}
			//Replace X with ' '
			else if (playfield[i][j] == NO_SPAWN_SYMBOL) 
			{
				playfield[i][j] = EMPTY_SYMBOL;
				playfield2[i][j] = EMPTY_SYMBOL;
			}
		}
	}

	// 2. create some ghosts
	for (int i = 0; i < NR_GHOSTS_START; i++) 
	{
		add_new_ghost();
	}

} // initialize


void user_input()
{
	if (_kbhit())
	{
		char c1 = _getch();
		if (c1 == ' ')
		{
			// stop moving
			myPacMan.vx = 0;
			myPacMan.vy = 0;
		}

		if (c1 == 27)
		{
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
		if (c1 == -32)
		{
			char c2 = _getch();

			//printf("c1=%d c2=%d\n", c1, c2);

			myPacMan.vx = 0;
			myPacMan.vy = 0;

			switch (c2)
			{
			case 72: myPacMan.vy = -1; break; // cursor up
			case 80: myPacMan.vy = +1; break; // cursor down
			case 75: myPacMan.vx = -1; break; // cursor left
			case 77: myPacMan.vx = +1; break; // cursor right                     
			}
		}

	}
}


void move_ghosts()
{
	// 1. move all Ghosts
	for (int i = 0; i < allGhosts.size(); i++)
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
	playfield[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;

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

	if (playfield2[ny][nx] == GHOST_SYMBOL) {
		collision_with_ghost = true;
		playfield[myPacMan.position.y][myPacMan.position.x] = EMPTY_SYMBOL;
	}
	else
	{
		if (playfield[ny][nx] == FOOD_SYMBOL)
		{
			myPacMan.food_collected++;
		}

		// put PacMan back again to playfield
		playfield[myPacMan.position.y][myPacMan.position.x] = PACMAN_SYMBOL;
	}

} // move_pacman


void show_playfield()
{
	set_cursor_position(0, 0);
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if(playfield2[i][j] == GHOST_SYMBOL)
			{
				printf("%c", playfield2[i][j]);
			}
			else 
			{
				printf("%c", playfield[i][j]);
			}
		}
		printf("\n");
	}
	printf("Score: %d - Lives:%d\n", myPacMan.food_collected, myPacMan.lives);
}

void check_collisions()
{
	// did a collision between PacMan and Ghost happen?
	if (collision_with_ghost)
	{
		set_cursor_position(W / 2 - 9, H / 2 - 3);
		printf("   GAME START   ");
		set_cursor_position(W / 2 - 8, H / 2 - 2);
		myPacMan.lives--;
		printf("   Lifes:%d   ", myPacMan.lives );
		set_cursor_position(W / 2 - 8, H / 2 - 1);
		printf("   Score:%d   ", myPacMan.food_collected);
		collision_with_ghost = false;
		Sleep(1500);
		resetGhosts();

		int x, y;
		x = 29;
		y = 16;
		// try to find a new (x,y) PacMan start coordinate randomly 
		/*
		do
		{
			x = 1 + rand() % (W - 1);
			y = 1 + rand() % (H - 1);

		} while ((playfield[y][x] != FOOD_SYMBOL) && (playfield[y][x] != EMPTY_SYMBOL));
		*/
		myPacMan.position.x = x;
		myPacMan.position.y = y;
		myPacMan.vx = 0;
		myPacMan.vy = 0;

		if (myPacMan.lives <= 0) 
		{
			game_over = true;
		}
	}
} // check_collisions

void resetGame() 
{
	//Remove characters
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			playfield[i][j] = resetField[i][j];
			if (playfield2[i][j] == GHOST_SYMBOL) {
				playfield2[i][j] = EMPTY_SYMBOL;
			}
		}
	}
	collision_with_ghost = false;
	game_over = false;
	allGhosts.clear();

	main();
}

void resetGhosts() {

	allGhosts.clear();
	//Remove characters
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			if (playfield2[i][j] == GHOST_SYMBOL) {
				playfield2[i][j] = EMPTY_SYMBOL;
			}
		}
	}

	//Creates Ghosts
	for (int i = 0; i < NR_GHOSTS_START; i++)
	{
		add_new_ghost();
	}
}
int main()
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

	system("cls"); 
	
	//Resizes cmd window
	system("MODE 62,33");

	//Disables resize and full screen.
	HWND consoleWindow = GetConsoleWindow();
	SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

	hidecursor();
	initialize();
	show_playfield();
	set_cursor_position(W / 2 - 7, H / 2 - 3);
	printf("   PAC-MAN   ");
	set_cursor_position(W / 2 - 10, H / 2 - 1);
	printf("   Press Any Key   ");
	_getch();


	set_cursor_position(W / 2 - 9, H / 2 - 3);
	printf("                ");
	set_cursor_position(W / 2 - 8, H / 2 - 2);
	printf("   GAME START   ");
	set_cursor_position(W / 2 - 8, H / 2 - 1);
	printf("                ");
	Sleep(1500);

	int steps = 0;
	while (!game_over)
	{
		move_ghosts();
		show_playfield();
		check_collisions();

		user_input();
		move_pacman();
		show_playfield();
		check_collisions();

		//Sleep(5);

		if (++steps % 100 == 0)
		{
			add_new_ghost();
			show_playfield();
		}

		if (myPacMan.food_collected == FOOD_TO_WIN)
		{
			set_cursor_position(W / 2 - 9, H / 2 - 3);
			printf("                ");
			set_cursor_position(W / 2 - 8, H / 2 - 2);
			printf("   YOU WON!   ");
			set_cursor_position(W / 2 - 8, H / 2 - 1);
			printf("                ");
			Sleep(1500);
			_getch();
			resetGame();
		}
	}

	system("cls");
	set_cursor_position(W / 2 - 9, H / 2 - 3);
	printf("                ");
	set_cursor_position(W / 2 - 8, H / 2 - 2);
	printf("   GAME OVER!   ");
	Sleep(1500);
	set_cursor_position(W / 2 - 9, H / 2 + 10);
	printf("   Press Any Key   ");
	_getch();
	resetGame();
} // main