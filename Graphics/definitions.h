#pragma once
#include <iostream>
const int MSZ = 100;

const int WALL = 0;
const int SPACE = 1;
const int MEDICINE_PACK = 2;
const int AMMUNITION_PACK = 3;
const double PI = 3.14;
const double SECURITY_FACTOR = 0.001;
const int MAX_HP = 100;
const int MAX_BULLETS_WARRIOR = 15;
const int MAX_GRENADES_WARRIOR = 10;
const int MAX_BULLETS_SQUIRE = 30;
const int MAX_GRENADES_SQUIRE = 20;
const int MAX_HP_SQUIRE = 100;
const int MEDICINE_PACK_AMOUNT = 2;
const int AMMUNITION_PACK_AMOUNT = 2;

typedef struct
{
	int row, col;
}Position;