#include "Bullet.h"
#include <math.h>
#include "glut.h"

// angle is in radians
Bullet::Bullet(double xx, double yy, double angle)
{
	x = xx;
	y = yy;
	dir_angle = angle;
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.3;
	isMoving = false;
}

void Bullet::move(int maze[MSZ][MSZ])
{
	if (isMoving)
	{
		x += speed * dirX;
		y += speed * dirY;
		if (maze[(int)y][(int)x] == WALL)
			isMoving = false;
	}
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
