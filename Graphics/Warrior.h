#pragma once
#include "PatrolState.h"
#include "NPC.h"
#include <queue>
#include "CompareCells.h"

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
	void SetGrenades(int g) { grenades = g; }
	bool GetAggressive() { return aggressive; }
	NPC* GetTarget() { return target; }
	void SetTarget(NPC* t) { target = t; }
	bool getIsReloading() { return isReloading; }
	void setIsReloading(bool r) { isReloading = r; }
	bool getIsWounded() { return isWounded; }
	void setIsWounded(bool w) { isWounded = w; }
	int getReloadTime() { return reloadTime; }
	void setReloadTime(int r) { reloadTime = r; }

	const char* getType() override { return "Warrior"; }
	int getAmmo() override { return bullets; }
	int getGrenades() override { return grenades; }

	// Enemy interaction methods 
	void moveToEnemy();
	void attackEnemy();
	void fireBullet(double angle);
	void throwGrenade();
	bool updateTarget(vector<Position>& enemiesPositions, vector<HitDetails>& enemiesInHitRange);

	// Squire interaction methods
	void addWoundedWarriorToQueue();
	void moveToSquire(Position squirePos);
	bool isWarriorCanReturnToFight();

};