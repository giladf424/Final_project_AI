#include "Bullet.h"
#include <math.h>
#include "glut.h"

vector<Bullet*> Bullet::bullets;
// angle is in radians
Bullet::Bullet(double xx, double yy, double angle)
{
	x = xx;
	y = yy;
	dir_angle = angle;
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.3;
	teamID = { -1, -1 };
	isMoving = false;
	isVisualization = true;
}

Bullet::Bullet(double xx, double yy, double angle, TeamID id)
{
	x = xx;
	y = yy;
	dir_angle = angle;
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.9;
	teamID = id;
	isMoving = false;
	isVisualization = false;
}

Position Bullet::move(int maze[MSZ][MSZ])
{
	if (isMoving)
	{
		x += speed * dirX;
		y += speed * dirY;
		if (maze[(int)y][(int)x] == WALL)
			isMoving = false;

		if (maze[(int)y][(int)x] == NPC_ && !isVisualization)
		{
			Position enemyPos = { (int)y, (int)x };
			NPC* n = findNPCByPosition(enemyPos);
			if (n != nullptr && IsEnemyTeam(n))
			{
				n->hitByBullet();
				isMoving = false;
			}
			return enemyPos;
		}
	}
	return { -1, -1 };
}

void Bullet::show()
{
	glColor3d(1, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2d(x - 0.5, y);
	glVertex2d(x , y+ 0.5);
	glVertex2d(x + 0.5, y);
	glVertex2d(x , y- 0.5);
	glEnd();
}

void Bullet::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	isMoving = true;
	while (isMoving)
	{
		sm[(int)y][(int)x] += SECURITY_FACTOR;
		move(maze);
	}
}

bool Bullet::IsEnemyFoundByExplosion(int maze[MSZ][MSZ], Position enemyPos)
{
	isMoving = true;
	while (isMoving)
	{
		if (moveToEnemyOrWall(enemyPos, maze))
			return true;
	}
	return false;
}

bool Bullet::moveToEnemyOrWall(Position enemyPos, int maze[MSZ][MSZ])
{
	if (isMoving)
	{
		x += speed * dirX;
		y += speed * dirY;
		if ((int)x == enemyPos.col && (int)y == enemyPos.row)
		{
			isMoving = false;
			return true;
		}
		if (maze[(int)y][(int)x] == WALL)
		{
			isMoving = false;
			return false;
		}
	}
	return false;
}

NPC* Bullet::findNPCByPosition(Position pos)
{
	for (Team* t : Team::Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			if (n->GetPosition().row == pos.row && n->GetPosition().col == pos.col)
			{
				return n;
			}
		}
	}
	return nullptr;
}

bool Bullet::IsEnemyTeam(NPC* n)
{
	return n->GetTeamID().team != teamID.team;
}
