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

    Squire* s = (Squire*)p;
    s->SetState(new DeliverResourcesState());

    // entering new state
    p->GetState()->OnEnter(p);
}

void SeekResourcesState::OnExit(NPC* p)
{
    // Clean up seek resources behavior
	p->SetIsMoving(true);
    std::cout << "Exiting SeekResourcesState\n";
}
