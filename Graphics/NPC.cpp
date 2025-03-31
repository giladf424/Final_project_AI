#include "NPC.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	pos = startPos;
	prevPos = {-1, -1};
	corridorIndex = -1;
	pCurrentState = nullptr;
}

int NPC::getRoomIndex()
{
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		if (pos.row >= roomScopes[i].startRow && pos.row <= roomScopes[i].endRow && pos.col >= roomScopes[i].startCol && pos.col <= roomScopes[i].endCol)
			return i;
	}
	return -1;
}

Position NPC::RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	isAstar = true;
	priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
	Cell* pc = new Cell(GetPosition().row,GetPosition().col, target.row, target.col, 0, nullptr, dupMap);
	grays.push(pc);
	Cell* nextStep = nullptr;

	while (!nextStep)
		nextStep = RunAStarIteration(target, grays, dupMaze, dupMap, numSteps);
	
	Position nextPos = Position { nextStep->getRow(), nextStep->getCol() };

	while (!grays.empty())
	{
		Cell* c = grays.top();
		grays.pop();
		delete c;
		c = nullptr;
	}
	isAstar = false;
	return nextPos;
}

Cell* NPC::RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	if (grays.empty())
		return nullptr;

	Cell* go_on = nullptr;
	Cell* pCurrent = grays.top();
	grays.pop();

	int row = pCurrent->getRow();
	int col = pCurrent->getCol();

	if (row == target.row && col == target.col)
		return pCurrent;
	// need to check if instead assign BLACK to dupMap, we need to assign it to maze
	dupMap[row][col] = BLACK;
	
	if (dupMaze[row + 1][col] != WALL && dupMap[row + 1][col] != BLACK)
		go_on = CheckNeighbor(row + 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row - 1][col] != WALL && dupMap[row - 1][col] != BLACK)
		go_on = CheckNeighbor(row - 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row][col - 1] != WALL && dupMap[row][col - 1] != BLACK)
		go_on = CheckNeighbor(row, col - 1, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row][col + 1] != WALL && dupMap[row][col + 1] != BLACK)
		go_on = CheckNeighbor(row, col + 1, pCurrent, target, grays, dupMaze, dupMap, numSteps);

	return go_on;
}

Cell* NPC::CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	if (r == target.row && c == target.col)
		return RestorePath(pCurrent, numSteps);

	if (dupMaze[r][c] == SPACE || (dupMaze[r][c] == NPC_ && pCurrent->getParent() != nullptr))
	{
		Cell* pc = new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap);
		grays.push(pc);
		dupMaze[r][c] = GRAY;
	}
	return nullptr;
}

Cell* NPC::RestorePath(Cell* pc, int* numSteps)
{
    *numSteps = 2;
	while (pc->getParent() && pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
		*numSteps += 1;
	}
	
	return pc;
}

bool NPC::IsEnemyInCorridorConnectedToMyRoom(int corridorIndex)
{
	for (Corridor* c : Corridor::corridors)
	{
		if (c->getId() == corridorIndex)
			return c->isConnectedRoom(getRoomIndex());
	}
	return false;
}

bool NPC::IsEnemyInSameRoom(int roomIndex)
{
	int myRoomIndex = getRoomIndex();
	if (myRoomIndex != -1 && myRoomIndex == roomIndex)
		return true;
	return false;
}

void NPC::move(Position pos)
{
	maze[GetPosition().row][GetPosition().col] = SPACE;
	maze[pos.row][pos.col] = NPC_;
	SetPrevPosition(GetPosition());
	SetPosition(pos);
	if (getRoomIndex() == -1 && GetRoomIndex(GetPrevPosition()) != -1)
	{
		for (Corridor* c : Corridor::corridors)
		{
			if (c->isEntrance(GetPosition()))
			{
				SetCorridorIndex(c->getId());
				return;
			}
		}
	}
	else if (getRoomIndex() != -1 && GetRoomIndex(GetPrevPosition()) == -1)
		SetCorridorIndex(-1);
}

Position NPC::BFSRadius(Position start, Position enemyPos, int radius, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	isAstar = true;
	queue<Cell*> q;
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	Cell* pc = new Cell(start.row, start.col, nullptr, dupMap);
	q.push(pc);
	pq.push(pc);
	int r, c;
	int currentRadius = 0;

	while (!q.empty() && currentRadius <= radius)
	{
		int layerSize = q.size();
		for (int i = 0; i < layerSize; i++)
		{
			Cell* pCurrent = q.front();
			q.pop();
			r = pCurrent->getRow();
			c = pCurrent->getCol();
			dupMaze[r][c] = BLACK;

			if (dupMaze[r + 1][c] != WALL && dupMaze[r + 1][c] != BLACK)
				BFSRadiusCheckNeighbor(r + 1, c, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r - 1][c] != WALL && dupMaze[r - 1][c] != BLACK)
				BFSRadiusCheckNeighbor(r - 1, c, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r][c + 1] != WALL && dupMaze[r][c + 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c + 1, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r][c - 1] != WALL && dupMaze[r][c - 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c - 1, pCurrent, pq, q, dupMaze, dupMap);
		}
		currentRadius++;
	}
	Position bestPos = Position();
	while (!pq.empty())
	{
		Cell* isBest = pq.top();
		pq.pop();
		if (IsEnemyInHitRange({ isBest->getRow(), isBest->getCol() }, enemyPos))
		{
			bestPos.row = isBest->getRow();
			bestPos.col = isBest->getCol();
			delete isBest;
			isBest = nullptr;
			break;
		}
		delete isBest;
		isBest = nullptr;
	}
	while (!pq.empty())
	{
		Cell* p = pq.top();
		pq.pop();
		delete p;
		p = nullptr;
	}
	while (!q.empty())
	{
		Cell* p = q.front();
		q.pop();
		delete p;
		p = nullptr;
	}
	isAstar = false;
	return bestPos;
}

void NPC::BFSRadiusCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*> q, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	if (dupMaze[r][c] == SPACE || (dupMaze[r][c] == NPC_ && pCurrent->getParent() != nullptr))
	{
		Cell* pc = new Cell(r, c, pCurrent, dupMap);
		pq.push(pc);
		q.push(pc);
		dupMaze[r][c] = GRAY;
	}
}

bool NPC::IsEnemyInHitRange(Position myPos, Position enemyPos)
{
	bool inRange = false;

	Grenade* pg = new Grenade(myPos.row, myPos.col);

	if (pg->IsEnemyFoundByExplosion(maze, enemyPos))
		inRange = true;

	delete pg;
	return inRange;
}

Position NPC::getEntranceToCorridor(int corridorIndex)
{
	Corridor* corridor = Corridor::getCorridorById(corridorIndex);
	int roomIndex = getRoomIndex();
	if (corridor == nullptr)
	{
		std::cout << "Error: corridor not found\n";
		return { -1, -1 };
	}
	for (Position p : corridor->getEntrances())
	{
		if (GetRoomIndex({p.row + 1, p.col}) == roomIndex)
			return { p.row + 1, p.col };
		if (GetRoomIndex({ p.row - 1, p.col }) == roomIndex)
			return { p.row - 1, p.col };
		if (GetRoomIndex({ p.row, p.col + 1 }) == roomIndex)
			return { p.row, p.col + 1 };
		if (GetRoomIndex({ p.row, p.col - 1 }) == roomIndex)
			return { p.row, p.col - 1 };
	}
	std::cout << "Error: entrance not found\n";
	return {-1, -1};
}
