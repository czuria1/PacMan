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

////////////////////////////////////////////////////////////////////
// To run with different processors use:
// /usr/lib64/openmpi/bin/mpirun -np {input # of processors} pacman
///////////////////////////////////////////////////////////////////
#ifdef _WIN32
#define H2 30
#define W2 120
#elif linux
#define H2 33
#define W2 120
#else
#define H2 33
#define W2 120
#endif

#include <curses.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <mpi.h>
#include <sys/time.h>

#define H 30
#define W 60
#define PACMAN_SYMBOL 'P'
#define GHOST_SYMBOL  'G'
#define FOOD_SYMBOL '.'
#define WALL_SYMBOL '#'
#define DOOR_SYMBOL '-'
#define EMPTY_SYMBOL ' '
#define NO_SPAWN_SYMBOL 'X'
#define LOGIC_SYMBOL 'A'
#define POWERUP_SYMBOL 'o'
#define INVINCIBLE_SYMBOL 'O'

int nr_ghosts_start = 0;
bool collision_with_ghost = false;
bool game_over = false;
bool ghostMsg = false;
bool firstTime = true;
bool displayRight = true;
bool invincible = false;
int foodToWin = 0;
int xs = 120;
int ys = 32;
int doPrint = 1;
time_t timer;
time_t timerInv;

void resetGhosts();
void game(WINDOW *win, int a[]);
void art();
void playerDeath();
using namespace std;

struct timeval start, stop;

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
	{ "#o######### ########### ############ ########### #########o#" },
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
	{ "#o######### ############### #### ############### #########o#" },
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

//Logic layer
char decisionField[H + 1][W + 1] =
{
{ "############################################################" },
{ "#          A            ############            A          #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "# ######### ########### ############ ########### ######### #" },
{ "#          A  A        A            A        A  A         A#" },
{ "#A######### ## ############################## ## ######### #" },
{ "#          A## ############################## ##A          #" },
{ "########### ##              ####              ## ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ############### #### ############### ###########" },
{ "########### ##             A    A             ## ###########" },
{ "########### ## #   #########    #########   # ## ###########" },
{ "##         A  A#                            #A  A         ##" },
{ "##         A  A#                            #A  A         ##" },
{ "##         A  A#                            #A  A         ##" },
{ "########### ## ############################## ## ###########" },
{ "########### ##A                              A## ###########" },
{ "########### ## ############################## ## ###########" },
{ "########### ## ############################## ## ###########" },
{ "#          A  A             ####             A  A          #" },
{ "# ######### ############### #### ############### ######### #" },
{ "# ######### ############### #### ############### ######### #" },
{ "#        ##A  A            A    A            A  A##        #" },
{ "######## ## ## ############################## ## ## ########" },
{ "######## ## ## ############################## ## ## ########" },
{ "#       A   ##              ####              ##   A       #" },
{ "# ######################### #### ######################### #" },
{ "# ######################### #### ######################### #" },
{ "#                          A    A                          #" },
{ "############################################################" }
}; // <-- CAUTION! Semicolon necessary!
class Ghost {

public:

	Ghost(int start_x, int start_y)
	{
		this->position.x = start_x;
		this->position.y = start_y;
		choose_random_direction();
	}
	int getX() {
		return position.x;
	}
	int getY() {
		return position.y;
	}
	void move()
	{
		// 1. where does the Ghost want to move to?
		int dx = position.x + vx;
		int dy = position.y + vy;
		int nx = position.x - vx;
		int ny = position.y - vy;

		// 2. what is at this place where we plan to move the Ghost to?
		//char whats_there = playfield[dy][dx];
		char whats_there2 = ghostField[dy][dx];
		char whats_there3 = playerField[dy][dx];
		char atFeet = playerField[position.y][position.x];
		char behind = playerField[ny][nx];
		// 3. is it a PacMan?
		if (whats_there3 == PACMAN_SYMBOL && !invincible)
		{
			collision_with_ghost = true;
			playerDeath();
		}
		if ((whats_there3 == INVINCIBLE_SYMBOL || atFeet == INVINCIBLE_SYMBOL || behind == INVINCIBLE_SYMBOL) && invincible)
		{
			ghostField[position.y][position.x] = EMPTY_SYMBOL;
			int x = rand() % (42 - 17 + 1) + 17;
			int y = rand() % (14 - 12 + 1) + 12;
			this->position.x = x;
			this->position.y = y;
		}
		// 4. is the field free to move there?
		if (whats_there2 != WALL_SYMBOL && decisionField[position.y][position.x] != LOGIC_SYMBOL)
		{
			ghostField[position.y][position.x] = EMPTY_SYMBOL;
			position.x = dx;
			position.y = dy;
			ghostField[position.y][position.x] = GHOST_SYMBOL;
		}
		else
		{
			double duration = difftime(time(0L), timer);
			if (duration < 7) {
				//chase();
				scatter();
			}
			else {
				if (duration > 20) {
					timer = time(0L);
				}
				chase();
			}

		}

	} // move

	void choose_random_direction()
	{
		int r = rand() % 4;
		switch (r)
		{
		case 0: vx = 1; vy = 0;  break;
		case 1: vx = 0;  vy = 1;  break;
		case 2: vx = -1; vy = 0;  break;
		case 3: vx = 0;  vy = -1; break;
		}
	}
	//Chase, Scatter, or Frightened.
	/*
	 * Scatter for 7 seconds, then Chase for 20 seconds.
	 */
	void scatter() {
		int y = rand() % 30;
		int x = rand() % 60;
		choose_smart_path(x, y);
	}
	void chase() {
		choose_smart_path(myPacMan.position.x, myPacMan.position.y);
	}

	void setMove(int x, int y) {
		vx = x;
		vy = y;
		int dx = position.x + vx;
		int dy = position.y + vy;
		ghostField[position.y][position.x] = EMPTY_SYMBOL;
		position.x = dx;
		position.y = dy;
		ghostField[position.y][position.x] = GHOST_SYMBOL;
	}

	int distance(int x1, int y1, int x2, int y2)
	{
		int distancex = (x2 - x1) * (x2 - x1);
		int distancey = (y2 - y1) * (y2 - y1);
		int distance = distancex - distancey;
		if (distance < 0) {
			return distancey - distancex;
		}
		return distance;
	}

	int findSmallestPathNumber(int arr[])
	{
		int temp = 0;
		for (int i = 0; i < 4; i++)
		{
			if (arr[i] != 0) {
				temp = arr[i];
			}
		}
		int min = temp;
		for (int i = 0; i < 4; i++)
		{
			if (min > arr[i] && arr[i] != 0)
				min = arr[i];
		}
		return min;
	}
	int findBiggestPathNumber(int arr[])
	{
		int temp = 0;
		for (int i = 0; i < 4; i++)
		{
			if (arr[i] != 0) {
				temp = arr[i];
			}
		}
		int max = temp;
		for (int i = 0; i < 4; i++)
		{
			if (max < arr[i] && arr[i] != 0)
				max = arr[i];
		}
		return max;
	}
	void keepGoing() {
		int dx = position.x + vx;
		int dy = position.y + vy;
		if (ghostField[dy][dx] == WALL_SYMBOL) {
			if (vx == 1)	//If going right
			{
				if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
				{
					//canUp = true;
					setMove(0, -1);
				}
				else
				{
					//canDown = true;
					setMove(0, 1);
				}
			}
			else if (vy == -1) 	//If going up
			{
				if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
				{
					//canLeft = true;
					setMove(-1, 0);
				}
				else
				{
					//canRight= true;
					setMove(1, 0);
				}
			}
			else if (vy == 1) 	//If going down
			{
				if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
				{
					//canRight = true;
					setMove(1, 0);
				}
				else
				{
					//canLeft = true;
					setMove(-1, 0);
				}
			}
			else if (vx == -1) //If going left
			{
				if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
				{
					//canDown = true;
					setMove(0, 1);
				}
				else
				{
					//canUp = true;
					setMove(0, -1);
				}
			}
			else {
				choose_random_direction();
			}

		}
	}

	void choose_smart_path(int x, int y)
	{
		//On Logic Tile
		if (decisionField[position.y][position.x] == LOGIC_SYMBOL)
		{
			int xPAC = x;
			int yPAC = y;
			int arr[] = { 0,0,0,0 };
			bool upPrev = false;
			bool downPrev = false;
			bool rightPrev = false;
			bool leftPrev = false;

			if (vx == 1)	//If going right
			{
				rightPrev = true;
			}
			else if (vy == -1) 	//If going up
			{
				upPrev = true;
			}
			else if (vy == 1) 	//If going down
			{
				downPrev = true;
			}
			else if (vx == -1) //If going left
			{
				leftPrev = true;
			}
			if (ghostField[position.y + 1][position.x] != WALL_SYMBOL && !upPrev)
			{
				arr[0] = distance(xPAC, yPAC, position.x, position.y + 1);
			}
			if (ghostField[position.y - 1][position.x] != WALL_SYMBOL && !downPrev)
			{
				arr[1] = distance(xPAC, yPAC, position.x, position.y - 1);
			}
			if (ghostField[position.y][position.x - 1] != WALL_SYMBOL && !rightPrev)
			{
				arr[2] = distance(xPAC, yPAC, position.x - 1, position.y);
			}
			if (ghostField[position.y][position.x + 1] != WALL_SYMBOL && !leftPrev)
			{
				arr[3] = distance(xPAC, yPAC, position.x + 1, position.y);
			}

			int path = 1;
			if (invincible) {
				path = findBiggestPathNumber(arr);
			}
			else {
				path = findSmallestPathNumber(arr);
			}
			if (path == arr[0]) {
				if (ghostField[position.y + 1][position.x] != WALL_SYMBOL)
				{
					setMove(0, 1);
				}
				else {
					keepGoing();
				}
			}
			else if (path == arr[1]) {
				if (ghostField[position.y - 1][position.x] != WALL_SYMBOL)
				{
					setMove(0, -1);
				}
				else {
					keepGoing();
				}
			}
			else if (path == arr[2]) {
				if (ghostField[position.y][position.x - 1] != WALL_SYMBOL)
				{
					setMove(-1, 0);
				}
				else {
					keepGoing();
				}
			}
			else if (path == arr[3]) {
				if (ghostField[position.y][position.x + 1] != WALL_SYMBOL)
				{
					setMove(1, 0);
				}
				else {
					keepGoing();
				}
			}
			else
			{
				keepGoing();
			}
		}
		else
		{
			keepGoing();
		}
	}

private:
	struct coord position;
	int vx;
	int vy;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MPI CODE
//
// cpu holds my processor number, cpu=0 is master, rest are slaves
// numcpus is the total number of processors
int cpu, numcpus;

// MPI function
void mpi(int* a, int N) {
	mvprintw(18, 62, "MPI was called\n");

	int i, slave;
	MPI_Status status;

	int numeach = N / numcpus;

	int addedGhosts;
	addedGhosts = 0;

	// Master process
	if (cpu == 0) {
		// For each processor available,
		// collect the address of the ghost that was added to the board
//        add_new_ghost();
//        addedGhosts = 1;
//        for (slave = 1; slave < numcpus; slave++) {
//            MPI_Recv(&a[numeach*slave], numeach, MPI_INT, slave, 2, MPI_COMM_WORLD, &status);
//            addedGhosts++;
//        }

		for (slave = 1; slave < numcpus; slave++)
			MPI_Send(&a[numeach*slave], numeach, MPI_INT, slave, 1, MPI_COMM_WORLD);

		for (i = 0; i < numeach; i++) {
			add_new_ghost();
			add_new_ghost();
			add_new_ghost();
			add_new_ghost();
		}

		for (slave = 1; slave < numcpus; slave++)
			MPI_Recv(&a[numeach*slave], numeach, MPI_INT, slave, 2, MPI_COMM_WORLD, &status);

		//        printf("Added ghosts: %d\n", addedGhosts);
	}
	// Slave process
	else {
		float data[numeach];
		MPI_Recv(&data[0], numeach, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
		for (i = 0; i < numeach; i++)
			add_new_ghost();
		MPI_Send(&data[0], numeach, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}
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
	invincible = false;

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

	//     2. create some ghosts
	for (int i = 0; i < nr_ghosts_start; i++)
	{
		add_new_ghost();
		mvprintw(19, 62, "Print new ghost: %d\n", i);
	}
	refresh();

} // initialize

void initializeMPI(int* a, int N)
{
	//Starting position
	myPacMan.position.x = 29;
	myPacMan.position.y = 16;
	myPacMan.vx = 0;
	myPacMan.vy = 0;
	myPacMan.food_collected = 0;
	myPacMan.lives = 3;
	myPacMan.chasing = false;
	invincible = false;

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
	mpi(a, N);
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
		if (c1 == 'z')
		{
			invincible = true;
			timerInv = time(0L);
			playerField[myPacMan.position.y][myPacMan.position.x] = INVINCIBLE_SYMBOL;
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

	if (invincible)
	{
		double duration = difftime(time(0L), timerInv);
		if (duration > 12)
		{
			invincible = false;
		}
	}

	if (ghostField[ny][nx] == GHOST_SYMBOL && !invincible)
	{
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
		else if (playfield[ny][nx] == POWERUP_SYMBOL)
		{
			invincible = true;
			playfield[ny][nx] = EMPTY_SYMBOL;
			timerInv = time(0L);
		}
		if (!invincible) {
			// put PacMan back again to playfield
			playerField[myPacMan.position.y][myPacMan.position.x] = PACMAN_SYMBOL;
		}
		else {
			playerField[myPacMan.position.y][myPacMan.position.x] = INVINCIBLE_SYMBOL;
		}
	}

} // move_pacman
void infoDisplay() {
	art();
	mvprintw(12, 62, "Score:%3d ", myPacMan.food_collected);
	mvprintw(13, 62, "Remaining:%3d ", foodToWin - myPacMan.food_collected);
	mvprintw(14, 62, "Lives:%2d", myPacMan.lives);
	if (invincible) {
		mvprintw(15, 62, "INVINCIBLE");
	}
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
			else if (playerField[i][j] == PACMAN_SYMBOL || playerField[i][j] == INVINCIBLE_SYMBOL)
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
		myPacMan.lives--;
		//Death Visual
		mvprintw(myPacMan.position.y, myPacMan.position.x - 1, "(O)");

		//After Death Status Board
		mvprintw(H / 2 - 3, W / 2 - 9, "   GAME START   ");
		mvprintw(H / 2 - 2, W / 2 - 8, "   Lifes:%d   ", myPacMan.lives);
		mvprintw(H / 2 - 1, W / 2 - 8, "   Score:%d   ", myPacMan.food_collected);
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

void resetGame(WINDOW *win, int a[])
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
	invincible = false;
	allGhosts.clear();
	foodToWin = 0;
	myPacMan.vx = 0;
	myPacMan.vy = 0;
	initializeMPI(a, nr_ghosts_start);
	game(win, a);
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
	for (int i = 0; i < nr_ghosts_start; i++)
	{
		add_new_ghost();
	}
}
void resetWindow() {
	endwin();
	erase();
	mvprintw(15, 20, "Game is still running! ");
	mvprintw(16, 20, "Please Resize Window...");
	mvprintw(17, 20, "........%d....%d.....", ys, xs);
	refresh();
}

void do_resize()
{
	getmaxyx(stdscr, ys, xs);
	if (ys < H2) {
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

void game(WINDOW *win, int a[]) {
	show_playfield(-1);
	nodelay(stdscr, FALSE);
	mvprintw(H / 2 - 3, W / 2 - 7, "   PAC-MAN   ");
	mvprintw(H / 2 - 1, W / 2 - 10, "   Press Any Key   ");
	refresh();
	getch();
	nodelay(stdscr, TRUE);
	mvprintw(H / 2 - 3, W / 2 - 9, "                ");
	mvprintw(H / 2 - 2, W / 2 - 8, "   GAME START   ");
	mvprintw(H / 2 - 1, W / 2 - 8, "                ");
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
			//firstTime = false;
			//add_new_ghost();
			//show_playfield(steps);
		}

		if (myPacMan.food_collected >= foodToWin)
		{
			mvprintw(H / 2 - 3, W / 2 - 9, "                ");
			mvprintw(H / 2 - 2, W / 2 - 7, "   YOU WON!   ");
			mvprintw(H / 2 - 1, W / 2 - 8, "                ");
			refresh();
			sleep(2);
			getch();
			resetGame(win, a);
		}
	}
	erase();
	mvprintw(H / 2 - 2, W / 2 - 8, "   GAME OVER!   ");
	refresh();
	sleep(5);
	resetGame(win, a);
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

void setupGhostBuffer(int *a, int N) {
	int i;
	for (i = 0; i < N; ++i) {
		a[i] = i;
	}
}

void print(int* a, int N) {
	if (doPrint) {
		int i;
		for (i = 0; i < N; ++i)
			printf("%d ", (int)a[i]);
		printf("\n");
	}
}

void starttime() {
	gettimeofday(&start, 0);
}

void endtime(const char* c) {
	gettimeofday(&stop, 0);
	double elapsed = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_usec - start.tv_usec) / 1000.0;
	printf("%s: %f ms\n", c, elapsed);
}

void init(int* a, int N, const char* c) {
	printf("***************** %s **********************\n", c);
	setupGhostBuffer(a, N);
	starttime();
}

void finish(int* a, int N, const char* c) {
	endtime(c);
	printf("***************************************************\n");
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);                     // Initialize the MPI environment
	MPI_Comm_size(MPI_COMM_WORLD, &numcpus);    // Get the number of processes
	MPI_Comm_rank(MPI_COMM_WORLD, &cpu);        // Get the rank of the process

	WINDOW * win;
	if ((win = initscr()) == NULL) {
		printf("Can't load Curses!\n");
		exit(EXIT_FAILURE);
	}
	win = newwin(120, 33, 0, 0);
	hidecursor();

	// initialize the number of ghosts for the game
	nr_ghosts_start = numcpus;

	int a[nr_ghosts_start];

	// Master process code
	if (cpu == 0) {
		// initialize the buffer of the number of ghosts for the game
		setupGhostBuffer(a, nr_ghosts_start);
		//        printf("nr_ghosts_start is: %d\n" , nr_ghosts_start);
		//        // Test 1: Sequential For Loop
		//        init(a, nr_ghosts_start, "Normal");
		//        initialize();
		//        // SET original add ghost function here
		//        finish(a, nr_ghosts_start, "Normal");
		//        // Test 2: MPI
		//        init(a, nr_ghosts_start, "MPI");
	}

	initializeMPI(a, nr_ghosts_start);

	//    if (cpu == 0)
	//        finish(a, nr_ghosts_start, "MPI");

	MPI_Finalize();

	//    initialize();
	cbreak();
	noecho();
	raw();
	keypad(win, true);
	keypad(stdscr, true);
	game(win, a);
	endwin();


	return 0;

} // main
