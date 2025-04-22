#pragma once
#include "definitions.h"
#include "Team.h"
class Bullet
{
private: 
	double x, y;
	double dir_angle;
	double dirX, dirY;
	double speed;
	bool isMoving;
	bool isVisualization;
	TeamID teamID;
public:
	static vector<Bullet*> bullets;
	Bullet(double xx, double yy, double angle);
	Bullet(double xx, double yy, double angle, TeamID id);
	~Bullet() = default;
	Position move(int maze[MSZ][MSZ]);
	void show();
	void setIsMoving(bool value) { isMoving = value; }
	bool getIsMoving() { return isMoving; }
	double getAngle() { return dir_angle; }
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	bool IsEnemyFoundByExplosion(int maze[MSZ][MSZ], Position enemyPos);
	Position getPosition() { return { (int)x, (int)y }; }
	bool moveToEnemyOrWall(Position enemyPos, int maze[MSZ][MSZ]);
	NPC* findNPCByPosition(Position pos);
	bool IsEnemyTeam(NPC* n);

};

