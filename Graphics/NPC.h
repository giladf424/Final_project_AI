#pragma once
#include "definitions.h"
#include "State.h"
#include "Cell.h"
#include "CompareCells.h"
#include "Corridor.h"

class State;

class NPC
{
protected:
	int hp;
	TeamID id;
	bool isAlive = true;
	bool isMoving;
	Position pos;
	Position prevPos;
	int corridorIndex;
	State* pCurrentState;
public:
	NPC(Position startPos, TeamID teamID);
	void SetState(State* pState) { pCurrentState = pState; }
	State* GetState() { return pCurrentState; }
	void SetPosition(Position p) { pos = p; }
	Position GetPosition() { return pos; }
	Position GetPrevPosition() { return prevPos; }
	int GetCorridorIndex() { return corridorIndex; }
	void SetCorridorIndex(int index) { corridorIndex = index; }
	void SetPrevPosition(Position p) { prevPos = p; }
	void SetIsMoving(bool value) { isMoving = value; }
	bool GetIsMoving() { return isMoving; }
	void SetHp(int h) { hp = h; }
	int GetHp() { return hp; }
	void SetIsAlive(bool value) { isAlive = value; }
	bool GetIsAlive() { return isAlive; }
	TeamID GetTeamID() { return id; }
	int getRoomIndex();
	Position RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	Cell* RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	Cell* CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	Cell* RestorePath(Cell* pc);

	virtual const char* getType() = 0;
	void move(Position p); // Add this line
};

