#include "Warrior.h"
#include "Grenade.h"

Warrior::Warrior(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
	isReloading = false;
	reloadTime = 0;
	pCurrentState = new PatrolState();
	target = nullptr;
}


void Warrior::moveToEnemy()
{
    if (isMoving)
    {
		vector<Position> enemiesPositions = Team::GetEnemiesPositionsInRoom(getRoomIndex(), GetTeamID().team, false);
		vector<Position> enemiesInHitRange = GetEnemiesInHitRange(GetPosition(), enemiesPositions);
        DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		Position nextStep = {-1, -1};
		int radius = 0;
		bool isComing = (Team::findDistance(pos, target->GetPosition()) < Team::findDistance(pos, target->GetPrevPosition()));
		if (enemiesPositions.size() > 0)
		{
			if (target->getRoomIndex() == this->getRoomIndex())
			{
				if (IsEnemyInHitRange(GetPosition(), target->GetPosition()))
				{
					pCurrentState->Transition(this);
					return;
				}
				else if (enemiesInHitRange.size() > 0 && !aggressive)
				{
					Position enemyPos = enemiesInHitRange[0];
					this->SetTarget(Team::GetNPCByPosition(enemyPos));
					pCurrentState->Transition(this);
					return;
				}
				else
				{
					radius = 3;
					Position bestPos = BFSRadius(GetPosition(), target->GetPosition(), radius, dupMaze, dupSecurityMap);
					DuplicateMaze(maze, dupMaze);
					if (bestPos.row != -1 && bestPos.col != -1)
						nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
					else
					{
						std::cout << "No bestPos found from BFSRadius (patrol, enemy in the same room)\n";
						nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
					}
					move(nextStep);
					return;
				}
			}
			else if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && enemiesInHitRange.size() > 0)
			{
				if (!aggressive)
				{
					Position enemyPos = enemiesInHitRange[0];
					this->SetTarget(Team::GetNPCByPosition(enemyPos));
				}
				pCurrentState->Transition(this);
				return;
			}
			else if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && enemiesInHitRange.size() == 0)
			{
				
			}
			else
			{
				if (!this->aggressive)
				{
					if (enemiesInHitRange.size() > 0)
					{
						Position enemyPos = enemiesInHitRange[0];
						this->SetTarget(Team::GetNPCByPosition(enemyPos));
						pCurrentState->Transition(this);
						return;
					}
					else
					{
						this->SetTarget(Team::findNearestEnemy(this));
						radius = 3;
						Position bestPos = BFSRadius(GetPosition(), target->GetPosition(), radius, dupMaze, dupSecurityMap);
						DuplicateMaze(maze, dupMaze);
						if (bestPos.row != -1 && bestPos.col != -1)
							nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
						else
						{
							std::cout << "No bestPos found from BFSRadius (patrol, enemy in other room)\n";
							nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
						}
						move(nextStep);
						return;
					}
				}
			}
		}
		if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && isComing)
		{
			Position entrance = this->getEntranceToCorridor(target->GetCorridorIndex());
			vector<Position> entrances = this->GetAllEntrancesToMyRoom();
			UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
			Position temp = target->RunAStar(entrance, dupMaze, dupSecurityMap, &radius);
			Position bestPos = BFSRadius(GetPosition(), entrance, radius, dupMaze, dupSecurityMap);
			DuplicateMaze(maze, dupMaze);
			if (bestPos.row != -1 && bestPos.col != -1)
				nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
			else
			{
				std::cout << "No bestPos found from BFSRadius (patrol, enemy in con corridor)\n";
				nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
			}
		}
		else
		{
			nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);

		}
        move(nextStep);
    }
}

void Warrior::attackEnemy()
{
	vector<Position> enemiesPositions = Team::GetEnemiesPositionsInRoom(getRoomIndex(), GetTeamID().team, false);
	vector<Position> enemiesInHitRange = GetEnemiesInHitRange(GetPosition(), enemiesPositions);
	DuplicateMaze(maze, dupMaze);
	DuplicateSecurityMap(security_map, dupSecurityMap);

	if (isReloading)
	{
		reloadTime--;
		if (reloadTime <= 0) {
			isReloading = false;
		}
	}
	
	if (target == nullptr || (!aggressive && !IsEnemyInHitRange(pos, target->GetPosition())) || enemiesPositions.empty())
	{
		if(!updateTarget(enemiesPositions, enemiesInHitRange))
			return;
	}
	bool isTargetInHitRange = IsEnemyInHitRange(pos, target->GetPosition());
	bool isTargetInSameRoom = target->getRoomIndex() == this->getRoomIndex();
	bool isTargetInCorridor = this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex());
	bool isComing = (Team::findDistance(pos, target->GetPosition()) < Team::findDistance(pos, target->GetPrevPosition()));
	int radius = 0;
	Position nextStep = { -1, -1 };
	if (!enemiesInHitRange.empty() && this->getRoomIndex() != -1)
	{
		if (!isTargetInHitRange)
		{
			if (isTargetInSameRoom)
			{
				radius = 3;
				Position bestPos = BFSRadius(GetPosition(), target->GetPosition(), radius, dupMaze, dupSecurityMap);
				DuplicateMaze(maze, dupMaze);
				if (bestPos.row != -1 && bestPos.col != -1)
					nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
				else
				{
					std::cout << "No bestPos found from BFSRadius (combat, enemy in the same room)\n";
					nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
				}
			}
			else if (isTargetInCorridor && isComing)
			{
				Position entrance = this->getEntranceToCorridor(target->GetCorridorIndex());
				vector<Position> entrances;
				entrances.push_back(entrance);
				UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
				Position temp = target->RunAStar(entrance, dupMaze, dupSecurityMap, &radius);
				Position bestPos = BFSRadius(GetPosition(), entrance, radius, dupMaze, dupSecurityMap);
				DuplicateMaze(maze, dupMaze);
				if (bestPos.row != -1 && bestPos.col != -1)
					nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
				else
				{
					std::cout << "No bestPos found from BFSRadius (Combat, corridor, coming)\n";
					nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
				}
			}
			else if (!isTargetInSameRoom && !isTargetInCorridor || isTargetInCorridor && !isComing)
			{
				nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
			}
		}
		if (!isReloading)
		{
			if (enemiesInHitRange.size() > 1)
				throwGrenade();
			else if(isTargetInHitRange)
				fireBullet(target->GetPosition());
			else
				fireBullet(enemiesInHitRange[0]);
		}

		if(nextStep.row != -1 && nextStep.col != -1)
			move(nextStep);
		else
		{
			std::cout << "No nextStep found (combat)\n";
		}
	}
	else
	{
		this->GetState()->Transition(this);
		return;
	}
	if (!aggressive && hp < HP_TH || aggressive && hp < HP_TH_AGGRESSIVE)
	{
		this->GetState()->Transition(this);
		return;
	}
	if ((!aggressive && bullets < AMMO_TH) || (aggressive && bullets < AMMO_TH_AGGRESSIVE))
	{
		this->GetState()->Transition(this);
		return;
	}
	if (grenades < GRENADE_TH)
	{
		this->GetState()->Transition(this);
		return;
	}
}

void Warrior::fireBullet(Position enemyPos)
{
	double angle = atan2(enemyPos.row - pos.row, enemyPos.col - pos.col);
	Bullet* bullet = new Bullet((double)pos.col+0.5, (double)pos.row+0.5, angle, GetTeamID());
	Bullet::bullets.push_back(bullet);
	bullet->setIsMoving(true);
	isReloading = true;
	reloadTime = RELOAD_TIME;
	bullets--;
}

void Warrior::throwGrenade()
{
	Grenade* grenade = new Grenade(pos.row, pos.col, GetTeamID());
	grenade->setIsExpending(true);
	grenade->explode();
	Grenade::grenades.push_back(grenade);
	isReloading = true;
	reloadTime = RELOAD_TIME;
	grenades--;
}

bool Warrior::updateTarget(vector<Position> enemiesPositions, vector<Position> enemiesInHitRange)
{
	if (enemiesPositions.size() > 0)
	{
		if (enemiesInHitRange.size() > 0)
		{
			this->SetTarget(Team::GetNPCByPosition(enemiesInHitRange[0]));
			return true;
		}
		else
		{
			this->SetTarget(Team::GetNPCByPosition(enemiesPositions[0]));
			this->GetState()->Transition(this);
			return false;
		}
	}
	else
	{
		this->GetState()->Transition(this);
	}
	return false;
}

