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
	Cell* pc = new Cell(GetPosition().row, GetPosition().col, 0, 0, 0, nullptr, dupMap);
	grays.push(pc);
	Cell* nextStep = nullptr;
	for (int i = 0; i < BFS_ITERATIONS; i++)
	{
		nextStep = RunBFSIteration(dupMaze, dupMap, grays);
		//if you found all the enemies
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

Cell* Squire::RunBFSIteration(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], queue<Cell*> grays)
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
			go_on = CheckNeighbor(row + 1, col, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row - 1][col] != WALL && dupMap[row - 1][col] != BLACK)
			go_on = CheckNeighbor(row - 1, col, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row][col + 1] != WALL && dupMap[row][col + 1] != BLACK)
			go_on = CheckNeighbor(row, col + 1, pCurrent, grays, dupMaze, dupMap);
		if (dupMap[row][col - 1] != WALL && dupMap[row][col - 1] != BLACK)
			go_on = CheckNeighbor(row, col - 1, pCurrent, grays, dupMaze, dupMap);
		return go_on;
	}

}

Cell* Squire::CheckNeighbor(int r, int c, Cell* pCurrent, queue<Cell*> grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	Cell* pNeighbor;
	if (dupMaze[r][c] = SPACE /*|| (dupMaze[r][c] == NPC_ && pCurrent->getParent() != nullptr)*/)
	{
		pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(), 0, pCurrent, dupMap);
		pNeighbor->ComputeH(dupMap);
		grays.push(pNeighbor);
		dupMap[r][c] = GRAY;
	}
	return (pNeighbor->getH() > grays.front()->getH()) ? pNeighbor : grays.front();
	
}


