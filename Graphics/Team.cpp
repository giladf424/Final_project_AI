#include "Team.h"

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
	NPC* nearestEnemy = nullptr; // Default value if no enemy is found
	for (Team* t : Teams) {
		if (t->GetTeamID().team != n->GetTeamID().team) {
			for (NPC* enemy : t->GetTeammates()) {
				if (enemy->GetIsAlive()) {
					int distance = (int)sqrt(pow(enemy->GetPosition().row - n->GetPosition().row, 2) + pow(enemy->GetPosition().col - n->GetPosition().col, 2)); // Manhattan distance
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
	Position teammatePos = { -1, -1 }; // Default value if no teammate is found
	for (Team* t : Teams) {
		if (t->GetTeamID().team == n->GetTeamID().team) {
			for (NPC* teammate : t->GetTeammates()) {
				if (teammate->GetIsAlive() && teammate->GetTeamID().place != n->GetTeamID().place) {
					int distance = (int)sqrt(pow(teammate->GetPosition().row - n->GetPosition().row, 2) + pow(teammate->GetPosition().col - n->GetPosition().col, 2));
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

NPC* Team::GetTeammate(Position pos)
{
	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (n->GetPosition().row == pos.row && n->GetPosition().col == pos.col)
				return n;
		}
	}
	return nullptr;
}

NPC* Team::findLowestHPEnemy(int teamNum)
{
	int minHP = INT_MAX;
	NPC* lowestHPEnemy = nullptr; // Default value if no enemy is found
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

NPC* Team::findNearestSquireEnemy(NPC* n)
{
	int minDistance = INT_MAX;
	NPC* nearestSquireEnemy = nullptr; // Default value if no enemy is found
	for (Team* t : Teams)
	{
		if (t->GetTeamID().team != n->GetTeamID().team)
		{
			for (NPC* enemy : t->GetTeammates())
			{
				if (strcmp(enemy->getType(), "Squire") == 0 && enemy->GetIsAlive())
				{
					int distance = (int)sqrt(pow(enemy->GetPosition().row - n->GetPosition().row, 2) + pow(enemy->GetPosition().col - n->GetPosition().col, 2));
					if (distance < minDistance)
					{
						minDistance = distance;
						nearestSquireEnemy = enemy;
					}
				}
			}
		}
	}
	return nearestSquireEnemy;
}

Position Team::GetSquireEnemyPositionInRoom(int roomIndex, int teamNum)
{
	Position squireEnemyPos = { -1, -1 }; // Default value if no enemy is found
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
		NPC* target = findNearestSquireEnemy(n);
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

