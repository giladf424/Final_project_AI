#include "Grenade.h"

vector<Grenade*> Grenade::grenades;
Grenade::Grenade(double r, double c)
{
	int i;
	row = r;
	col = c;
	double alpha = 2*PI/NUM_BULLETS; // bullet separation angle
	for (i = 0;i < NUM_BULLETS;i++)
	{
		bullets.push_back(new Bullet(col, row, i * alpha));
	}
	isExpending = false;
	id = { -1, -1 };
}

Grenade::Grenade(double r, double c, TeamID teamID)
{
	int i;
	row = r;
	col = c;
	double alpha = 2 * PI / NUM_BULLETS; // bullet separation angle
	for (i = 0; i < NUM_BULLETS; i++)
	{
		bullets.push_back(new Bullet(col, row, i * alpha, teamID));
	}
	isExpending = false;
	id = teamID;
}

Grenade::~Grenade()
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
	{
		Bullet* b = bullets.back();
		bullets.pop_back();
		delete b;
		b = nullptr;
	}
	
}

void Grenade::explode()
{
	int i;

	for (i = 0;i < NUM_BULLETS;i++)
	{
		bullets[i]->setIsMoving(true);
	}


}

void Grenade::expand(int maze[MSZ][MSZ])
{
	bool stillExpanding = false;
	int i;

	for (i = 0;i < NUM_BULLETS;i++)
	{
		bullets[i]->move(maze);
		stillExpanding = bullets.at(i)->getIsMoving() || stillExpanding;
	}
	isExpending = stillExpanding;
}

void Grenade::show()
{
	int i;

	for (i = 0;i < NUM_BULLETS;i++)
	{
		if (bullets[i]->getIsMoving())
			bullets[i]->show();
	}

}

void Grenade::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	int i;

	for (i = 0;i < NUM_BULLETS;i++)
	{
		bullets[i]->SimulateExplosion(maze,sm);
		//delete bullets[i];
	}


}

double Grenade::IsEnemyFoundByExplosion(int maze[MSZ][MSZ], Position enemyPos)
{
	int i;
	for (i = 0; i < NUM_BULLETS; i++)
	{
		if (bullets[i]->IsEnemyFoundByExplosion(maze, enemyPos))
			return bullets[i]->getAngle();
	}
	return -1;
}
