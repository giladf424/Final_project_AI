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

void Squire::RunFromEnemyWithHeuristicLogic(NPC* nearestTeamate)
{
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		Position nextStep = RunBFS(dupMaze, dupSecurityMap, nearestTeamate);
		move(nextStep);
	}
}

Position Squire::RunBFS(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], NPC* n)
{
	queue<Cell*> grays;
	int roomIndex = n->GetRoomIndex();
	vector<Position> enemiesPos = Team::GetEnemiesPositionsInRoom(roomIndex, n->GetTeamID().team, true);
	Cell* pc = new Cell(pos.row, pos.col, nullptr);
	grays.push(pc);
	Cell* nextStep = nullptr;
	for (int i = 0; i < BFS_ITERATIONS; i++)
	{
		nextStep = RunBFSIteration(dupMaze, grays);
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

Cell* Squire::RunBFSIteration(int dupMaze[MSZ][MSZ], queue<Cell*>& grays)
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
		if (dupMaze[row + 1][col] != WALL)
			CheckNeighbor(row + 1, col, pCurrent, grays, dupMaze);
		if (dupMaze[row - 1][col] != WALL)
			CheckNeighbor(row - 1, col, pCurrent, grays, dupMaze);
		if (dupMaze[row][col + 1] != WALL)
			CheckNeighbor(row, col + 1, pCurrent, grays, dupMaze);
		if (dupMaze[row][col - 1] != WALL)
			CheckNeighbor(row, col - 1, pCurrent, grays, dupMaze);
		return pCurrent;
	}
}

void Squire::CheckNeighbor(int r, int c, Cell* pCurrent, queue<Cell*>& grays, int dupMaze[MSZ][MSZ])
{
	if (dupMaze[r][c] == SPACE)
	{
		Cell* pNeighbor = new Cell(r, c, pCurrent);
		dupMaze[r][c] = GRAY;
		grays.push(pNeighbor);
	}
}


