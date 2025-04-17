#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <unordered_map>
#include <queue>

using namespace std;

const int MSZ = 100;
const int NUM_ROOMS = 12;
const double WALL_COST = 5;
const double SPACE_COST = 1;

const int WALL = 0;
const int SPACE = 1;
const int MEDICINE_PACK = 2;
const int AMMUNITION_PACK = 3;
const int BOTH_PACKS = 1;
const int NPC_ = 4;
const int BLACK = 5;
const int GRAY = 6;
const double PI = 3.14;
const double SECURITY_FACTOR = 0.001;
const double SECURITY_COEFFICIENT = 5;
const int MAX_HP = 100;
const int HP_TH = 35;
const int HP_TH_AGGRESSIVE = 25;
const int AMMO_TH = 7;
const int AMMO_TH_AGGRESSIVE = 4;
const int GRENADE_TH = 2;
const int MAX_BULLETS_WARRIOR = 15;
const int MAX_GRENADES_WARRIOR = 10;
const int MAX_BULLETS_SQUIRE = 30;
const int MAX_GRENADES_SQUIRE = 20;
const int MAX_HP_PACKS = 10;
const int MEDICINE_PACK_AMOUNT = 2;
const int AMMUNITION_PACK_AMOUNT = 2;
const int TEAM_RANK = 2;
const int TEAM_SIZE = 2;
const int MAX_CORRIDORS_ENTRANCES = 4;
const int BFS_RADIUS_SQUIRE = 30;
const int DAMAGE = 10;
const int RELOAD_TIME = 10;


extern int maze[MSZ][MSZ];
extern int dupMaze[MSZ][MSZ];
extern double security_map[MSZ][MSZ];
extern double dupSecurityMap[MSZ][MSZ];
extern bool isAstar;

typedef struct
{
	int row, col;
}Position;

typedef struct
{
	int team, place;
}TeamID;

typedef struct
{
	int startRow, endRow;
	int startCol, endCol;
}RoomScope;

typedef struct
{
	int roomIndex;
	int numEnemies;
	int distance;
}RoomDetails;

extern RoomScope roomScopes[NUM_ROOMS];
extern vector<Position> ammunitionStash;
extern vector<Position> medicineStash;

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
//void UpdateSecurityMap(vector<Position> positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
int GetRoomIndex(Position pos);
void printMaze(int maze[MSZ][MSZ]);