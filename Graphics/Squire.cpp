#include "Squire.h"

Squire::Squire(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_SQUIRE;
	pCurrentState = new IdleState();
}

void Squire::MoveToTeamMate(Position teammatePos)
{
	// Move to the teammate position
	// ...
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		// Run A* algorithm to find the path to the teammate
		Position nextPos = RunAStar(teammatePos, dupMaze, dupSecurityMap);
		// Move to the next position
		move(nextPos);

	}
}

void Squire::RunFromEnemy()
{
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		Position nextStep = RunBFS(dupMaze, dupSecurityMap);
		move(nextStep);
	}
}

Position Squire::RunBFS(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	queue<Cell*> grays;
	Cell* pc = new Cell(pos.row, pos.col, nullptr, dupMap);
	grays.push(pc);
	Cell* nextStep = nullptr;
	for (int i = 0; i < BFS_ITERATIONS; i++)
	{
		nextStep = RunBFSIteration(dupMaze, dupMap, grays);
		//HERE YOU NEED TO ADD THE HEURISTIC CALCULATION TO DECIDE TO SAVE THE BEST CELL TO MOVE TO
	}
	Position nextPos = Position{ nextStep->getRow(), nextStep->getCol() };
	while (!grays.empty())
	{
		Cell* c = grays.front();
		grays.pop();
		delete c;
		c = nullptr;
	}
	return nextPos;
}

Cell* Squire::RunBFSIteration(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], queue<Cell*>& grays)
{
	Cell* pCurrent;
	int row, col;
	Cell* go_on = nullptr;
	if (grays.empty())
		return nullptr;
	else 
	{
		pCurrent = grays.front();
		grays.pop();
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		dupMaze[row][col] = BLACK;
		if (dupMaze[row + 1][col] != WALL && dupMap[row + 1][col] != BLACK)
			CheckNeighbor(row + 1, col, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row - 1][col] != WALL && dupMap[row - 1][col] != BLACK)
			CheckNeighbor(row - 1, col, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row][col + 1] != WALL && dupMap[row][col + 1] != BLACK)
			CheckNeighbor(row, col + 1, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row][col - 1] != WALL && dupMap[row][col - 1] != BLACK)
			CheckNeighbor(row, col - 1, pCurrent, grays, dupMaze, dupMap);
		return pCurrent;
	}
}

void Squire::CheckNeighbor(int r, int c, Cell* pCurrent, queue<Cell*>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	//if (dupMaze[r][c] = SPACE /*|| (dupMaze[r][c] == NPC_ && pCurrent->getParent() != nullptr)*/)
	//{
		Cell* pNeighbor = new Cell(r, c, pCurrent, dupMap);
		dupMap[r][c] = GRAY;
		grays.push(pNeighbor);
	//}
}


