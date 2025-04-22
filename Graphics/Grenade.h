#pragma once
#include "Bullet.h"

const int NUM_BULLETS = 20;
class Grenade
{
private:
	double row, col;
	vector<Bullet*> bullets;
	bool isExpending;
	TeamID id;
public:
	static vector<Grenade*> grenades;
	Grenade(double r, double c);
	Grenade(double r, double c, TeamID teamID);
	~Grenade();
	void explode();
	void expand(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	double IsEnemyFoundByExplosion(int maze[MSZ][MSZ], Position enemyPos);
	void setIsExpending(bool value) { isExpending = value; }
	bool getIsExpending() { return isExpending; }
};

