/*
	I use this file in many projects, as I often find dealing with RNGs
	frusturating, so I made this to give myself easy acess to some good
	random numbers.
*/

#include<random>
using namespace std;
random_device rd;
mt19937 mt(rd());

int better_rand(int min, int max){
	uniform_int_distribution<int> dist(min, max);
	return dist(mt);
}