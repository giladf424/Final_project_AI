#include "Squire.h"
#include "IdleState.h"
#include "Warrior.h"

Squire::Squire(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_PACKS;
	pCurrentState = new IdleState();
}

void Squire::MoveToTeamMate(Position sRoomCenter)
{
	// Move to the teammate position
	// ...
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		int roomToScanIndex = GetRoomIndex(sRoomCenter);
		vector<Position> enemiesPositions = Team::GetEnemiesPositionsInRoom(roomToScanIndex, GetTeamID().team, true);
		vector<Position> entrances = this->GetAllEntrancesToMyRoom(roomToScanIndex, true);
		UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
		UpdateSecurityMap(enemiesPositions, dupMaze, dupSecurityMap);
		entrances.clear();
		entrances = this->GetAllEntrancesToMyRoom(roomToScanIndex, false);
		for (Position p : entrances)
			dupMaze[p.row][p.col] = BLACK;

		vector<Position> enemyPos;
		Position safestPos = BFSRadius(sRoomCenter, enemyPos, BFS_RADIUS_SQUIRE, dupMaze, dupSecurityMap);
		if (!isValidPos(safestPos)) // dont forget to verifay that the squire not get stuck in teammate position
		{
			this->SetPrevPosition(this->GetPosition());
			std::cout << "No bestPos found from BFSRadius (Squire, move to teammate)\n";
			return;
		}
		DuplicateMaze(maze, dupMaze);
		//DuplicateSecurityMap(security_map, dupSecurityMap);
		int numSteps = 0;
		Position nextPos = RunAStar(safestPos, dupMaze, dupSecurityMap, &numSteps);
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
		vector<Position> entrances = this->GetAllEntrancesToMyRoom(this->getRoomIndex(), true);
		UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
		NPC* target = Team::findNearestEnemy(Team::GetNPCByPosition(GetPosition(), this->GetTeamID().team, this->GetTeamID().place));
		Position bestEscapePos = BFSRadius(GetPosition(), entrances, BFS_RADIUS_SQUIRE, dupMaze, dupSecurityMap);
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
	for (int i = 0; i < BFS_RADIUS_SQUIRE; i++)
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

int Squire::findSafestRoom(vector<RoomDetails> connectedRooms)
{
	int minNumEnemies = INT_MAX;
	int minDistance = INT_MAX;
	int safestRoomIndex = -1;
	for (auto it = connectedRooms.begin(); it != connectedRooms.end();)
	{
		if (it->distance > 30)
		{
			it = connectedRooms.erase(it); // Erase returns the next iterator
			continue; // Skip to the next iteration
		}
		else if (it->numEnemies < minNumEnemies)
		{
			minNumEnemies = it->numEnemies;
		}
		++it; // Increment the iterator only if no element was erased
	}
	// Find the room with the minimum number of enemies, if there is more than one, choose the one with the smallest distance
	for (auto it = connectedRooms.begin(); it != connectedRooms.end(); ++it)
	{
		if (it->numEnemies == minNumEnemies && it->distance < minDistance)
		{
			minDistance = it->distance;
			safestRoomIndex = it->roomIndex;
		}
	}
	
	return safestRoomIndex;
}

bool Squire::deliverToWarrior(NPC* w)
{
	
	if (w->getType() == "Warrior")
	{
		Warrior* warrior = static_cast<Warrior*>(w);
		if (warrior->GetIsAlive())
		{
			if (bulletsPack > 0)
			{
				warrior->SetBulletsPack(warrior->GetBulletsPack() + bulletsPack);
				bulletsPack = 0;
				return true;
			}
			else if (grenadesPack > 0)
			{
				warrior->SetGrenadesPack(warrior->GetGrenadesPack() + grenadesPack);
				grenadesPack = 0;
				return true;
			}
			else if (healthPack > 0)
			{
				warrior->SetHealthPack(warrior->GetHealthPack() + healthPack);
				healthPack = 0;
				return true;
			}
		}
	}

































	
}




void Squire::refillResources()
{
	if (isMoving)
	{
	}
}

