#include "SeekResourcesState.h"
#include "DeliverResourcesState.h"
#include "Squire.h"
#include "IdleState.h"
#include <iostream>

void SeekResourcesState::OnEnter(NPC* p)
{
	Squire* s = (Squire*)p;
	s->SetIsMoving(true);
	s->refillResources();
}

void SeekResourcesState::Transition(NPC* p)
{
	OnExit(p);

	Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	if (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
		s->SetState(new IdleState);
	else
		s->SetState(new DeliverResourcesState());

	delete currentState;
	currentState = nullptr;
}

void SeekResourcesState::OnExit(NPC* p)
{
	Squire* s = (Squire*)p;
	s->SetReStocking(false);
	p->SetIsMoving(true);
}
