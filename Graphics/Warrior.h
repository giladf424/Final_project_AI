#pragma once
#include "definitions.h"
#include "PatrolState.h"
#include "NPC.h"
#include "Cell.h"
//#include "definitions.h"
#include <queue>
#include "CompareCells.h"
//#include "Bullet.h"
//#include "Grenade.h"

class Warrior : public NPC
{
private:
	int bullets, grenades;
	bool aggressive;
	NPC* target;
public:
	Warrior(Position startPos, TeamID teamID);
	void SetBullets(int b) { bullets = b; }
	int GetBullets() { return bullets; }
	void SetGrenades(int g) { grenades = g; }
	int GetGrenades() { return grenades; }
	bool GetAggressive() { return aggressive; }
	NPC* GetTarget() { return target; }
	void SetTarget(NPC* t) { target = t; }

	const char* getType() override { return "Warrior"; }

	// Enemy interaction methods 
	void moveToEnemy();
	void attackEnemy(Position enemyPos);
	void fireBullet(Position enemyPos);
	void throwGrenade(Position enemyPos);

};

// test