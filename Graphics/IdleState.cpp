#include "IdleState.h"
#include "SeekResourcesState.h"
#include "Squire.h"
#include <iostream>

void IdleState::OnEnter(NPC* p)
{
    // Initialize idle behavior
    Squire* s = (Squire*)p;
    s->SetIsMoving(true);
	Position pos = Team::findNearestTeammate(p);
	if (pos.row != -1 && pos.col != -1)
		//s->MoveToTeamMate(pos);
    std::cout << "Entering IdleState\n";
}

void IdleState::Transition(NPC* p)
{
    // Exiting from current state
    OnExit(p);

    Squire* s = (Squire*)p;
    s->SetState(new SeekResourcesState());

    // entering new state
    p->GetState()->OnEnter(p);
}

void IdleState::OnExit(NPC* p)
{
    // Clean up idle behavior
    p->SetIsMoving(true);
    std::cout << "Exiting IdleState\n";
}
