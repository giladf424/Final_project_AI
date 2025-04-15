#pragma once
#include "definitions.h"
#include "State.h"
//#include "Cell.h"
//#include "CompareCells.h"
#include "Corridor.h"
//#include "Grenade.h"

//class Team; // Forward declaration

class State;

class NPC
{
protected:
	int hp;
	TeamID id;
	bool isAlive;
	bool isMoving;
	Position pos;
	Position prevPos;
	int prevStep;
	int corridorIndex;
	int prevRoomIndex;
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
	int GetPrevStep() { return prevStep; }
	void SetPrevStep(int step) { prevStep = step; }
	int getPrevRoomIndex() { return prevRoomIndex; }
	void SetPrevRoomIndex(int index) { prevRoomIndex = index; }

	int getRoomIndex();
	Position RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps);
	Cell* RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps);
	Cell* CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps);
	Cell* RestorePath(Cell* pc, int* numSteps);

	bool IsEnemyInCorridorConnectedToMyRoom(int corridorIndex);
	bool IsEnemyInSameRoom(int roomIndex);
	bool isValidPos(Position nextStep);
	bool isSamePosAsMyPos(Position p);
	bool isStillInSamePos();

	void move(Position nextPos); // Add this line
	Position BFSRadius(Position start, vector <Position> enemyPos, int radius, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	void BFSRadiusCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& q, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	bool IsEnemyInHitRange(Position myPos, Position enemyPos);
	Position getEntranceToCorridor(int corridorIndex, int roomIndex, bool forSecMap);
	vector<Position> GetAllEntrancesToMyRoom(int roomIndex, bool forSecMap);
	void UpdateSecurityMap(vector<Position> positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	vector<Position> GetEnemiesInHitRange(Position myPos, vector<Position> enemiesPos);
	void hitByBullet();

	virtual const char* getType() = 0;
	virtual int getAmmo() = 0;
	virtual int getGrenades() = 0;
};
