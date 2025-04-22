#pragma once
#include "definitions.h"
#include "CompareCells.h"

class Corridor
{
private:
	vector<Position> path;
	vector<Position> entrances;
	int connectedRooms[MAX_CORRIDORS_ENTRANCES];
	int numOfRooms;
	static int nextId;
	int id;
public:
	static vector<Corridor*> corridors;
	Corridor();
	vector<Position> getPath() { return path; }
	vector<Position> getEntrances() { return entrances; }
	int getConnectedRoom(int i) { return connectedRooms[i]; }
	int getNumOfRooms() { return numOfRooms; }
	bool addConnectedRoom(int roomIndex);
	void CorridorRunBFS(Position sRoom, Position sCorridor, int dupMaze[MSZ][MSZ]);
	void CorridorRunBFSIteration(priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ], vector<Cell*>& toDelete);
	bool CorridorCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ], vector<Cell*>& toDelete);
	void CorridorRestorePath(Cell* pc);
	bool isEntrance(Position p);
	bool isConnectedRoom(int roomIndex);
	int getId() const { return id; }
	Position getEntranceToRoom(int roomIndex);
	int getDistanceBetweenConnectedRooms(int roomIndex1, int roomIndex2);
	static Corridor* getCorridorById(int id);
};

