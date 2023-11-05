// caps lock disables movement -- shift inputs to .lower() or whatever. 

// shifting map sizes

// Score??
// Leaderboard??


#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcpy()
#include <math.h> // pow(), log2()
#include <time.h>
#include <ncurses.h> // thanks stackoverflow
#include <unistd.h> // sleep()
#include <ctype.h> // isalpha()

#include <sys/wait.h>

#define rows 25 // 21 default // and yes, this is hardcoded
#define cols 51 // 51 default 


// Global vars because I'm lazy and it kinda just makes sense here
int grid[rows][cols];

int charX; // character position (uses x/y, not rows/cols. Lack of foresight). 
int charY;


// Stackoverflow said to use this for keylogging. 
// I honestly don't see how it works the way it does, but here we are. 
int kbhit(void)
{
	int ch = getch();

	if(ch != ERR)
	{
		ungetch(ch);
		return 1;
	}
	else
	{
		return 0;
	}
}



// Helper functions for Conway's
int getUp(int x, int y)
{
	if(x == 0)
	{
		return grid[rows-1][y] == '#';
	}
	else
	{
		return grid[x-1][y] == '#';
	}
}

int getRight(int x, int y)
{
	if(y == cols-1)
	{
		return grid[x][0] == '#';
	}
	else
	{
		return grid[x][y+1] == '#';
	}
}

int getDown(int x, int y)
{
	if(x == rows-1)
	{
		return grid[0][y] == '#';
	}
	else
	{
		return grid[x+1][y] == '#';
	}
}

int getLeft(int x, int y)
{
	if(y == 0)
	{
		return grid[x][cols-1] == '#';
	}
	else
	{
		return grid[x][y-1] == '#';
	}
}

int getUpRight(int x, int y)
{
	if(x == 0)
	{
		if(y == cols-1)
		{
			return grid[rows-1][0] == '#';
		}
		else
		{
			return grid[rows-1][y+1] == '#';
		}
		
	}
	else
	{
		if(y == cols-1)
		{
			return grid[x-1][0] == '#';
		}
		else
		{
			return grid[x-1][y+1] == '#';
		}
	}
}

int getDownRight(int x, int y)
{
	if(x == rows-1)
	{
		if(y == cols-1)
		{
			return grid[0][0] == '#';
		}
		else
		{
			return grid[0][y+1] == '#';
		}
		
	}
	else
	{
		if(y == cols-1)
		{
			return grid[x+1][0] == '#';
		}
		else
		{
			return grid[x+1][y+1] == '#';
		}
	}
}

int getDownLeft(int x, int y)
{
	if(x == rows-1)
	{
		if(y == 0)
		{
			return grid[0][cols-1] == '#';
		}
		else
		{
			return grid[0][y-1] == '#';
		}
		
	}
	else
	{
		if(y == 0)
		{
			return grid[x+1][cols-1] == '#';
		}
		else
		{
			return grid[x+1][y-1] == '#';
		}
	}
}

int getUpLeft(int x, int y)
{
	if(x == 0)
	{
		if(y == 0)
		{
			return grid[rows-1][cols-1] == '#';
		}
		else
		{
			return grid[rows-1][y-1] == '#';
		}
		
	}
	else
	{
		if(y == 0)
		{
			return grid[x-1][cols-1] == '#';
		}
		else
		{
			return grid[x-1][y-1] == '#';
		}
	}
}


// Main code behind the Conway's rules
int getNeighbors(int x, int y)
{
	int sum = 0;

	if(getUp(x, y))
		sum++;
	if(getRight(x, y))
		sum++;
	if(getDown(x, y))
		sum++;
	if(getLeft(x, y))
		sum++;
	if(getUpRight(x, y))
		sum++;
	if(getDownRight(x, y))
		sum++;
	if(getDownLeft(x, y))
		sum++;
	if(getUpLeft(x, y))
		sum++;

	return sum;
}


// Main code behind the Conway's simulation, modifies global grid
void updateConway(int gridRef[rows][cols], int* end)
{
	int sum = 0;
	for(int i = 0; i < rows; i++)
	{
		for(int k = 0; k < cols; k++)
		{
			sum = getNeighbors(i, k);

			if(grid[i][k] == '#')
			{
				if(charX == k && charY == i)
				{
					*end = 1;
					break;
				}

				if(sum < 2 || sum > 3)
				{
					gridRef[i][k] = '.';
				}
				else
				{
					gridRef[i][k] = '#';
				}
			}
			else
			{
				if(sum == 3)
				{
					gridRef[i][k] = '#';
				}
				else
				{
					gridRef[i][k] = '.';
				}
			}
		}
		if(*end == 1)
			break;
	}
	

	for(int i = 0; i < rows; i++)
	{
		for(int k = 0; k < cols; k++)
		{
			grid[i][k] = gridRef[i][k];
		}
	}
}


// Self-explanatory functions: 
void printBoard()
{
	for(int i = 0; i < rows; i++)
	{
		for(int k = 0; k < cols; k++)
		{
			printw("%c", grid[i][k]);
		}
		printw("\n");
	}
}

void printTab()
{
	printw("    ");
}

// I use this to get the indenting right for everything in the terminal
void space()
{
	for(int i = 0; i < 8; i++)
	{
		printTab();	
	}
}

// I use this to print the current state of the board, but with the fancy border and everything. 
void printState(char instruction[256], int length)
{
	clear();

	space();
	printw("X-");
	for(int i = 0; i < cols+2; i++)
	{
		printw("-");
	}
	printw("-X\n");

	space();
	printw("  ");
	for(int i = 0; i < (cols+2 - length)/2; i++)
	{
		printw(" ");                   // print out titles and instructions here. 
	}
	printw("%s", instruction);

	for(int i = 0; i < (cols+2 - length)/2; i++)
	{
		printw(" ");                   // print out titles and instructions here. 
	}
	printw("  \n");

	space();
	printw("X-");
	for(int i = 0; i < cols+2; i++)
	{
		printw("-");
	}
	printw("-X\n");


	int count = 0;
	for(int i = 0; i < rows; i++)
	{
		for(int k = 0; k < cols; k++)
		{
			if(k == 0 && count % 2 == 0)
			{
				space();
				printw("|/|");
			}

			if(k == 0 && count % 2 != 0)
			{
				space();
				printw("|\\|");
			}

			if(i == charY && k == charX && grid[charY][charX] != '#' && grid[charY][charX] != '_')
				printw("@");
			else
				printw("%c", grid[i][k]);

			if(k == cols - 1 && count % 2 == 0)
				printw("|\\|");

			if(k == cols - 1 && count % 2 != 0)
				printw("|/|");

			count++;
		}
		printw("\n");
	}

	space();
	printw("X-");
	for(int i = 0; i < cols+2; i++)
	{
		printw("-");
	}
	printw("-X\n");
}


// Spawns a basic glider on the left border
void spawnGliderLeft(int row)
{
	if(row + 2 <= rows - 1)
	{
		grid[row][2] = '#';
		grid[row+1][0] = '#';
		grid[row+1][2] = '#';
		grid[row+2][1] = '#';
		grid[row+2][2] = '#';
	}
}


// Spawns a basic glider on the right border
void spawnGliderRight(int row)
{
	if(row + 2 <= rows - 1)
	{
		grid[row][cols-1 - 2] = '#';
		grid[row+1][cols-1 - 0] = '#';
		grid[row+1][cols-1 - 2] = '#';
		grid[row+2][cols-1 - 1] = '#';
		grid[row+2][cols-1 - 2] = '#';
	}
}

// Spawns a lightweight spaceship (LWSS) on the bottom border
void spawnLWSSBottom(int col)
{
	if(col + 5 < cols-1)
	{
		grid[rows-5][col] = '#';
		grid[rows-5][col+1] = '#';
		grid[rows-5][col+2] = '#';
		grid[rows-4][col] = '#';
		grid[rows-4][col+3] = '#';
		grid[rows-3][col] = '#';
		grid[rows-2][col] = '#';
		grid[rows-2][col+3] = '#';
		grid[rows-1][col+1] = '#';
	}
}


// Main code behind the events in Conway's. Randomly spawns stuff. 
void simConway(int counter)
{
	if(counter == 0)
	{
		spawnGliderLeft(0);
		spawnGliderRight(5);
	}


	int r = rand() % 30;

	if(r == 0 && counter != 0)
	{
		r = rand() % (rows/2) + rows/2;
		spawnGliderLeft(r);
	}

	r = rand() % 30;

	if(r == 0 && counter != 0)
	{
		r = rand() % (rows/2);
		spawnGliderLeft(r);
	}

	r = rand() % 30;

	if(r == 0)
	{
		r = rand() % (rows/2) + rows/2;
		spawnGliderRight(r);
	}

	r = rand() % 30;

	if(r == 0)
	{
		r = rand() % (rows/2);
		spawnGliderRight(r);
	}

	r = rand() % 45;

	if(r == 0 || counter == 0)
	{
		r = rand() % (cols/2);
		spawnLWSSBottom(r);
	}

	r = rand() % 45;

	if(r == 0 || counter == 0)
	{
		r = rand() % (cols/2) + cols/2;
		spawnLWSSBottom(r);
	}
}


// The same indentation system, just not with the ncurses window system. 
void spaceF()
{
	for(int i = 0; i < 8; i++)
	{
		printf("    ");
	}
}


// Pretty much just handles the opening title screen printing, outside of the ncurses window. 
void printScreen(char screen[50][55])
{
	for(int j = 0; j < 50; j++)
	{
		system("clear");

		for(int i = 0; i < 50; i++)
		{
			spaceF();
			
			for(int k = 0; k < 55; k++)
			{
				if(screen[i][k] == '\0')
					break;

				if(screen[i][k] == '!')
				{
					printf("\n");
					break;
				}

				printf("%c", screen[i][k]);
			}
		}
		printf("\n");

		// Shifts the screen up
		for(int i = 0; i < 49; i++)
		{
			for(int k = 0; k < 55; k++)
			{
				screen[i][k] = screen[i+1][k];
			}
		}

		if(j == 0)
			sleep(1);

		usleep(50000);

	}

}

// Same thing as above, but for the end title screen. 
void printEndScreen(char screen[50][55])
{
	char tempScreen[50][55];

	for(int j = 0; j < 50; j++)
	{
		system("clear");
	
		for(int i = 0; i < j; i++)
		{
			spaceF();
			for(int k = 0; k < 55; k++)
			{
				if(tempScreen[i][k] == '\0')
					break;

				if(tempScreen[i][k] == '!')
				{
					printf("\n");
					break;
				}

				printf("%c", tempScreen[i][k]);
			}
		}
		printf("\n");

		// Each iteration provides the printed screen one more line 
		for(int i = 0; i < j+1; i++)
		{
			for(int k = 0; k < 55; k++)
			{
				tempScreen[i][k] = screen[i][k];
			}
		}


		if(j == 0)
			sleep(1);

		usleep(50000);

	}
}

// Resets the board to its "clear" state
// Note that the character is never actually represented on the board
void clearBoard()
{
	for(int i = 0; i < rows; i++)
	{
		for(int k = 0; k < cols; k++)
		{
			grid[i][k] = '.';
		}
	}
}


// Cycles to the next mode to be played 
void nextMode(int* mode)
{
	int r = rand() % 2;
	while(r == (*mode))
		r = rand() % 2;
	(*mode) = r;
}


// The warning blips for the Floor is Lava mode
// Give it a number from 0-11 inclusive and it will generate a pattern. 
void warning(int mode)
{
	if(mode == 0)
	{
		for(int i = 0; i < rows; i+=2)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '_';
			}
		}
	}
	else if(mode == 1)
	{
		for(int i = 1; i < rows; i+=2)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '_';
			}
		}
	}
	else if(mode == 2)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k+=2)
			{
				grid[i][k] = '_';
			}
		}
	}
	else if(mode == 3)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 1; k < cols; k+=2)
			{
				grid[i][k] = '_';
			}
		}
	}
	else if(mode == 4)
	{
		int count = 0;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				if(count % 2 == 1)
				{
					count++;
					continue;
				}
				grid[i][k] = '_';
				count++;
			}
		}
	}
	else if(mode == 5)
	{
		int count = 1;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				if(count % 2 == 1)
				{
					count++;
					continue;
				}
				grid[i][k] = '_';
				count++;
			}
		}
	}

	else if(mode == 6)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols/2; k++)
			{
				grid[i][k] = '_';
				
			}
		}	
	}
	else if(mode == 7)
	{
		
		for(int i = 0; i < rows/2; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '_';
				
			}
		}

		
	}
	else if(mode == 8)                            // Just know that this might segfault if you do arbitrary grid sizes. 
	{
		int count = cols-1;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < count; k++)
			{
				grid[i][k] = '_';
				
			}
			count-= rows / cols + 2;
		}
	}
	else if(mode == 9)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = cols/2; k < cols; k++)
			{
				grid[i][k] = '_';
				
			}
		}
	}
	else if(mode == 10)
	{
		for(int i = rows/2; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '_';
				
			}
		}
	}
	else if(mode == 11)
	{
		int count = 1;
		for(int i = rows-1; i >= 0; i--)
		{
			for(int k = count; k < cols; k++)
			{
				grid[i][k] = '_';
				
			}
			count+= rows / cols + 2;
		}
	}
}


// The above, but generates the danger character ('#') when it does its thing. 
// Also can end the game if the player stands on one. 
void death(int mode, int* end)
{
	if(mode == 0)
	{
		for(int i = 0; i < rows; i+=2)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}
	}
	else if(mode == 1)
	{
		for(int i = 1; i < rows; i+=2)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}
	}
	else if(mode == 2)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k+=2)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}
	}
	else if(mode == 3)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 1; k < cols; k+=2)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}
	}
	else if(mode == 4)
	{
		int count = 0;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				if(count % 2 == 1)
				{
					count++;
					continue;
				}
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
				count++;
			}
		}
	}
	else if(mode == 5)
	{
		int count = 1;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				if(count % 2 == 1)
				{
					count++;
					continue;
				}
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
				count++;
			}
		}
	}
	else if(mode == 6)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < cols/2; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}	
	}
	else if(mode == 7)
	{
		for(int i = 0; i < rows/2; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}
	}
	else if(mode == 8)
	{
		int count = cols-1;
		for(int i = 0; i < rows; i++)
		{
			for(int k = 0; k < count; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
			count-= rows / cols + 2;
		}
	}
	else if(mode == 9)
	{
		for(int i = 0; i < rows; i++)
		{
			for(int k = cols/2; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
			}
		}	
	}
	else if(mode == 10)
	{
		for(int i = rows/2; i < rows; i++)
		{
			for(int k = 0; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
				
			}
		}
	}
	else if(mode == 11)
	{
		int count = 1;
		for(int i = rows-1; i >= 0; i--)
		{
			for(int k = count; k < cols; k++)
			{
				grid[i][k] = '#';
				if(charY == i && charX == k)
					*end = 1;
				
			}
			count+= rows / cols + 2;
		}
	}
}


// I had this in main, but it cluttered main, so I kinda just sent it here along with any relevant data as pointed parameters. 
void updateLava(long long int * counter, int * r, int * prevR, int * end)
{
	if(*counter == 0)
	{
		*r = rand() % 12;
		while(*r == *prevR)
		{
			*r = rand() % 12;
		}
		*prevR = *r;
	}

	if((*counter % 32 == 0 || *counter % 32 == 1) && *counter != 0 && *counter != 1)
	{
		death(*r, end);
		//mode = 2;
		//counter = -1;
		if(*counter % 32 == 1)
		{
			*r = rand() % 12;
			while(*r == *prevR)
			{
				*r = rand() % 12;
			}
			*prevR = *r;
		}
	}
	else if(*counter == 0 || *counter == 1 || *counter % 32 == 8 || *counter % 32 == 9 || *counter % 32 == 16 || *counter % 32 == 17 || *counter % 32 == 24 || *counter % 32 == 25)//|| counter % 8 == 1)
	{
		warning(*r);
	}
	else
	{
		clearBoard();
	}
}


int main(){
	// I'm not sure how necessary this is, but the ncurses thing was softlocking my terminal, so I guess just make a copy of the terminal that it can softlock. 
	pid_t p;
	p = fork();
if(p == 0)      // Yeah this is bad indentation form, but, like, it works. Also no code should really go in the parent process, so it doesn't really matter. 
{
	srand(time(NULL)); // for the random number generator
	char titleScreen[50][55];
	FILE* titleFile = fopen("titlescreen.txt", "r");


	// Get the title screen
	char title[256];
	int titleIndex = 0;
	while(fgets(title, 55, titleFile) != NULL)
	{
		for(int k = 0; k < 55; k++)
		{
			titleScreen[titleIndex][k] = title[k];
		}

		titleIndex++;
	}

	fclose(titleFile);

	// Print the scrolling title screen
	printScreen(titleScreen);
	printf("Enter any key to continue: ");
	scanf("%s", title);

	system("clear");


	// Sets up the ncurses window. I honestly don't know much about how it works exactly. Stackoverflow provided this code, and I'm not about to mess with it. 
	// All I can say is that it kinda just makes a new window over the terminal that it takes keyboard input from and can also write to. 
	initscr();

	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	scrollok(stdscr, TRUE);



	char answer[256]; 
	FILE* inFile = fopen("board.txt", "r");
	char line[cols+8];


	// Initializes the grid
	int index = 0;
	while(fgets(line, cols + 8, inFile) != NULL)
	{
		if(index == rows+1)
			break;

		for(int i = 0; i < cols; i++)
		{
			if(line[i] == ' ')
				break;

			grid[index][i] = line[i];	

		}
		index++;
	}


	// Initial player position
	charX = cols / 2;
	charY = rows / 2;

	long long int score = 0;
	
	// Sets the mode title for the fancy border thingy. 
	char inst[256];
	sprintf(inst, "Game of Death: %lld", score);
	//strcpy(inst, "Game of Death");
	printState(inst, 15 + 1 + ((int)log10(score)));


	// long long is overkill, but this isn't exactly a Data Structures homework. 
	long long int counter = 0; // Swap to 475 to debug mode 1. 
	int end = 0;

	int gridRef[rows][cols];

	/*
	0 == Conway
	1 == Floor is Lava


	*/
	int mode = 0; 


	// At this point this is basically "while(1)" but you get the point. 
	while(strcmp(answer, "exit") != 0 && end == 0)
	{
		// Random number reference, and also its previous value. 
		int r;
		int prevR; 

		// Switch gamemodes after 500 game cycles. 
		if(counter > 500 || (mode == 1 && counter > 480))
		{
			nextMode(&mode);
			clearBoard();
			counter = 0;
		}

		/*
		0 == Conway's
		1 == Lava


		*/
		if(mode == 0)
		{
			updateConway(gridRef, &end);
		}
		else if(mode == 1)
		{
			updateLava(&counter, &r, &prevR, &end);
		}

		

		if(end == 1)
		{
			break;
		}


		if(strcmp(answer, "w") == 0)
		{
			if(charY == 0)
				charY = rows - 1;
			else
				charY--;
		}
		else if(strcmp(answer, "s") == 0)
		{
			if(charY == rows - 1)
				charY = 0;
			else
				charY++;
		}
		else if(strcmp(answer, "a") == 0)
		{
			if(charX == 0)
				charX = cols - 1;
			else
				charX--;
		}
		else if(strcmp(answer, "d") == 0)
		{
			if(charX == cols - 1)
				charX = 0;
			else
				charX++;
		}


		if(mode == 0)
			simConway(counter);

		if(mode == 0)
		{
			sprintf(inst, "Game of Death: %lld", score);
			printState(inst, 15 + 1 + ((int)log10(score)));
		}
		if(mode == 1)
		{
			sprintf(inst, "Floor is Lava: %lld", score);
			printState(inst, 15 + 1 + ((int)log10(score)));
			clearBoard();
		}
		if(mode == 2)
		{
			sprintf(inst, "Floor is Lava: %lld", score);
			printState(inst, 15 + 1 + ((int)log10(score)));
			clearBoard();
		}


		// This takes user input, but if the user misses too many clock cycles it'll just go on without you. 
		answer[0] = getch();
		int miss = 0;
		while(!isalpha(answer[0]))
		{
			//printw("%c\n", answer[0]);
			if(kbhit())
			{
				answer[0] = getch();
				printw("%c\n", answer[0]);
			}
			refresh();
			sleep(0.5); // Yes, this does just wait 0 seconds because of integer rounding. That said, it works, so I'm not touching it. 
			miss++;

			if(miss > 1250) // 2500 was pretty good, 1250 is pretty fast. 
			{
				break;
			}
		}
			

		counter++;
		score++;
	}


	sprintf(inst, "You Lose... %lld", score);
	printState(inst, 11 + 1 + 1 + ((int)log10(score)));
	//strcpy(inst, "You Lose...");
	//printState(inst, 11);
	refresh();
	

	sleep(2);

	fclose(inFile);
	refresh();

	endwin();
	system("clear");


	// Do the score thing here. 
	for(int i = 0; i < 3; i++)
	{
		printf("\n\n\n\n\n");
		spaceF();
		printf("Score: %lld\n", score);
		usleep(750000);
		system("clear");
		usleep(750000);
	}




	titleFile = fopen("titlescreen.txt", "r");
	
	titleIndex = 0;
	while(fgets(title, 55, titleFile) != NULL)
	{
		for(int k = 0; k < 55; k++)
		{
			titleScreen[titleIndex][k] = title[k];
		}

		titleIndex++;
	}


	fclose(titleFile);

	printEndScreen(titleScreen);
	

	sleep(1);
	system("clear");
}
else
{
	wait(NULL);
	endwin();
	return 1;
}
}