/*
	Developed by Max Feldman 
	First Created 4/17/2019
	Last Edited 4/18/2019
*/

#include "helper.h"
#include "std_lib_facilities.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <stropts.h>


constexpr char _up = 'w';
constexpr char _left = 'a';
constexpr char _down = 's';
constexpr char _right = 'd';

constexpr int width = 25;
constexpr int height = 15;

struct plot{
	int x, y;
	plot(int a, int b)
	:x(a), y(b){}
};

vector <vector<int>> grid;
/*
	note that there are four possible states/values for an element in grid
	0: plot is unoccupied
	1: plot is occupied by the snake
	2: plot is occupied by food
	3: plot is occupied by both the snake and food
*/
vector <plot> snake;
int score;
int bytes_waiting;

void display_grid();
void play_game();
int _kbhit();
bool overlaps_snake(int x, int y);
bool overlaps_snake(int x, int y, int start);


int main(){

	play_game();
	//endwin();
	return 0;
}

void play_game(){
	snake = vector<plot>();
	score = 1;
	//coordinates for the head of the snake
	int x = width/2;
	int y = height/2;
	//coordinates for the fruit
	int fx = better_rand(0, 14);
	int fy = better_rand(0, 14);

	char dir;
	snake.push_back({x,y});
	for (;;){
		//reset the grid and print everything in the appropriate place
		grid = vector <vector<int>> (width, vector<int>(height, 0));
		for (int i = 0; i < snake.size(); ++i)
			grid[snake[i].x][snake[i].y] = 1;
		grid[x][y] = 1;
		if (overlaps_snake(fx, fy))
			grid[fx][fy] = 3;
		else
			grid[fx][fy] = 2;
		display_grid();

		//handle input
		if (_kbhit()){
			char c;
			cin >> c;
			switch(c){
				case'w':
				case 'W':
					if (dir != _down)
						dir = _up;
					break;
				case 'a':
				case 'A':
					if (dir != _right)
						dir = _left;
					break;
				case 's':
				case 'S':
					if (dir != _up)
						dir = _down;
					break;
				case 'd':
				case 'D':
					if (dir != _left)
						dir = _right;
					break;
			}
		}
		//move in the appropriate direction depending on the input
		if (dir == _up)
			++y;
		else if (dir == _left)
			--x;
		else if (dir == _down)
			--y;
		else	//dir == _right
			++x;
		//if we move off the screen wrap us around to other side
		if (x < 0)
			x = width - 1;
		if (y < 0)
			y = height - 1;
		x %= width;
		y %= height;

		//if we hit the snake excluding the last plot we end the loop
		//also if we hit the end of the tail and it is also the fruit we break
		if (overlaps_snake(x, y, 1) ||
		 (overlaps_snake(x, y) && (x == fx && y == fy)) )
			break;
		//add the location of the head to the snake
		snake.push_back({x,y});
		//adjust the score when food is eaten
		//place more food in a different location
		if (x == fx && y == fy){
			score++;
			fx = better_rand(0, width - 1);
			fy = better_rand(0, height - 1);
		}
		else 
			//when we don't get food remove the last plot of the snake
			snake.erase(snake.begin() + 0);
		//based on the score we will let the system sleep for an appropriate amount of time
		double speed = 10.0 / (score + 25);
		string str = to_string(speed);
		system(("sleep " + str).c_str());	
	}
	cout << "Final Score: " << score << endl;
}

void display_grid(){
	system("clear");
	//display current score
	cout << "Score: " << score << endl;
	//display the top of the box
	for (int i=0; i < grid.size()+2; ++i)
		cout << '|';
	cout << '\n';
	//display each row
	for (int i=0; i < grid[0].size(); ++i){
		cout << '|';
		for (int j = 0; j < grid.size(); ++j){
			//print the correct character based on the state of the plot
			switch (grid[j][grid[j].size() - 1 - i]){
				case 0:
					cout << ' ';
					break;
				case 1:
					cout << '#';
					break;
				case 2:
					cout << '*';
					break;
				case 3:
					cout << '@';
					break;
			}
		}
		cout << "|\n";
	}
	//display the bottom of the box
	for (int i=0; i < grid.size()+2; ++i)
		cout << '|';
	cout << '\n';
}

bool overlaps_snake(int x, int y) {
	for (int i = 0; i < snake.size(); ++i){
		if (snake[i].x == x && snake[i].y == y)
			return true;
	}
	return false;
}

//this is useful for when we don't care about overlapping with the last plot
bool overlaps_snake(int x, int y, int start) {
	if (start < 0)
		start = 0;
	for (int i = start; i < snake.size(); ++i){
		if (snake[i].x == x && snake[i].y == y)
			return true;
	}
	return false;
}


//for this function I credit http://www.flipcode.com/archives/_kbhit_for_Linux.shtml
//this function helps us see if the keyboard has been pressed
int _kbhit(){
	constexpr int STDIN = 1;
	termios term;
	tcgetattr(STDIN, &term);
	term.c_lflag &= ~ICANON;
	tcsetattr(STDIN, TCSANOW, &term);
	setbuf(stdin, NULL);
	ioctl( STDIN, FIONREAD, &bytes_waiting);
	return bytes_waiting;
}
