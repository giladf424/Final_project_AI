#include "DeliverResourcesState.h"
#include "IdleState.h"
//#include "Squire.h"
//#include "Warrior.h"
#include <iostream>

void DeliverResourcesState::OnEnter(NPC* p)
{
    // Initialize deliver resources behavior
    Squire* s = (Squire*)p;
	// get the first warrior in the queue
	// if the queue is empty, return
	if (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
	{
		
        std::cout << "No wounded warriors in the queue\n";
		s->SetPrevPosition(s->GetPosition());
        s->GetState()->Transition(p);
		return;
	}
	NPC* n = (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.front());
    
    //s->deliverToWarrior(w);
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
