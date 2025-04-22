#include "PatrolState.h"
#include "CombatState.h"
#include "Warrior.h"
#include <iostream>

void PatrolState::OnEnter(NPC* p)
{
	Warrior* w = (Warrior*)p;
	w->SetIsMoving(true);
	if (w->GetTarget() == nullptr)
		w->SetTarget(Team::findTargetEnemy(w, w->GetAggressive()));
	w->moveToEnemy();
}

void PatrolState::Transition(NPC* p)
{
	OnExit(p);
	State* currentState = p->GetState();
	p->SetState(new CombatState());
	delete currentState;
	currentState = nullptr;
}

void PatrolState::OnExit(NPC* p)
{
	p->SetIsMoving(false);
}