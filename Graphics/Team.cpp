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
	NPC* nearestEnemy = nullptr; // Default value if no enemy is found
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
	Position teammatePos = { -1, -1 }; // Default value if no teammate is found
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
	{
		//std::cout << "Error: Invalid room index\n";
		return enemiesPos;
	}
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

NPC* Team::GetNPCByPosition(Position p, int teamNum, int id)
{
	for (Team* t : Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (n->GetTeamID().team == teamNum && n->GetTeamID().place == id)
				continue;
			if (n->isSamePosAsMyPos(p))
				return n;
		}
	}
	std::cout << "Error: NPC not found (GetNPCByPosition)\n";
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
					int distance = (int)findDistance(enemy->GetPosition(), n->GetPosition());
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

double Team::findDistance(Position p1, Position p2)
{
	return sqrt(pow(p1.row - p2.row, 2) + pow(p1.col - p2.col, 2));
}

void Team::blockPathSearchDirection(Position p1, Position p2, int maze[MSZ][MSZ])
{
	int maxDistance = INT_MAX;
	int distance = 0;
	Position toBlock = { -1, -1 };
	if (maze[p1.row + 1][p1.col] != WALL)
	{
		distance = (int)findDistance({ p1.row + 1, p1.col }, p2);
		if (distance < maxDistance)
		{
			maxDistance = distance;
			toBlock = { p1.row + 1, p1.col };
		}
	}
	if (maze[p1.row - 1][p1.col] != WALL)
	{
		distance = (int)findDistance({ p1.row - 1, p1.col }, p2);
		if (distance < maxDistance)
		{
			maxDistance = distance;
			toBlock = { p1.row - 1, p1.col };
		}
	}
	if (maze[p1.row][p1.col + 1] != WALL)
	{
		distance = (int)findDistance({ p1.row, p1.col + 1 }, p2);
		if (distance < maxDistance)
		{
			maxDistance = distance;
			toBlock = { p1.row, p1.col + 1 };
		}
	}
	if (maze[p1.row][p1.col - 1] != WALL)
	{
		distance = (int)findDistance({ p1.row, p1.col - 1 }, p2);
		if (distance < maxDistance)
		{
			maxDistance = distance;
			toBlock = { p1.row, p1.col - 1 };
		}
	}
	if (toBlock.row != -1 && toBlock.col != -1)
	{
		std::cout << "Blocking path from " << p1.row << ", " << p1.col << " to " << toBlock.row << ", " << toBlock.col << "\n";
		maze[toBlock.row][toBlock.col] = BLACK;
	}
		
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
	{
		std::cout << "Error: Invalid room index\n";
		return vector<RoomDetails>();
	}
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
	{
		std::cout << "Error: Invalid room index (Team, findRoomCenter)\n";
		return Position();
	}
	RoomScope rs = roomScopes[roomIndex];
	int centerRow = (rs.startRow + rs.endRow) / 2;
	int centerCol = (rs.startCol + rs.endCol) / 2;
	Position center = { centerRow, centerCol };
	return center;
}
