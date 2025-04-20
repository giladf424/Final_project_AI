#include "PatrolState.h"
#include "CombatState.h"
#include "Warrior.h"
#include <iostream>

void PatrolState::OnEnter(NPC* p)
{
	// Initialize patrol behavior
	std::cout << "Entering PatrolState\n" << endl;
	Warrior* w = (Warrior*)p;
	w->SetIsMoving(true);
	if (w->GetTarget() == nullptr)
		w->SetTarget(Team::findTargetEnemy(w, w->GetAggressive()));
	w->moveToEnemy();
	std::cout << "Exiting PatrolState\n" << endl;
}

void PatrolState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);
	State* currentState = p->GetState();
	// Check for state transitions
	p->SetState(new CombatState());
	delete currentState; // Clean up the old state
	currentState = nullptr;
	// entering new state
	//p->GetState()->OnEnter(p);

}

void PatrolState::OnExit(NPC* p)
{
	// Clean up patrol behavior
	p->SetIsMoving(false);
	std::cout << "Exiting PatrolState" << endl;
}