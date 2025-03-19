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
