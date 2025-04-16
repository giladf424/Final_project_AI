#include "RequestResourcesState.h"
//#include "CombatState.h"
#include "Warrior.h"
#include <iostream>

void RequestResourcesState::OnEnter(NPC* p)
{
    // Initialize request resources behavior
    Warrior* w = (Warrior*)p;
    //w->SetIsMoving(false);
    //p->GetState()->Transition(p);
	if (w->isWarriorCanReturnToFight())
	{
		std::cout << "Warrior can return to fight\n";
		w->SetIsMoving(true);
		w->GetState()->Transition(p);
		return;
	}
    w->addWoundedWarriorToQueue();
	w->SetIsMoving(true);
	NPC* teammateSquire = Team::findNearestSquireEnemyOrTeammate(p, false);
	Position teammateSquirePosition = teammateSquire->GetPosition();
    if (w->isValidPos(teammateSquirePosition))
        w->moveToSquire(teammateSquirePosition);
    std::cout << "Entering RequestResourcesState\n";
}

void RequestResourcesState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);
    State* currentState = p->GetState();
	p->SetState(new PatrolState());
    delete currentState; // Clean up the old state
    currentState = nullptr;
    // entering new state
   // p->GetState()->OnEnter(p);
}

void RequestResourcesState::OnExit(NPC* p)
{
    // Clean up request resources behavior
    p->SetIsMoving(true);
    std::cout << "Exiting RequestResourcesState\n";
}
