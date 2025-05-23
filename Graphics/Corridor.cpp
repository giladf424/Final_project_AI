#include "Corridor.h"

vector<Corridor*> Corridor::corridors;
bool runBFS = false;
int Corridor::nextId = 0;
Corridor::Corridor() : id(nextId++)
{
	numOfRooms = 0;
	for (int i = 0; i < MAX_CORRIDORS_ENTRANCES; i++)
	{
		connectedRooms[i] = -1;
	}
}

bool Corridor::addConnectedRoom(int roomIndex)
{
	if (numOfRooms < MAX_CORRIDORS_ENTRANCES)
	{
		connectedRooms[numOfRooms++] = roomIndex;
		return true;
	}
	return false;
}

void Corridor::CorridorRunBFS(Position sRoom, Position sCorridor, int dupMaze[MSZ][MSZ])
{
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	vector<Cell*> toDelete;
	Cell* pc = new Cell(sCorridor.row, sCorridor.col, sRoom.row, sRoom.col, nullptr);
	pq.push(pc);
	toDelete.push_back(pc);

	Corridor* corridor = this;
	corridor->addConnectedRoom(GetRoomIndex(sRoom));
	corridor->entrances.push_back(sCorridor);
	corridor->path.push_back(sCorridor);

	runBFS = true;
	do
	{
		while (runBFS)
			corridor->CorridorRunBFSIteration(pq, dupMaze, toDelete);

		if (!pq.empty())
		{
			pc = pq.top();
			pq.pop();
			corridor->path.push_back({ pc->getRow(), pc->getCol() });
			Cell* newPc = new Cell(pc->getRow(), pc->getCol(), pc->getTargetRow(), pc->getTargetCol(), nullptr);
			pq.push(newPc);
			toDelete.push_back(newPc);
			runBFS = true;
		}

	} while (runBFS);

	while (!pq.empty())
	{
		Cell* p = pq.top();
		pq.pop();
	}
	while (!toDelete.empty())
	{
		Cell* p = toDelete.back();
		toDelete.pop_back();
		delete p;
		p = nullptr;
	}

}

void Corridor::CorridorRunBFSIteration(priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ], vector<Cell*>& toDelete)
{
	Cell* pCurrent;
	int r, c;
	bool go_on = false;
	Corridor* corridor = this;

	if (pq.empty())
	{
		runBFS = false;
		return;
	}
	pCurrent = pq.top();
	pq.pop();

	r = pCurrent->getRow();
	c = pCurrent->getCol();

	dupMaze[r][c] = BLACK;

	if (dupMaze[r + 1][c] == SPACE && !corridor->isConnectedRoom(GetRoomIndex({ r + 1, c })))
		go_on = corridor->CorridorCheckNeighbor(r + 1, c, pCurrent, pq, dupMaze, toDelete);
	if (!go_on && dupMaze[r - 1][c] == SPACE && !corridor->isConnectedRoom(GetRoomIndex({ r - 1, c })))
		go_on = corridor->CorridorCheckNeighbor(r - 1, c, pCurrent, pq, dupMaze, toDelete);
	if (!go_on && dupMaze[r][c + 1] == SPACE && !corridor->isConnectedRoom(GetRoomIndex({ r, c + 1 })))
		go_on = corridor->CorridorCheckNeighbor(r, c + 1, pCurrent, pq, dupMaze, toDelete);
	if (!go_on && dupMaze[r][c - 1] == SPACE && !corridor->isConnectedRoom(GetRoomIndex({ r, c - 1 })))
		go_on = corridor->CorridorCheckNeighbor(r, c - 1, pCurrent, pq, dupMaze, toDelete);

}

bool Corridor::CorridorCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ], vector<Cell*>& toDelete)
{
	int roomIndex = GetRoomIndex({ r, c });
	if (roomIndex != -1)
	{
		Corridor* corridor = this;
		corridor->addConnectedRoom(roomIndex);
		corridor->CorridorRestorePath(pCurrent);
		runBFS = false;
		return true;
	}

	Cell* pc = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(), pCurrent);
	pq.push(pc);
	toDelete.push_back(pc);
	dupMaze[r][c] = GRAY;

	return false;
}

void Corridor::CorridorRestorePath(Cell* pc)
{
	Position p = { pc->getRow(), pc->getCol() };
	Corridor* corridor = this;
	corridor->path.push_back(p);
	corridor->entrances.push_back(p);
	pc = pc->getParent();
	while (pc->getParent() != nullptr)
	{
		p = { pc->getRow(), pc->getCol() };
		corridor->path.push_back(p);
		pc = pc->getParent();
	}
}

bool Corridor::isEntrance(Position p)
{
	for (Position entrance : entrances)
	{
		if (entrance.row == p.row && entrance.col == p.col)
			return true;
	}
	return false;
}

bool Corridor::isConnectedRoom(int roomIndex)
{
	for (int i = 0; i < numOfRooms; i++)
	{
		if (connectedRooms[i] == roomIndex)
			return true;
	}
	return false;
}

Position Corridor::getEntranceToRoom(int roomIndex)
{
	for (Position entrance : entrances)
	{
		if (GetRoomIndex({ entrance.row + 1, entrance.col }) == roomIndex)
			return entrance;
		if (GetRoomIndex({ entrance.row - 1, entrance.col }) == roomIndex)
			return entrance;
		if (GetRoomIndex({ entrance.row, entrance.col + 1 }) == roomIndex)
			return entrance;
		if (GetRoomIndex({ entrance.row, entrance.col - 1 }) == roomIndex)
			return entrance;
	}
	return { -1, -1 };
}

int Corridor::getDistanceBetweenConnectedRooms(int roomIndex1, int roomIndex2)
{
	Position p1 = getEntranceToRoom(roomIndex1);
	Position p2 = getEntranceToRoom(roomIndex2);
	if (p1.row == -1 || p2.row == -1)
		return -1;

	int distance = abs(p1.row - p2.row) + abs(p1.col - p2.col);
	if (p1.row == p2.row || p1.col == p2.col)
		return distance;
	else
		return distance + 1;
}

Corridor* Corridor::getCorridorById(int id)
{
	for (Corridor* c : corridors)
	{
		if (c->getId() == id)
			return c;
	}
	return nullptr;
}