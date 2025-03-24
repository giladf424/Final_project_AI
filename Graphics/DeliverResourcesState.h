#pragma once
#include "State.h"
class DeliverResourcesState :
	public State
{
public:
	void OnEnter(NPC* p);
	void Transition(NPC* p);
	void OnExit(NPC* p);
	string toString() { return "DeliverResources"; }

};

