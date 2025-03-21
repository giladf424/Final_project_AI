#pragma once
#include "definitions.h"
#include "State.h"
#include "NPC.h"
#include "Cell.h"
#include <queue>
#include "CompareCells.h"

class Warrior : public NPC
{
private:
	int bullets, grenades;
	bool aggressive;
public:
	Warrior(Position startPos, TeamID teamID);
	void SetBullets(int b) { bullets = b; }
	int GetBullets() { return bullets; }
	void SetGrenades(int g) { grenades = g; }
	int GetGrenades() { return grenades; }
	bool GetAggressive() { return aggressive; }

	const char* getType() override { return "Warrior"; }

	// A* Pathfinding
	std::vector<Cell*> RunAStar(int targetRow, int targetCol, int maze[MSZ][MSZ]);
	void AddNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, vector <Cell>& grays, vector <Cell>& black, int maze[MSZ][MSZ]);

	// Movement
	void move(int maze[MSZ][MSZ]) override;
};

// test