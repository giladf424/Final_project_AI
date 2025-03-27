#include "NPC.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	pos = startPos;
	prevPos = {-1, -1};
	corridorIndex = -1;
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

Position NPC::RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
	Cell* pc = new Cell(GetPosition().row,GetPosition().col, target.row, target.col, 0, nullptr, dupMap);
	grays.push(pc);
	Cell* nextStep = nullptr;

	while (!nextStep)
		nextStep = RunAStarIteration(target, grays, dupMaze, dupMap);
	
	Position nextPos = Position { nextStep->getRow(), nextStep->getCol() };

	while (!grays.empty())
	{
		Cell* c = grays.top();
		grays.pop();
		delete c;
		c = nullptr;
	}
	return nextPos;
}

Cell* NPC::RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
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

	dupMap[row][col] = BLACK;
	
	if (dupMaze[row + 1][col] != WALL && dupMap[row + 1][col] != BLACK)
		go_on = CheckNeighbor(row + 1, col, pCurrent, target, grays, dupMaze, dupMap);
	if (!go_on && dupMaze[row - 1][col] != WALL && dupMap[row - 1][col] != BLACK)
		go_on = CheckNeighbor(row - 1, col, pCurrent, target, grays, dupMaze, dupMap);
	if (!go_on && dupMaze[row][col - 1] != WALL && dupMap[row][col - 1] != BLACK)
		go_on = CheckNeighbor(row, col - 1, pCurrent, target, grays, dupMaze, dupMap);
	if (!go_on && dupMaze[row][col + 1] != WALL && dupMap[row][col + 1] != BLACK)
		go_on = CheckNeighbor(row, col + 1, pCurrent, target, grays, dupMaze, dupMap);

	return go_on;
}

Cell* NPC::CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	if (r == target.row && c == target.col)
		return RestorePath(pCurrent);

	if (dupMaze[r][c] == SPACE || (dupMaze[r][c] == NPC_ && pCurrent->getParent() != nullptr))
	{
		Cell* pc = new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap);
		grays.push(pc);
		dupMaze[r][c] = GRAY;
	}
	return nullptr;
}

Cell* NPC::RestorePath(Cell* pc)
{
	while (pc->getParent() && pc->getParent()->getParent() != nullptr)
		pc = pc->getParent();
	
	return pc;
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
