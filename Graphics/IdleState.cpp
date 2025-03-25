#include "IdleState.h"
#include "FleeState.h"
#include "Squire.h"
#include <iostream>

void IdleState::OnEnter(NPC* p)
{
    // Initialize idle behavior
    Squire* s = (Squire*)p;
    s->SetIsMoving(true);
	Position teammatePos = Team::findNearestTeammate(p);
	if (teammatePos.row != -1 && teammatePos.col != -1)
		s->MoveToTeamMate(teammatePos);
    //if(abs(teammatePos.row - s->GetPosition().row) + abs(teammatePos.col - s->GetPosition().col) <= 2)
    if(Team::GetTeammate(teammatePos)->GetState()->toString()._Equal("Combat"))
		Transition(p);
    std::cout << "Entering IdleState\n";
}

void IdleState::Transition(NPC* p)
{
    // Exiting from current state
    OnExit(p);

    Squire* s = (Squire*)p;
    s->SetState(new FleeState());

    // entering new state
    p->GetState()->OnEnter(p);
}

void IdleState::OnExit(NPC* p)
{
    // Clean up idle behavior
    p->SetIsMoving(true);
    std::cout << "Exiting IdleState\n";
}
