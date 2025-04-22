#include "Squire.h"
#include "IdleState.h"
#include "Warrior.h"

Squire::Squire(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_PACKS;
	reStocking = false;
	safetyRoomIndex = -1;
	pCurrentState = new IdleState();
}

void Squire::MoveToTeamMate(Position sRoomCenter)
{
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
		if (!isValidPos(safestPos))
		{
			this->SetPrevPosition(this->GetPosition());
			return;
		}
		DuplicateMaze(maze, dupMaze);
		int numSteps = 0;
		Position nextPos = RunAStar(safestPos, dupMaze, dupSecurityMap, &numSteps);
		Position nextPos2 = checkIfStuck(nextPos, safestPos);
		(isValidPos(nextPos2)) ? move(nextPos2) : move(nextPos);

	}
}

int Squire::findSafestRoom(vector<RoomDetails>& connectedRooms)
{
	int minNumEnemies = INT_MAX;
	int minDistance = INT_MAX;
	int safestRoomIndex = -1;
	bool found = false;
	RoomDetails* safestRoom = nullptr;
	for (auto it = connectedRooms.begin(); it != connectedRooms.end();)
	{
		if (it->distance > 30)
		{
			it = connectedRooms.erase(it);
			continue;
		}
		else if (it->numEnemies < minNumEnemies)
		{
			minNumEnemies = it->numEnemies;
		}
		if (it->roomIndex == this->safetyRoomIndex)
		{
			found = true;
			safestRoom = &(*it);
		}
		++it;
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
	if (found && safestRoom->numEnemies == minNumEnemies)
	{
		safestRoomIndex = this->safetyRoomIndex;
	}

	return safestRoomIndex;
}

bool Squire::deliverToWarrior(NPC* w)
{
	if (w->getType() == "Warrior")
	{
		Warrior* warrior = static_cast<Warrior*>(w);
		if (this->isAdjacentToMyPos(warrior->GetPosition()) || this->isSamePosAsMyPos(warrior->GetPosition()))
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
		if (this->isSamePosAsMyPos(warriorPos))
		{
			Position nextPos = { -1, -1 };
			if (!this->isSamePosAsMyPos(this->GetPrevPosition()))
				nextPos = this->GetPrevPosition();
			else
				nextPos = this->FindFreeSpaceToMove();

			move(nextPos);
			return true;
		}
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		int numSteps = 0;
		Position nextPos = RunAStar(warriorPos, dupMaze, dupSecurityMap, &numSteps);
		Position nextPos2 = this->checkIfStuck(nextPos, warriorPos);
		(isValidPos(nextPos2)) ? move(nextPos2) : move(nextPos);
		return (this->isAdjacentToMyPos(warriorPos) || this->isSamePosAsMyPos(warriorPos));
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
			DuplicateSecurityMap(security_map, dupSecurityMap);
			int numSteps = 0;
			Position nextPos = RunAStar(stashPos, dupMaze, dupSecurityMap, &numSteps);
			Position nextPos2 = this->checkIfStuck(nextPos, stashPos);
			(isValidPos(nextPos2)) ? move(nextPos2) : move(nextPos);
			//move(nextPos);
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

void Squire::runAwayFromEnemy()
{
	if (isMoving)
	{
		if ((rand() % 100) < 20)
		{
			move(this->pos);
			return;
		}
		DuplicateMaze(maze, dupMaze);
		Position bestEscapePos = this->RunAStarFlee(dupMaze);
		if (!isValidPos(bestEscapePos))
		{
			this->SetPrevPosition(this->GetPosition());
			std::cout << "No bestPos found from RunAstarFlee (Squire, runAwayFromEnemy)" << endl;
			return;
		}
		vector<Position> warriorsEnemies = Team::findAllWarriorsEnemies(this->GetTeamID().team);
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		//UpdateSecurityMap(warriorsEnemies, dupMaze, dupSecurityMap);
		int numSteps = 0;
		Position nextPos = RunAStar(bestEscapePos, dupMaze, dupSecurityMap, &numSteps);
		Position nextPos2 = checkIfStuck(nextPos, bestEscapePos);
		(isValidPos(nextPos2)) ? move(nextPos2) : move(nextPos);
	}
}

