#include "Squire.h"
#include "IdleState.h"
#include "Warrior.h"

Squire::Squire(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_PACKS;
	reStocking = false;
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
		if (this->isAdjacentToMyPos(warrior->GetPosition()))
		{
			if (warrior->GetHp() < MAX_HP)
			{
				if (this->GetHealthPack() > 0)
				{
					warrior->SetHp(MAX_HP);
					this->SetHealthPack(this->GetHealthPack() - 1);
					this->SetReStocking(this->GetHealthPack() == 0 ? true : false);
				}
				else
					this->SetReStocking(true);
			}
			if (warrior->getAmmo() < MAX_BULLETS_WARRIOR)
			{
				if (this->GetBulletsPack() > MAX_BULLETS_WARRIOR - warrior->getAmmo())
				{
					this->bulletsPack -= (MAX_BULLETS_WARRIOR - warrior->getAmmo());
					warrior->SetBullets(MAX_BULLETS_WARRIOR);
				}
				else if (this->GetBulletsPack() > 0)
				{
					warrior->SetBullets(warrior->getAmmo() + this->GetBulletsPack());
					this->SetBulletsPack(0);
					this->SetReStocking(true);
				}
				else
					this->SetReStocking(true);
			}
			if (warrior->getGrenades() < MAX_GRENADES_WARRIOR)
			{
				if (this->GetGrenadesPack() > MAX_GRENADES_WARRIOR - warrior->getGrenades())
				{
					this->grenadesPack -= (MAX_GRENADES_WARRIOR - warrior->getGrenades());
					warrior->SetGrenades(MAX_GRENADES_WARRIOR);
				}
				else if (this->GetGrenadesPack() > 0)
				{
					warrior->SetGrenades(warrior->getGrenades() + this->GetGrenadesPack());
					this->SetGrenadesPack(0);
					this->SetReStocking(true);
				}
				else
					this->SetReStocking(true);
			}
			return warrior->isWarriorCanReturnToFight();
		}
	}
	return false;








}

bool Squire::moveToWarrior(Position warriorPos)
{
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		int numSteps = 0;
		Position nextPos = RunAStar(warriorPos, dupMaze, dupSecurityMap, &numSteps);
		move(nextPos);
		return this->isAdjacentToMyPos(warriorPos);
	}
	return false;
}




void Squire::refillResources()
{
	if (isMoving)
	{
		Position stashPos;
		int stashType = 0;
		if (GetHealthPack() == 0)
			stashType = (getAmmo() == 0 || getGrenades() == 0) ? BOTH_PACKS : MEDICINE_PACK;
		else if (getAmmo() == 0 || getGrenades() == 0)
			stashType = AMMUNITION_PACK;

		if (stashType != 0)
		{
			stashPos = findNearestStash(stashType);
			DuplicateMaze(maze, dupMaze);
			int numSteps = 0;
			Position nextPos = RunAStar(stashPos, dupMaze, dupSecurityMap, &numSteps);
			move(nextPos);
			if (this->isAdjacentToMyPos(stashPos))
			{
				if (maze[stashPos.row][stashPos.col] == AMMUNITION_PACK)
				{
					SetBulletsPack(MAX_BULLETS_SQUIRE);
					SetGrenadesPack(MAX_GRENADES_SQUIRE);
				}
				else
					SetHealthPack(MAX_HP_PACKS);
				if (stashType != BOTH_PACKS)
					this->GetState()->Transition(this);
			}
		}
		else
			this->GetState()->Transition(this);
	}
}

Position Squire::findNearestStash(int stashType)
{
	Position nearestStashAmmo = { -1, -1 };
	Position nearestStashMed = { -1, -1 };
	double minDistanceAmmo = INT_MAX;
	double minDistanceMed = INT_MAX;
	for (Position p : ammunitionStash)
	{
		double distance = Team::findDistance(GetPosition(), p);
		if (distance < minDistanceAmmo)
		{
			minDistanceAmmo = distance;
			nearestStashAmmo.row = p.row;
			nearestStashAmmo.col = p.col;
		}
	}
	for (Position p : medicineStash)
	{
		double distance = Team::findDistance(GetPosition(), p);
		if (distance < minDistanceMed)
		{
			minDistanceMed = distance;
			nearestStashMed.row = p.row;
			nearestStashMed.col = p.col;
		}
	}
	if (stashType == BOTH_PACKS)
		return (minDistanceAmmo <= minDistanceMed) ? nearestStashAmmo : nearestStashMed;
	else if (stashType == AMMUNITION_PACK)
		return nearestStashAmmo;
	else
		return nearestStashMed;

}

