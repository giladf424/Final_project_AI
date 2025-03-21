#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
using namespace std;

const int MSZ = 100;

const double WALL_COST = 5;
const double SPACE_COST = 1;

const int WALL = 0;
const int SPACE = 1;
const int MEDICINE_PACK = 2;
const int AMMUNITION_PACK = 3;
const int NPC_ = 4;
const double PI = 3.14;
const double SECURITY_FACTOR = 0.001;
const double SECURITY_COEFFICIENT = 10;
const int MAX_HP = 100;
const int MAX_BULLETS_WARRIOR = 15;
const int MAX_GRENADES_WARRIOR = 10;
const int MAX_BULLETS_SQUIRE = 30;
const int MAX_GRENADES_SQUIRE = 20;
const int MAX_HP_SQUIRE = 100;
const int MEDICINE_PACK_AMOUNT = 2;
const int AMMUNITION_PACK_AMOUNT = 2;
const int TEAM_RANK = 2;
const int TEAM_SIZE = 2;

typedef struct
{
	int row, col;
}Position;

typedef struct
{
	int team, place;
}TeamID;

//const unordered_map<string, array<double, 3>> team_colors = {
//    {"FPurple", {217 / (double)255, 1 / (double)255, 122 / (double)255}},
//    {"SPurple", {219 / (double)255, 62 / (double)255, 177 / (double)255}},
//    {"FOrange", {252 / (double)255, 76 / (double)255, 2 / (double)255}},
//    {"SOrange", {255 / (double)255, 153 / (double)255, 19 / (double)255}}
//};
const unordered_map<string, array<double, 3>> team_colors = {
    {"FRed", {139 / (double)255, 0 / (double)255, 0 / (double)255}}, // Dark Red
    {"SRed", {255 / (double)255, 99 / (double)255, 71 / (double)255}}, // Light Red
    {"FYellow", {204 / (double)255, 204 / (double)255, 0 / (double)255}}, // Dark Yellow
    {"SYellow", {255 / (double)255, 255 / (double)255, 102 / (double)255}} // Light Yellow
};

void DuplicateMaze(int source[MSZ][MSZ], int target[MSZ][MSZ]);
void DuplicateSecurityMap(double source[MSZ][MSZ], double target[MSZ][MSZ]);