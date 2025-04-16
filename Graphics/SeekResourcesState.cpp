#include "SeekResourcesState.h"
#include "DeliverResourcesState.h"
#include "Squire.h"
#include <iostream>

void SeekResourcesState::OnEnter(NPC* p)
{
    // Initialize seek resources behavior
    Squire* s = (Squire*)p;
    s->SetIsMoving(true);
    s->refillResources();
    std::cout << "Entering SeekResourcesState\n";
}

void SeekResourcesState::Transition(NPC* p)
{
    // Exiting from current state
    OnExit(p);

	// Clean up the old state

    Squire* s = (Squire*)p;
	State* currentState = p->GetState();

    s->SetState(new DeliverResourcesState());

	delete currentState; // Clean up the old state
	currentState = nullptr;

    // entering new state
    p->GetState()->OnEnter(p);
}

void SeekResourcesState::OnExit(NPC* p)
{
    // Clean up seek resources behavior
	p->SetIsMoving(true);
    std::cout << "Exiting SeekResourcesState\n";
}
