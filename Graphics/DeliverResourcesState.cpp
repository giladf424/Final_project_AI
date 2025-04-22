#include "DeliverResourcesState.h"
#include "IdleState.h"
#include "SeekResourcesState.h"
#include <iostream>

void DeliverResourcesState::OnEnter(NPC* p)
{
	Squire* s = (Squire*)p;
	if (Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
	{
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
}

void DeliverResourcesState::Transition(NPC* p)
{
	OnExit(p);

	Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	if (s->GetReStocking())
		p->SetState(new SeekResourcesState());
	else
		p->SetState(new IdleState());

	delete currentState;
	currentState = nullptr;
}

void DeliverResourcesState::OnExit(NPC* p)
{
	p->SetIsMoving(true);
}
