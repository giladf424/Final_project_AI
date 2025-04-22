#include "Team.h"
#include "Warrior.h"

vector<Team*> Team::Teams;
Team::Team(vector<array<double, 3>> tc)
{
	teamColor = tc;
	teamSize = 0;
	teamID = generateTeamID();
}

TeamID Team::generateTeamID()
{
	static int team = 0;
	TeamID tid = { team++, 0 };
	return tid;
}

TeamID Team::generateTeamIDForTeammate()
{
	TeamID tid = { GetTeamID().team , teamSize++ };
	return tid;
}

void Team::addTeammate(Position start_pos, bool isWarrior)
{
	if (isWarrior)
		teammates.push_back(new Warrior(start_pos, generateTeamIDForTeammate()));
	else
		teammates.push_back(new Squire(start_pos, generateTeamIDForTeammate()));
}

NPC* Team::findNearestEnemy(NPC* n)
{
	int minDistance = INT_MAX;
	NPC* nearestEnemy = nullptr;
	for (Team* t : Teams) {
		if (t->GetTeamID().team != n->GetTeamID().team) {
			for (NPC* enemy : t->GetTeammates()) {
				if (enemy->GetIsAlive()) {
					int distance = (int)findDistance(enemy->GetPosition(), n->GetPosition());
					if (distance < minDistance) {
						minDistance = distance;
						nearestEnemy = enemy;
					}
				}
			}
		}
	}
	return nearestEnemy;
}

Position Team::findNearestTeammate(NPC* n)
{
	int minDistance = INT_MAX;
	Position teammatePos = { -1, -1 };
	for (Team* t : Teams) {
		if (t->GetTeamID().team == n->GetTeamID().team) {
			for (NPC* teammate : t->GetTeammates()) {
				if (teammate->GetIsAlive() && teammate->GetTeamID().place != n->GetTeamID().place) {
					int distance = (int)findDistance(teammate->GetPosition(), n->GetPosition());
					if (distance < minDistance) {
						minDistance = distance;
						teammatePos = teammate->GetPosition();
					}
				}
			}
		}
	}

	return teammatePos;
}

vector<Position> Team::GetEnemiesPositionsInRoom(int roomIndex, int teamNum, bool onlyWarriors)
{
	vector<Position> enemiesPos;
	if (roomIndex < 0 || roomIndex >= NUM_ROOMS)
		return enemiesPos;

	RoomScope rs = roomScopes[roomIndex];
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != teamNum)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (onlyWarriors && strcmp(n->getType(), "Squire") == 0)
					continue;

				Position p = n->GetPosition();
				if (p.row >= rs.startRow && p.row <= rs.endRow && p.col >= rs.startCol && p.col <= rs.endCol)
					enemiesPos.push_back(p);
			}
		}
	}
	return enemiesPos;
}

NPC* Team::GetNPCByPosition(Position p, int teamNum, int id, int type)
{
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != teamNum && type == ENEMY || t->GetTeamID().team == teamNum && type == TEAMMATE || type == BOTH)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (n->GetTeamID().team == teamNum && n->GetTeamID().place == id)
					continue;
				if (n->isSamePosAsMyPos(p))
					return n;
			}
		}
	}
	return nullptr;
}

NPC* Team::findLowestHPEnemy(int teamNum)
{
	int minHP = INT_MAX;
	NPC* lowestHPEnemy = nullptr;
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != teamNum)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (n->GetHp() < minHP)
				{
					minHP = n->GetHp();
					lowestHPEnemy = n;
				}
			}
		}
	}
	return lowestHPEnemy;
}

NPC* Team::findNearestSquireEnemyOrTeammate(NPC* n, bool isEnemy)
{
	int minDistance = INT_MAX;
	NPC* nearestSquireEnemyOrTeammate = nullptr;
	for (Team* t : Teams)
	{
		if ((t->GetTeamID().team != n->GetTeamID().team && isEnemy) || (t->GetTeamID().team == n->GetTeamID().team && !isEnemy))
		{
			for (NPC* enemyOrTeammate : t->GetTeammates())
			{
				if (strcmp(enemyOrTeammate->getType(), "Squire") == 0 && enemyOrTeammate->GetIsAlive())
				{
					int distance = (int)findDistance(enemyOrTeammate->GetPosition(), n->GetPosition());
					if (distance < minDistance)
					{
						minDistance = distance;
						nearestSquireEnemyOrTeammate = enemyOrTeammate;
					}
				}
			}
		}
	}
	return nearestSquireEnemyOrTeammate;
}

Position Team::GetSquireEnemyPositionInRoom(int roomIndex, int teamNum)
{
	Position squireEnemyPos = { -1, -1 };
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != teamNum)
		{
			for (NPC* n : t->GetTeammates())
			{
				Position p = n->GetPosition();
				if (n->getRoomIndex() == roomIndex && strcmp(n->getType(), "Squire") == 0)
				{
					squireEnemyPos = p;
					break;
				}
			}
		}
	}
	return squireEnemyPos;
}

NPC* Team::findTargetEnemy(NPC* n, bool aggresive)
{
	int action = rand() % 100;
	if (action < 50 && aggresive || action <= 10)
	{
		NPC* target = findNearestSquireEnemyOrTeammate(n, true);
		if (target == nullptr)
		{
			return  findLowestHPEnemy(n->GetTeamID().team);
		}
		return target;
	}
	else if (action >= 50 && aggresive || action >= 90)
	{
		return findLowestHPEnemy(n->GetTeamID().team);
	}
	else
	{
		return findNearestEnemy(n);
	}
}

void Team::removeTeammate(NPC* dead)
{
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team == dead->GetTeamID().team)
		{
			for (size_t i = 0; i < t->GetTeammates().size(); i++)
			{
				if (t->GetTeammates()[i] == dead)
				{
					t->GetTeammates().erase(t->GetTeammates().begin() + i);
					t->removeWoundedWarrior(dead);
					t->teamSize--;
					break;
				}
			}
			break;
		}
	}
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != dead->GetTeamID().team)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (strcmp(n->getType(), "Warrior") == 0)
					if (((Warrior*)n)->GetTarget() == dead)
						((Warrior*)n)->SetTarget(nullptr);
			}
		}
	}
}

void Team::removeWoundedWarrior(NPC* dead)
{
	queue<NPC*> tempQueue;
	int count = 0;
	while (!woundedWarriors.empty())
	{
		NPC* n = woundedWarriors.front();
		woundedWarriors.pop();
		if (n == dead && count == 0)
			return;
		else if (n == dead && count > 0)
			continue;
		else
			tempQueue.push(n);

		count++;
	}
	this->woundedWarriors.swap(tempQueue);
}

double Team::findDistance(Position p1, Position p2)
{
	return sqrt(pow(p1.row - p2.row, 2) + pow(p1.col - p2.col, 2));
}

bool Team::isAnyBodyInMyPosition(Position p, int teamNum, int id)
{
	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (n->GetTeamID().team == teamNum && n->GetTeamID().place == id)
				continue;

			if (n->GetIsAlive() && n->isSamePosAsMyPos(p))
				return true;
		}
	}
	return false;
}

vector<RoomDetails> Team::getConnectedRooms(int roomIndex, int teamNum)
{
	if (roomIndex < 0 || roomIndex >= NUM_ROOMS)
		return vector<RoomDetails>();

	vector<RoomDetails> connectedRooms;
	int numEnemies = GetEnemiesPositionsInRoom(roomIndex, teamNum, true).size();
	connectedRooms.push_back({ roomIndex, numEnemies, 0 });
	for (Corridor* c : Corridor::corridors)
	{
		if (c->isConnectedRoom(roomIndex))
		{
			for (int i = 0; i < c->getNumOfRooms(); i++)
			{
				if (c->getConnectedRoom(i) != roomIndex)
				{
					RoomDetails rd = { c->getConnectedRoom(i), 0, 0 };
					rd.numEnemies = GetEnemiesPositionsInRoom(rd.roomIndex, teamNum, true).size();
					rd.distance = c->getDistanceBetweenConnectedRooms(roomIndex, rd.roomIndex);
					connectedRooms.push_back(rd);
				}
			}
		}
	}
	return connectedRooms;
}

Position Team::findRoomCenter(int roomIndex)
{
	if (roomIndex < 0 || roomIndex >= NUM_ROOMS)
		return { -1, -1 };

	RoomScope rs = roomScopes[roomIndex];
	int centerRow = (rs.startRow + rs.endRow) / 2;
	int centerCol = (rs.startCol + rs.endCol) / 2;
	Position center = { centerRow, centerCol };
	return center;
}

bool Team::isTeamsSizesEqualTwo()
{
	for (Team* t : Teams)
	{
		if (t->GetTeamSize() != 2)
			return false;
	}
	return true;
}

vector<Position> Team::findAllWarriorsEnemies(int teamNum)
{
	vector<Position> enemiesPos;
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != teamNum)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (strcmp(n->getType(), "Warrior") == 0 && n->GetIsAlive())
				{
					enemiesPos.push_back(n->GetPosition());
				}
			}
		}
	}
	return enemiesPos;
}

vector<Position> Team::findAllPlayesPositions(int teamNum, int id)
{
	vector<Position> playersPos;
	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (t->GetTeamID().team == teamNum && n->GetTeamID().place == id)
				continue;
			if (n->GetIsAlive())
				playersPos.push_back(n->GetPosition());
		}
	}
	return playersPos;
}

bool Team::isSafePosition(Position p, int teamNum, int id)
{
	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (t->GetTeamID().team == teamNum && n->GetTeamID().place == id)
				continue;
			if (n->GetIsAlive() && findDistance(n->GetPosition(), p) < 2)
				return false;
		}
	}
	return true;
}

bool Team::HandleGameState()
{
	int t1NumWarriors = 0, t2NumWarriors = 0;
	int t1NumSquires = 0, t2NumSquires = 0;

	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (n->GetIsAlive())
			{
				if (strcmp(n->getType(), "Warrior") == 0)
				{
					if (t->GetTeamID().team == 0)
						t1NumWarriors++;
					else
						t2NumWarriors++;

					if (n->getAmmo() > 0 || n->getGrenades() > 0)
						return false;
				}
				else if (strcmp(n->getType(), "Squire") == 0)
				{
					if (t->GetTeamID().team == 0)
						t1NumSquires++;
					else
						t2NumSquires++;
				}
			}
		}
	}

	bool t1IsLost = (t1NumSquires == 1 && t1NumWarriors == 0 && t2NumSquires == 0 && t2NumWarriors > 0);
	bool t2IsLost = (t2NumSquires == 1 && t2NumWarriors == 0 && t1NumSquires == 0 && t1NumWarriors > 0);

	if (t1NumWarriors == 0 && t2NumWarriors == 0)
		return true;

	if ((t1NumSquires == 0 && t2NumSquires == 0) || t1IsLost || t2IsLost)
	{
		for (Team* t : Teams)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (strcmp(n->getType(), "Warrior") == 0 && n->GetIsAlive())
				{
					Warrior* w = (Warrior*)n;
					w->SetGrenades(MAX_GRENADES_WARRIOR);
					w->SetBullets(MAX_BULLETS_WARRIOR);
				}
			}
		}
	}
	return false;
}
