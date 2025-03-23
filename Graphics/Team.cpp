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

Position Team::findNearestEnemy(NPC* n)
{
	int minDistance = INT_MAX;
	Position enemyPos = { -1, -1 }; // Default value if no enemy is found
	for (Team* t : Teams) {
		if (t->GetTeamID().team != n->GetTeamID().team) {
			for (NPC* enemy : t->GetTeammates()) {
				if (enemy->GetIsAlive()) {
					int distance = abs(enemy->GetPosition().row - n->GetPosition().row) + abs(enemy->GetPosition().col - n->GetPosition().col); // Manhattan distance
					if (distance < minDistance) {
						minDistance = distance;
						enemyPos = enemy->GetPosition();
					}
				}
			}
		}
	}
	return enemyPos;
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
