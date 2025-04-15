#pragma once
#include "NPC.h"
//#include "IdleState.h"
#include <queue>
//#include "CompareCells.h"
//#include "Cell.h"
//#include "definitions.h"

class Squire : public NPC
{
private:
	int bulletsPack, grenadesPack, healthPack;
public:
	Squire(Position startPos, TeamID teamID);
	void SetBulletsPack(int b) { bulletsPack = b; }
	void SetGrenadesPack(int g) { grenadesPack = g; }
	void SetHealthPack(int h) { healthPack = h; }
	int GetHealthPack() { return healthPack; }

	const char* getType() override { return "Squire"; }
	int getAmmo() override { return bulletsPack; }
	int getGrenades() override { return grenadesPack; }

	// Movement
	void MoveToTeamMate(Position sRoomCenter);
	void RunFromEnemyWithHeuristicLogic(NPC* nearestTeamate);
	//void move(int maze[MSZ][MSZ]) override;
	Position RunBFS(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], NPC* n);
	Cell* RunBFSIteration(int dupMaze[MSZ][MSZ], priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, double dupMap[MSZ][MSZ], vector<Position> enemiesPos);
    void CheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], vector<Position> enemiesPos);
	int findSafestRoom(vector<RoomDetails> connectedRooms);

};

