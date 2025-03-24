#pragma once
#include "State.h"
#include "Team.h"

class IdleState :
	public State
{
public:
	void OnEnter(NPC* p);
	void Transition(NPC* p);
	void OnExit(NPC* p);
	string toString() { return "Idle"; }

};

