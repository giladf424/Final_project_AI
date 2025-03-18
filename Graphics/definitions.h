#pragma once
const int MSZ = 100;

const int SPACE = 1;
const int WALL = 0;
const double PI = 3.14;
const double SECURITY_FACTOR = 0.001;
const int MAX_HP = 100;
const int MAX_BULLETS_WARRIOR = 15;
const int MAX_GRENADES_WARRIOR = 10;
const int MAX_BULLETS_SQUIRE = 30;
const int MAX_GRENADES_SQUIRE = 20;
const int MAX_HP_SQUIRE = 100;

typedef struct
{
	int row, col;
}Position;