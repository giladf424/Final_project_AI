#pragma once
#include "definitions.h"
class Bullet
{
private: 
	double x, y;
	double dir_angle;
	double dirX, dirY;
	double speed;
	bool isMoving;
public:
	Bullet(double xx, double yy, double angle);
	void move(int maze[MSZ][MSZ]);
	void show();
	void setIsMoving(bool value) { isMoving = value; }
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	bool IsEnemyFoundByExplosion(int maze[MSZ][MSZ], Position enemyPos);

};

