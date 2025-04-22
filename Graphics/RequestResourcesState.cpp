#include "RequestResourcesState.h"
#include "Warrior.h"
#include <iostream>

void RequestResourcesState::OnEnter(NPC* p)
{
	Warrior* w = (Warrior*)p;
	if (w->isWarriorCanReturnToFight())
	{
		w->SetIsMoving(true);
		w->GetState()->Transition(p);
		return;
	}
	w->addWoundedWarriorToQueue();
	w->SetIsMoving(true);
	NPC* teammateSquire = Team::findNearestSquireEnemyOrTeammate(p, false);
	if (teammateSquire == nullptr)
	{
		w->GetState()->Transition(p);
		return;
	}
	Position teammateSquirePosition = teammateSquire->GetPosition();
	if (w->isValidPos(teammateSquirePosition))
		w->moveToSquire(teammateSquirePosition);
}

void RequestResourcesState::Transition(NPC* p)
{
	OnExit(p);
	State* currentState = p->GetState();
	p->SetState(new PatrolState());
	delete currentState;
	currentState = nullptr;
}

void RequestResourcesState::OnExit(NPC* p)
{
	p->SetIsMoving(true);
}
