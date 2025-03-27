#include "Warrior.h"

Warrior::Warrior(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
	pCurrentState = new PatrolState();
}


void Warrior::moveToEnemy(Position enemyPos)
{
    bool isEnemyInRange = false;
    if (isMoving)
    {
        DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
        isAstar = true;
        Position nextStep = RunAStar(enemyPos, dupMaze, dupSecurityMap);
        isAstar = false;
        move(nextStep);
        this->getRoomIndex();
        /*if (path.size() > 1) {
            Position nextPos = { path[1]->getRow(), path[1]->getCol() };
            isEnemyInRange = abs(enemyPos.row - pos.row) + abs(enemyPos.col - pos.col) <= 5;
        }*/
		if (isEnemyInRange)
			pCurrentState->Transition(this);
    }
}

void Warrior::attackEnemy(Position enemyPos)
{
	Grenade* grenade = new Grenade(pos.row, pos.col);
    if (grenade->IsEnemyFoundByExplosion(maze, enemyPos))
    {
        //enemy is found
		int chooseRandomWeapon = rand() % 10;
		if (chooseRandomWeapon < 7)
			fireBullet(enemyPos);
		else
			throwGrenade(enemyPos);

    }
}

void Warrior::fireBullet(Position enemyPos)
{
	double angle = atan2(enemyPos.row - pos.row, enemyPos.col - pos.col);
	Bullet bullet = Bullet((double)pos.row+0.5, (double)pos.col+0.5, angle);
	bullet.setIsMoving(true);
	//while (bullet())
	//{
		bullet.move(maze);
	//}
	int BulletHitType = bullet.moveToEnemyOrWall(enemyPos);
	if (BulletHitType == 1)
	{
		//enemy is hit
		NPC* enemy = Team::GetTeammate(enemyPos);
		//bulletDamage = enemy->GetHp() - BULLET_DAMAGE;
		//enemy->SetHp(bulletDamage);
		bullet.setIsMoving(false);
	}
	else if (BulletHitType == 2)
	{
		//wall is hit
		bullet.setIsMoving(false);
	}
}

void Warrior::throwGrenade(Position enemyPos)
{

}

