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
	bool isStuck;
	int stuckness;
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
	void SetStuck(bool value) { isStuck = value; }
	bool GetStuck() { return isStuck; }
	int GetStuckness() { return stuckness; }
	void SetStuckness(int value) { stuckness = value; }

	int getRoomIndex();
	Position RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps);
	Cell* RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps, vector<Cell*>& toDelete);
	Cell* CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps, vector<Cell*>& toDelete);
	Cell* RestorePath(Cell* pc, int* numSteps);

	Position RunAStarFlee(int dupMaze[MSZ][MSZ]);
	void AStarFleeCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& grays, int dupMaze[MSZ][MSZ], vector<Position>& warriorsEnemy);

	Position RunBFS(Position target, int dupMaze[MSZ][MSZ], int* numSteps);
	Cell* RunBFSIteration(Position target, int dupMaze[MSZ][MSZ], queue<Cell*>& q , vector<Cell*>& toDelete, int* numSteps);
	Cell* BFSCheckNeighbor(int r, int c, Cell* pCurrent, Position target, int dupMaze[MSZ][MSZ], queue<Cell*>& q, vector<Cell*>& toDelete, int* numSteps);

	bool IsEnemyInCorridorConnectedToMyRoom(int corridorIndex);
	bool IsEnemyInSameRoom(int roomIndex);
	bool isValidPos(Position nextStep);
	bool isSamePosAsMyPos(Position p);
	bool isStillInSamePos();
	bool isAdjacentToMyPos(Position p);
	bool isSamePositions(Position p1, Position p2);
	Position FindFreeSpaceToMove();
	Position FindFreeSpaceToMoveInLoop();
	Position checkIfStuck(Position nextPos, Position targetPos);

	void move(Position nextPos); // Add this line
	Position BFSRadius(Position start, vector <Position>& enemyPos, int radius, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	void BFSRadiusCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& q, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], vector<Position>& enemiesInRoom);
	double IsEnemyInHitRange(Position myPos, Position enemyPos);
	Position getEntranceToCorridor(int corridorIndex, int roomIndex, bool forSecMap);
	vector<Position> GetAllEntrancesToMyRoom(int roomIndex, bool forSecMap);
	void UpdateSecurityMap(vector<Position>& positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ]);
	vector<HitDetails> GetEnemiesInHitRange(Position myPos, vector<Position>& enemiesPos);
	void hitByBullet();

	virtual const char* getType() = 0;
	virtual int getAmmo() = 0;
	virtual int getGrenades() = 0;
};
