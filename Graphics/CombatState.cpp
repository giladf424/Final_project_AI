#include "CombatState.h"
#include "PatrolState.h"
#include "RequestResourcesState.h"
#include "FleeState.h"

#include "Warrior.h"
#include <iostream>

void CombatState::OnEnter(NPC* p)
{
	// Initialize combat behavior
	Warrior* w = (Warrior*)p;
	w->SetIsMoving(false);
	std::cout << "Entering CombatState\n";
}

void CombatState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	Warrior* w = (Warrior*)p;

	if (w->GetHp() <= 10 || w->GetBullets() <= 5)/* low health or low ammunition */
	{
		w->SetState(new RequestResourcesState());
	}
	else //if (/* enemy is no longer in range */) 
	{
		w->SetState(new PatrolState());
	}
	// entering new state
	p->GetState()->OnEnter(p);
}

void CombatState::OnExit(NPC* p)
{
	// Clean up combat behavior
	p->SetIsMoving(true);
	std::cout << "Exiting CombatState\n";
}
