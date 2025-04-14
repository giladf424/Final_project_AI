#include "IdleState.h"
#include "FleeState.h"
#include "Squire.h"
#include <iostream>

void IdleState::OnEnter(NPC* p)
{
    // Initialize idle behavior
    std::cout << "Entering IdleState\n\n";
    Squire* s = (Squire*)p;
    int id = s->GetTeamID().team;
	/*if (Team::GetEnemiesPositionsInRoom(s->getRoomIndex(), id, true).size() > 0 || !Team::Teams.at(id)->woundedWarriors.empty())
	{
		s->SetPrevPosition(s->GetPosition());
        s->GetState()->Transition(p);
		return;
	}*/
    s->SetIsMoving(true);
	Position teammatePos = Team::findNearestTeammate(p);
	if (s->isValidPos(teammatePos))
		s->MoveToTeamMate(teammatePos);
	else
		s->SetPrevPosition(s->GetPosition());
    std::cout << "Exiting IdleState\n\n";
}

void IdleState::Transition(NPC* p)
{
    // Exiting from current state
    OnExit(p);

    Squire* s = (Squire*)p;
   // s->SetState(new FleeState());

    // entering new state
  //  p->GetState()->OnEnter(p);
}

void IdleState::OnExit(NPC* p)
{
    // Clean up idle behavior
    p->SetIsMoving(true);
    std::cout << "Exiting IdleState\n";
}
