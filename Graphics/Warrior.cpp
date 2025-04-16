#include "Warrior.h"
#include "Grenade.h"
//#include "PatrolState.h"

Warrior::Warrior(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
	isReloading = false;
	isWounded = false;
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
		bool isComing = (Team::findDistance(pos, target->GetPosition()) < Team::findDistance(pos, target->GetPrevPosition()));
		vector<Position> enemyPos;
        DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		Position nextStep = {-1, -1};
		int radius = 0;
		// if there are enemies in the same room
		if (enemiesPositions.size() > 0)
		{
			// if the target is in the same room
			if (target->getRoomIndex() == this->getRoomIndex())
			{
				// if the target is in the same room and in hit range (switch to combat state)
				if (IsEnemyInHitRange(GetPosition(), target->GetPosition()))
				{
					this->SetPrevPosition(this->GetPosition());
					pCurrentState->Transition(this);
					return;
				}// if there are enemies in hit range and the warrior is not aggressive (switch to target in hit range and switch to combat state)
				else if (enemiesInHitRange.size() > 0 && !aggressive)
				{
					Position enemyPos = enemiesInHitRange[0];
					this->SetTarget(Team::GetNPCByPosition(enemyPos, this->GetTeamID().team, this->GetTeamID().place));
					this->SetPrevPosition(this->GetPosition());
					pCurrentState->Transition(this);
					return;
				}// if there are enemies in hit range and the warrior is aggressive or there are no enemies in hit range
				else
				{// find the best position to move to
					// need to change the radius to be the number of steps from the target
					radius = 3;
					enemyPos.push_back(target->GetPosition());
					Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
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
			}// if the target is in connected corridor and there are enemies in hit range
			else if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && enemiesInHitRange.size() > 0 && (isComing || !aggressive))
			{// switch to combat state ( if isn't aggressive switch target to the first enemy in hit range)
				if (!aggressive)
				{
					Position enemyPos = enemiesInHitRange[0];
					this->SetTarget(Team::GetNPCByPosition(enemyPos, this->GetTeamID().team, this->GetTeamID().place));
				}
				this->SetPrevPosition(this->GetPosition());
				pCurrentState->Transition(this);
				return;
			}// if the target is in connected corridor and there are no enemies in hit range
			else if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && enemiesInHitRange.size() == 0 && isComing)
			{
				// handle the case on the next if statement
			}
			// if the target is in another room
			else
			{
				// if the warrior is not aggressive
				if (!this->aggressive)
				{
					// if there are enemies in hit range (switch to target in hit range and switch to combat state)
					if (enemiesInHitRange.size() > 0)
					{
						Position enemyPos = enemiesInHitRange[0];
						this->SetTarget(Team::GetNPCByPosition(enemyPos, this->GetTeamID().team, this->GetTeamID().place));
						this->SetPrevPosition(this->GetPosition());
						pCurrentState->Transition(this);
						return;
					}// otherwise switch to the closest enemy in the same room and find the best position to move to
					else if (enemiesPositions.size() > 0)
					{
						this->SetTarget(Team::GetNPCByPosition(enemiesPositions[0], this->GetTeamID().team, this->GetTeamID().place));
						radius = 3;
						enemyPos.push_back(target->GetPosition());
						Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
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
		Position myEntrance = this->getEntranceToCorridor(this->corridorIndex, this->prevRoomIndex, true);
		Position targetEntrance = this->getEntranceToCorridor(this->target->GetCorridorIndex(), this->target->getPrevRoomIndex(), true);
		bool isMyRoomCloser = (Team::findDistance(pos, myEntrance) < Team::findDistance(this->target->GetPosition(), targetEntrance));
		// if the target is in connected corridor and coming
		if (this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex()) && isComing)
		{ // find the best position to wait for the target
			Position entrance = this->getEntranceToCorridor(target->GetCorridorIndex(), this->getRoomIndex(), true);
			enemyPos.push_back(entrance);
			vector<Position> entrances = this->GetAllEntrancesToMyRoom(this->getRoomIndex(), true);
			UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
			Position temp = target->RunAStar(entrance, dupMaze, dupSecurityMap, &radius);
			std::cout << "Radius: ( " << radius << " ) patrol last\n";
			DuplicateMaze(maze, dupMaze);
			Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
			DuplicateMaze(maze, dupMaze);
			if (bestPos.row != -1 && bestPos.col != -1)
				nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
			else
			{
				std::cout << "No bestPos found from BFSRadius (patrol, enemy in con corridor)\n";
				nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
			}
		}// if you and the target are in the same corridor and the target is coming and you are closer to your room than the the target to his toom
		else if (getRoomIndex() == -1 && this->corridorIndex == this->target->GetCorridorIndex() && isComing && isMyRoomCloser)
		{// go back to the room you came from and find the best position to wait for the target
			enemyPos.push_back(myEntrance);
			vector<Position> entrances = this->GetAllEntrancesToMyRoom(this->prevRoomIndex, true);
			enemiesPositions = Team::GetEnemiesPositionsInRoom(this->prevRoomIndex, this->GetTeamID().team, false);
			UpdateSecurityMap(enemiesPositions, dupMaze, dupSecurityMap);
			UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
			Position temp = target->RunAStar(myEntrance, dupMaze, dupSecurityMap, &radius);
			DuplicateMaze(maze, dupMaze);
			Team::blockPathSearchDirection(myEntrance, target->GetPosition(), dupMaze);
			Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
			DuplicateMaze(maze, dupMaze);
			if (bestPos.row != -1 && bestPos.col != -1)
				nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
			else
			{
				std::cout << "No bestPos found from BFSRadius (patrol, me and my target in corridor)\n";
				nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
			}
		}// if the target is in another room and there are no enemies in your room/hit range
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
	vector<Position> enemyPos;
	DuplicateMaze(maze, dupMaze);
	DuplicateSecurityMap(security_map, dupSecurityMap);

	if (isReloading)
	{
		reloadTime--;
		if (reloadTime <= 0) {
			isReloading = false;
		}
	}
	bool isTargetInHitRange = false;
	if(target != nullptr)
	{
		for (Position p : enemiesInHitRange)
		{
			if (p.row == target->GetPosition().row && p.col == target->GetPosition().col)
				isTargetInHitRange = true;

		}
	}
	if (target == nullptr || (!aggressive && !isTargetInHitRange) || enemiesPositions.empty())
	{
		if(!updateTarget(enemiesPositions, enemiesInHitRange))
		{
			this->SetPrevPosition(this->GetPosition());
			return;
		}
	}
	bool isTargetInSameRoom = target->getRoomIndex() == this->getRoomIndex();
	bool isTargetInCorridor = this->IsEnemyInCorridorConnectedToMyRoom(target->GetCorridorIndex());
	bool isComing = (Team::findDistance(pos, target->GetPosition()) <= Team::findDistance(pos, target->GetPrevPosition()));
	bool isAnybodyInMyPos = (Team::isAnyBodyInMyPosition(this->pos, this->id.team, this->id.place) ? (Team::GetNPCByPosition(this->pos, this->id.team, this->id.place))->isStillInSamePos() : false);
	int radius = 0;
	Position nextStep = { -1, -1 };
	// if there are enemies in hit range and the warrior is not in a corridor
	if (!enemiesInHitRange.empty() && this->getRoomIndex() != -1)
	{
		// if the target is not in hit range
		if (!isTargetInHitRange || isAnybodyInMyPos)
		{
			// if the target is in the same room
			if (isTargetInSameRoom)
			{
				// find the best position to move to
				radius = 3;
				enemyPos.push_back(target->GetPosition());
				Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
				DuplicateMaze(maze, dupMaze);
				if (bestPos.row != -1 && bestPos.col != -1)
					nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
				else
				{
					std::cout << "No bestPos found from BFSRadius (combat, enemy in the same room)\n";
					nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
				}
			}// if the target is in connected corridor and coming
			else if (isTargetInCorridor && isComing)
			{
				// find the best position to wait for the target before he comes to the room
				Position entrance = this->getEntranceToCorridor(target->GetCorridorIndex(), this->getRoomIndex(), true);
				enemyPos.push_back(entrance);
				vector<Position> entrances;
				entrances.push_back(entrance);
				UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
				Position temp = target->RunAStar(entrance, dupMaze, dupSecurityMap, &radius);
				Position bestPos = this->BFSRadius(GetPosition(), enemyPos, radius, dupMaze, dupSecurityMap);
				DuplicateMaze(maze, dupMaze);
				if (bestPos.row != -1 && bestPos.col != -1)
					nextStep = this->RunAStar(bestPos, dupMaze, dupSecurityMap, &radius);
				else
				{
					std::cout << "No bestPos found from BFSRadius (Combat, corridor, coming)\n";
					nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
				}
			} // / if the target is in another room or in connected corridor and not coming
			else if (!isTargetInSameRoom && !isTargetInCorridor || isTargetInCorridor && !isComing)
			{
				nextStep = RunAStar(target->GetPosition(), dupMaze, dupSecurityMap, &radius);
			}
		}
		/*if ((!isValidPos(nextStep) || isSamePosAsMyPos(nextStep)) && Team::isAnyBodyInMyPosition(GetPosition(),this->id.team,this->id.place) && prevStep == NPC_)
		{
			std::cout << "Error: nextStep is not valid (combat)\n";
			this->GetState()->Transition(this);
			return;
		}*/
		if (!isReloading)
		{
			if (enemiesInHitRange.size() > 1)
				throwGrenade();
			else if(isTargetInHitRange)
				fireBullet(target->GetPosition());
			else
				fireBullet(enemiesInHitRange[0]);
		}
		// if the next step taking the warrior out of the room (into the corridor)
		if (isValidPos(nextStep) && GetRoomIndex(nextStep) == -1)
		{
			// switch to patrol state
			std::cout << "Error: nextStep is not in the same room (combat)\n";
			this->SetPrevPosition(this->GetPosition());
			this->GetState()->Transition(this);
			return;
		}
		move(nextStep);
	}
	else
	{
		this->SetPrevPosition(this->GetPosition());
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
			this->SetTarget(Team::GetNPCByPosition(enemiesInHitRange[0], this->GetTeamID().team, this->GetTeamID().place));
			return true;
		}
		else
		{
			this->SetTarget(Team::GetNPCByPosition(enemiesPositions[0], this->GetTeamID().team, this->GetTeamID().place));
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

void Warrior::addWoundedWarriorToQueue()
{
	if (!isWounded)
	{
		isWounded = true;
		Team::Teams[this->GetTeamID().team]->woundedWarriors.push(this);
	}
}

void Warrior::moveToSquire(Position squirePos)
{
	if (isMoving)
	{
		DuplicateMaze(maze, dupMaze);
		DuplicateSecurityMap(security_map, dupSecurityMap);
		int squireRoomIndex = GetRoomIndex(squirePos);
		vector<Position> entrances = this->GetAllEntrancesToMyRoom(this->getRoomIndex(), true);
		vector<Position> enemiesPos = Team::GetEnemiesPositionsInRoom(this->getRoomIndex(), this->GetTeamID().team, true);
		if (enemiesPos.size() > 0)
			UpdateSecurityMap(enemiesPos, dupMaze, dupSecurityMap);
		else
			UpdateSecurityMap(entrances, dupMaze, dupSecurityMap);
		int numSteps = 0;
		Position nextPos = RunAStar(squirePos, dupMaze, dupSecurityMap, &numSteps);
		if (nextPos.col == squirePos.col && nextPos.row == squirePos.row)
		{
			this->SetPrevPosition(this->GetPosition());
			return;
		}
		move(nextPos);
	}
}

bool Warrior::isWarriorCanReturnToFight()
{
	if (!GetAggressive() && bullets > AMMO_TH && hp > HP_TH && grenades > GRENADE_TH)
	{
		isWounded = false;
		return true;
	}
	else if(GetAggressive() && bullets > AMMO_TH_AGGRESSIVE && hp > HP_TH_AGGRESSIVE && grenades > GRENADE_TH)
	{
		isWounded = false;
		return true;
	}
	else
		return false;
	
}

