#pragma once
#include "NPC.h"
#include "IdleState.h"
#include <queue>
#include "CompareCells.h"
#include "Cell.h"
#include "definitions.h"

class Squire : public NPC
{
private:
	int bulletsPack, grenadesPack, healthPack;
public:
	Squire(Position startPos, TeamID teamID);
	void SetBulletsPack(int b) { bulletsPack = b; }
	int GetBulletsPack() { return bulletsPack; }
	void SetGrenadesPack(int g) { grenadesPack = g; }
	int GetGrenadesPack() { return grenadesPack; }
	void SetHealthPack(int h) { healthPack = h; }
	int GetHealthPack() { return healthPack; }
	const char* getType() override { return "Squire"; }

	// Movement
	void MoveToTeamMate(Position teammatePos);
	//void move(int maze[MSZ][MSZ]) override;

};

