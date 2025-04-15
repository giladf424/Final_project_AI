#pragma once
//#include "definitions.h"
#include "PatrolState.h"
#include "NPC.h"
//#include "Cell.h"
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
	bool isReloading;
	bool isWounded;
	int reloadTime;
	NPC* target;
public:
	Warrior(Position startPos, TeamID teamID);
	void SetBullets(int b) { bullets = b; }
	//int GetBullets() { return bullets; }
	void SetGrenades(int g) { grenades = g; }
	//int GetGrenades() { return grenades; }
	bool GetAggressive() { return aggressive; }
	NPC* GetTarget() { return target; }
	void SetTarget(NPC* t) { target = t; }
	bool getIsReloading() { return isReloading; }
	void setIsReloading(bool r) { isReloading = r; }
	bool getIsWounded() { return isWounded; }
	void setIsWounded(bool w) { isWounded = w; }

	const char* getType() override { return "Warrior"; }
	int getAmmo() override { return bullets; }
	int getGrenades() override { return grenades; }

	// Enemy interaction methods 
	void moveToEnemy();
	void attackEnemy();
	void fireBullet(Position enemyPos);
	void throwGrenade();
	bool updateTarget(vector<Position> enemiesPositions, vector<Position> enemiesInHitRange);
	
	// Squire interaction methods
	void addWoundedWarriorToQueue();
	void moveToSquire(Position squirePos);
	void isWarriorCanReturnToFight();

};

// test