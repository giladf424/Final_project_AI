#include "DeliverResourcesState.h"
#include "IdleState.h"
//#include "Squire.h"
//#include "Warrior.h"
#include "SeekResourcesState.h"
#include <iostream>

void DeliverResourcesState::OnEnter(NPC* p)
{
	// Initialize deliver resources behavior
	std::cout << "Entering DeliverResourcesState" << endl;
	Squire* s = (Squire*)p;
	if (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
	{
		std::cout << "No wounded warriors in the queue" << endl;
		s->SetPrevPosition(s->GetPosition());
		s->GetState()->Transition(p);
		return;
	}
	s->SetIsMoving(true);
	NPC* n = (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.front());
	bool toDeliver = true;
	if (!(s->isAdjacentToMyPos(n->GetPosition())))
		toDeliver = s->moveToWarrior(n->GetPosition());
	else
		s->SetPrevPosition(s->GetPosition());

	if (toDeliver)
	{
		if (s->deliverToWarrior(n))
			Team::Teams.at(s->GetTeamID().team)->woundedWarriors.pop();

		if (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty() || s->GetReStocking())
			s->GetState()->Transition(p);
	}
	std::cout << "Exiting DeliverResourcesState" << endl;
}

void DeliverResourcesState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	if (s->GetReStocking())
		p->SetState(new SeekResourcesState());
	else
		p->SetState(new IdleState());

	delete currentState; // Clean up the old state
	currentState = nullptr;
	// entering new state
	//p->GetState()->OnEnter(p);
}

void DeliverResourcesState::OnExit(NPC* p)
{
	// Clean up deliver resources behavior
	p->SetIsMoving(true);
	std::cout << "Exiting DeliverResourcesState" << endl;
}
