#include "Squire.h"
#include "IdleState.h"

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
		int numSteps = 0;
		Position nextPos = RunAStar(teammatePos, dupMaze, dupSecurityMap, &numSteps);
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
		isAstar = false;
		Position nextStep = { -1, -1 };
		vector<Position> entrances = this->GetAllEntrancesToMyRoom();
		UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
		NPC* target = Team::findNearestEnemy(Team::GetNPCByPosition(GetPosition()));
		Position bestEscapePos = BFSRadius(GetPosition(), target->GetPosition(), BFS_ITERATIONS, dupMaze, dupSecurityMap);
		//Position nextStep = RunBFS(dupMaze, dupSecurityMap, nearestTeamate);
		move(bestEscapePos);
	}
}

Position Squire::RunBFS(int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], NPC* n)
{
	priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
	int roomIndex = n->getRoomIndex();
	vector<Position> enemiesPos = Team::GetEnemiesPositionsInRoom(roomIndex, n->GetTeamID().team, true);
	Cell* pc = new Cell(pos.row, pos.col, nullptr, dupMap);
	pc->CalcH(dupMap, enemiesPos);
	pc->CalcF();
	grays.push(pc);
	Cell* nextStep = nullptr;
	for (int i = 0; i < BFS_ITERATIONS; i++)
	{
		nextStep = RunBFSIteration(dupMaze, grays, dupMap, enemiesPos);
		//HERE YOU NEED TO ADD THE HEURISTIC CALCULATION TO DECIDE TO SAVE THE BEST CELL TO MOVE TO
	}
	Position nextPos = Position{ nextStep->getRow(), nextStep->getCol() };
	while (!grays.empty())
	{
		Cell* c = grays.top();
		grays.pop();
		delete c;
		c = nullptr;
	}
	return nextPos;
}

Cell* Squire::RunBFSIteration(int dupMaze[MSZ][MSZ], priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, double dupMap[MSZ][MSZ], vector<Position> enemiesPos)
{
	Cell* pCurrent;
	int row, col;
	Cell* go_on = nullptr;
	if (grays.empty())
		return nullptr;
	else 
	{
		pCurrent = grays.top();
		grays.pop();
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		dupMaze[row][col] = BLACK;
		if (dupMaze[row + 1][col] != WALL)
			CheckNeighbor(row + 1, col, pCurrent, grays, dupMaze, dupMap, enemiesPos);
		if (dupMaze[row - 1][col] != WALL)
			CheckNeighbor(row - 1, col, pCurrent, grays, dupMaze, dupMap, enemiesPos);
		if (dupMaze[row][col + 1] != WALL)
			CheckNeighbor(row, col + 1, pCurrent, grays, dupMaze, dupMap, enemiesPos);
		if (dupMaze[row][col - 1] != WALL)
			CheckNeighbor(row, col - 1, pCurrent, grays, dupMaze, dupMap, enemiesPos);
		return pCurrent;
	}
}

void Squire::CheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], vector<Position> enemiesPos)
{
	if (dupMaze[r][c] == SPACE)
	{
		Cell* pNeighbor = new Cell(r, c, pCurrent, dupMap);
		pNeighbor->CalcH(dupMap, enemiesPos);
		pNeighbor->CalcF();
		dupMaze[r][c] = GRAY;
		grays.push(pNeighbor);
	}
}


