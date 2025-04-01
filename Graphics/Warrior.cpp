#include "Warrior.h"

Warrior::Warrior(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
	pCurrentState = new PatrolState();
}


void Warrior::moveToEnemy(NPC* target)
{
    if (isMoving)
    {
		if (Team::GetEnemiesPositionsInRoom(getRoomIndex(),GetTeamID().team, true).size() > 0)
		{
			pCurrentState->Transition(this);
			return;
		}
        DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		Position nextStep = {-1, -1};
		int radius = 0;
		if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()))
		{
			Position entrance = this->getEntranceToCorridor(target->GetCorridorIndex());
			vector<Position> entrances = this->GetAllEntrancesToMyRoom();
			UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
			Position temp = target->RunAStar(entrance, dupMaze, dupSecurityMap, &radius);
			Position bestPos = BFSRadius(GetPosition(), entrance, radius, dupMaze, dupSecurityMap);
			if (bestPos.row != -1 && bestPos.col != -1)
				nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
			else
				std::cout << "No bestPos found from BFSRadius\n";
		}
		else
		{
			nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);

		}
        move(nextStep);
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
	//int BulletHitType = bullet.moveToEnemyOrWall(enemyPos);
	int BulletHitType = 0;
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

