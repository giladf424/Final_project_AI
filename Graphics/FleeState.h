#pragma once
#include "State.h"
class FleeState :
	public State
{
public:
	void OnEnter(NPC* p);
	void Transition(NPC* p);
	void OnExit(NPC* p);
	string toString() { return "Flee"; }

};

