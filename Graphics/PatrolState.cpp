#include "PatrolState.h"
#include "CombatState.h"
#include "SeekResourcesState.h"
#include "Warrior.h"
#include <iostream>

void PatrolState::OnEnter(NPC* p)
{
	// Initialize patrol behavior
	Warrior* w = (Warrior*)p;
    w->SetIsMoving(true);
	NPC* target = Team::findNearestEnemy(w);
	if (target != nullptr)
		w->moveToEnemy(target);
    std::cout << "Entering PatrolState\n";
}

void PatrolState::Transition(NPC* p)
{
    // Exiting from current state
	OnExit(p);

	p->SetState(new CombatState());

    // entering new state
    p->GetState()->OnEnter(p);

}

void PatrolState::OnExit(NPC* p)
{
    // Clean up patrol behavior
	p->SetIsMoving(false);
    std::cout << "Exiting PatrolState\n";
}