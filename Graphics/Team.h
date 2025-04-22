#pragma once
#include "definitions.h"
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
	queue<NPC*> woundedWarriors;
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

	static NPC* findNearestEnemy(NPC* n);
	static Position findNearestTeammate(NPC* n);
	static vector<Position> GetEnemiesPositionsInRoom(int roomIndex, int teamNum, bool onlyWarriors);
	static NPC* GetNPCByPosition(Position p, int teamNum, int id, int type);
	static NPC* findLowestHPEnemy(int teamNum);
	static NPC* findNearestSquireEnemyOrTeammate(NPC* n, bool isEnemy);
	static Position GetSquireEnemyPositionInRoom(int roomIndex, int teamNum);
	static NPC* findTargetEnemy(NPC* n, bool aggresive);
	void removeTeammate(NPC* dead);
	void removeWoundedWarrior(NPC* dead);
	static double findDistance(Position p1, Position p2);
	static bool isAnyBodyInMyPosition(Position p, int teamNum, int id);
	static vector<RoomDetails> getConnectedRooms(int roomIndex, int teamNum);
	static Position findRoomCenter(int roomIndex);
	static bool isTeamsSizesEqualTwo();
	static vector<Position> findAllWarriorsEnemies(int teamNum);
	static vector<Position> findAllPlayesPositions(int teamNum, int id);
	static bool isSafePosition(Position p, int teamNum, int id);
	static bool HandleGameState();
};

