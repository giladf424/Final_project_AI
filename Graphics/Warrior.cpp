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
        this->GetRoomIndex();
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

}

void Warrior::throwGrenade(Position enemyPos)
{
}

