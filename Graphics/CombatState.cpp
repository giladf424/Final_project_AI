#include "CombatState.h"
#include "PatrolState.h"
#include "RequestResourcesState.h"
#include "Warrior.h"
#include <iostream>

void CombatState::OnEnter(NPC* p)
{
	// Initialize combat behavior
	std::cout << "Entering CombatState\n\n";
	Warrior* w = (Warrior*)p;
	w->SetIsMoving(true);
	w->attackEnemy();
	w->SetIsMoving(false);
	std::cout << "Exiting CombatState\n\n";
}

void CombatState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	Warrior* w = (Warrior*)p;
	State* currentState = p->GetState();
	bool isAggressive = w->GetAggressive();
	int numBullets = w->getAmmo();

	if ((!isAggressive && w->GetHp() < HP_TH) || isAggressive && w->GetHp() < HP_TH_AGGRESSIVE)/* low health or low ammunition */
	{
		p->SetState(new RequestResourcesState());
	}
	else if ((!isAggressive && numBullets < AMMO_TH) || (isAggressive && numBullets < AMMO_TH_AGGRESSIVE) || w->getGrenades() < GRENADE_TH)
	{
		p->SetState(new RequestResourcesState());
	}
	else //if (/* enemy is no longer in range */) 
	{
		p->SetState(new PatrolState());
	}
	delete currentState; // Clean up the old state
	currentState = nullptr;
	// entering new state
	//p->GetState()->OnEnter(p);
}

void CombatState::OnExit(NPC* p)
{
	// Clean up combat behavior
	p->SetIsMoving(true);
	std::cout << "Exiting CombatState\n";
}
