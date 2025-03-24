#pragma once
#include "NPC.h"

class NPC;

class State
{
public:
	virtual ~State() = default;
	virtual void OnEnter(NPC* npc) = 0;
	virtual void Transition(NPC* npc) = 0;
	virtual void OnExit(NPC* npc) = 0;
	virtual string toString() = 0;
};

