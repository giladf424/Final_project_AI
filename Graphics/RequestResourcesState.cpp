#include "RequestResourcesState.h"
#include "FleeState.h"
#include "Warrior.h"
#include <iostream>

void RequestResourcesState::OnEnter(NPC* p)
{
    // Initialize request resources behavior
    Warrior* w = (Warrior*)p;
    w->SetIsMoving(false);
    std::cout << "Entering RequestResourcesState\n";
}

void RequestResourcesState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);
	p->SetState(new FleeState());

    // entering new state
    p->GetState()->OnEnter(p);
}

void RequestResourcesState::OnExit(NPC* p)
{
    // Clean up request resources behavior
    p->SetIsMoving(true);
    std::cout << "Exiting RequestResourcesState\n";
}
