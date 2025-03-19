#include "DeliverResourcesState.h"
#include "IdleState.h"
#include "Squire.h"
#include <iostream>

void DeliverResourcesState::OnEnter(NPC* p)
{
    // Initialize deliver resources behavior
    Squire* s = (Squire*)p;
    s->SetIsMoving(true);
    std::cout << "Entering DeliverResourcesState\n";
}

void DeliverResourcesState::Transition(NPC* p)
{
    // Exiting from current state
    OnExit(p);

    Squire* s = (Squire*)p;
    s->SetState(new IdleState());

    // entering new state
    p->GetState()->OnEnter(p);
}

void DeliverResourcesState::OnExit(NPC* p)
{
    // Clean up deliver resources behavior
	p->SetIsMoving(true);
    std::cout << "Exiting DeliverResourcesState\n";
}
