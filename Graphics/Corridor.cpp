#include "Corridor.h"

vector<Corridor*> Corridor::corridors;
bool runBFS = false;
Corridor::Corridor()
{
	numOfRooms = 0;
	for (int i = 0; i < MAX_CORRIDORS_ENTRANCES; i++)
	{
		connectedRooms[i] = -1; // Initialize connectedRooms with a default value
	}
}

bool Corridor::addConnectedRoom(int roomIndex)
{
	if (numOfRooms < MAX_CORRIDORS_ENTRANCES)
	{
		connectedRooms[numOfRooms++] = roomIndex;
		return true;
	}
	std::cout << "Error: maximum number of connected rooms reached\n";
	return false;
}

void Corridor::CorridorRunBFS(Position sRoom, Position sCorridor, int dupMaze[MSZ][MSZ])
{
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	Cell* pc = new Cell(sCorridor.row, sCorridor.col, sRoom.row, sRoom.col, nullptr);
	pq.push(pc);
	
	Corridor* corridor = new Corridor();
	corridor->addConnectedRoom(GetRoomIndex(sRoom));
	corridor->getEntrances().push_back(sCorridor);
	corridor->getPath().push_back(sCorridor);
	corridors.push_back(corridor);

	runBFS = true;
	do
	{
		while (runBFS)
			CorridorRunBFSIteration(pq, dupMaze);

		if (!pq.empty())
		{
			pc = pq.top();
			pq.pop();
			corridors.back()->getPath().push_back({ pc->getRow(), pc->getCol() });
			Cell* newPc = new Cell(pc->getRow(), pc->getCol(), pc->getTargetRow(), pc->getTargetCol(), nullptr);
			pq.push(newPc);
			runBFS = true;
		}

	} while (runBFS);


}

void Corridor::CorridorRunBFSIteration(priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ])
{
	Cell* pCurrent;
	int r, c;
	bool go_on = false;

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

	if(dupMaze[r + 1][c] == SPACE && !isConnectedRoom(GetRoomIndex({r + 1, c})))
		go_on = CorridorCheckNeighbor(r + 1, c, pCurrent, pq, dupMaze);
	if (!go_on && dupMaze[r - 1][c] == SPACE && !isConnectedRoom(GetRoomIndex({ r - 1, c })))
		go_on = CorridorCheckNeighbor(r - 1, c, pCurrent, pq, dupMaze);
	if (!go_on && dupMaze[r][c + 1] == SPACE && !isConnectedRoom(GetRoomIndex({ r, c + 1 })))
		go_on = CorridorCheckNeighbor(r, c + 1, pCurrent, pq, dupMaze);
	if (!go_on && dupMaze[r][c - 1] == SPACE && !isConnectedRoom(GetRoomIndex({ r, c - 1 })))
		go_on = CorridorCheckNeighbor(r, c - 1, pCurrent, pq, dupMaze);

}

bool Corridor::CorridorCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, int dupMaze[MSZ][MSZ])
{
	int roomIndex = GetRoomIndex({ r, c });
	if (roomIndex != -1)
	{
		corridors.back()->addConnectedRoom(roomIndex);
		CorridorRestorePath(pCurrent);
		runBFS = false;
		return true;
	}

	Cell* pc = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(), pCurrent);
	pq.push(pc);
	dupMaze[r][c] = GRAY;
	
	return false;
}

void Corridor::CorridorRestorePath(Cell* pc)
{
	Position p = {pc->getRow(), pc->getCol()};
	corridors.back()->path.push_back(p);
	corridors.back()->entrances.push_back(p);
	pc = pc->getParent();
	while (pc->getParent() != nullptr)
	{
		p = { pc->getRow(), pc->getCol() };
		corridors.back()->path.push_back(p);
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
