#pragma once
#include "NPC.h"
#include "definitions.h"
#include "Warrior.h"
#include "Squire.h"
#include <vector>

class Team
{
private:
	TeamID teamID;
	int teamSize;
	vector<NPC*> teammates{};
	vector<array<double, 3>> teamColor;
public:
	static vector<Team*> Teams;
	Team(vector<array<double, 3>> tc);
	TeamID GetTeamID() { return teamID; }
	int GetTeamSize() { return teamSize; }
	vector<NPC*>& GetTeammates() { return teammates; }
	void SetTeammates(vector<NPC*> t) { teammates = t; }
	TeamID generateTeamID();
	TeamID generateTeamIDForTeammate();
	void addTeammate(Position start_pos, bool isWarrior);
	array<double, 3> getWarriorColor() { return teamColor.at(0); };
	array<double, 3> getSquireColor() { return teamColor.at(1); };

	// Find nearest logic
	static Position findNearestEnemy(NPC* n); 
	static Position findNearestEnemyInRoom(NPC* n, int roomIndex);
	static Position findNearestTeammate(NPC* n);
	static vector<Position> GetEnemiesPositionsInRoom(int roomIndex, int teamNum, bool onlyWarriors);
};

