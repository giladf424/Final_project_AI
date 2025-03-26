#pragma once
#include "definitions.h"
#include "Cell.h"
#include "CompareCells.h"

class Corridor
{
private:
	vector<Position> path;
	Position entrances[MAX_CORRIDORS_ENTRANCES];
	int connectedRooms[MAX_CORRIDORS_ENTRANCES];
public:
	static vector<Corridor*> corridors;
	Corridor();
	Corridor(vector<Position> p, Position e[MAX_CORRIDORS_ENTRANCES], int c[MAX_CORRIDORS_ENTRANCES]);
	vector<Position> getPath() { return path; }
	Position getEntrance(int i) { return entrances[i]; }
	int getConnectedRoom(int i) { return connectedRooms[i]; }
	void setPath(vector<Position> p) { path = p; }
	void setEntrance(Position e, int i) { entrances[i] = e; }
	void setConnectedRoom(int c, int i) { connectedRooms[i] = c; }
	void FindAllCorridors();
	void CorridorRunBFS(Position sRoom, Position sCorridor);
};

