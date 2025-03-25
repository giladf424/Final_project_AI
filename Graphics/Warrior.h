#pragma once
#include "definitions.h"
#include "PatrolState.h"
#include "NPC.h"
#include "Cell.h"
#include "definitions.h"
#include <queue>
#include "CompareCells.h"

class Warrior : public NPC
{
private:
	int bullets, grenades;
	bool aggressive;
public:
	Warrior(Position startPos, TeamID teamID);
	void SetBullets(int b) { bullets = b; }
	int GetBullets() { return bullets; }
	void SetGrenades(int g) { grenades = g; }
	int GetGrenades() { return grenades; }
	bool GetAggressive() { return aggressive; }

	const char* getType() override { return "Warrior"; }

	// Enemy interaction methods 
	void moveToEnemy(Position enemyPos);
	void shootEnemy(Position enemyPos);
};

// test