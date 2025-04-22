#include "CombatState.h"
#include "RequestResourcesState.h"
#include "Warrior.h"
#include <iostream>

void CombatState::OnEnter(NPC* p)
{
	Warrior* w = (Warrior*)p;
	w->SetIsMoving(true);
	w->attackEnemy();
	w->SetIsMoving(false);
}

void CombatState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	Warrior* w = (Warrior*)p;
	State* currentState = p->GetState();
	bool isAggressive = w->GetAggressive();
	int numBullets = w->getAmmo();
	NPC* s = Team::findNearestSquireEnemyOrTeammate(p, false);
	bool isAlive = (s != nullptr && s->GetIsAlive());
	if (isAlive && ((!isAggressive && w->GetHp() < HP_TH) || isAggressive && w->GetHp() < HP_TH_AGGRESSIVE))/* low health or low ammunition */
	{
		p->SetState(new RequestResourcesState());
	}
	else if (isAlive && ((!isAggressive && numBullets < AMMO_TH) || (isAggressive && numBullets < AMMO_TH_AGGRESSIVE) || w->getGrenades() < GRENADE_TH))
	{
		p->SetState(new RequestResourcesState());
	}
	else
	{
		p->SetState(new PatrolState());
	}
	delete currentState;
	currentState = nullptr;
}

void CombatState::OnExit(NPC* p)
{
	Warrior* w = (Warrior*)p;
	if (w->getIsReloading())
	{
		w->setIsReloading(false);
		w->setReloadTime(0);
	}
	p->SetIsMoving(true);
}
