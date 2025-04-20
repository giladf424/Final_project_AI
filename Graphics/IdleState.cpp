#include "IdleState.h"
#include "FleeState.h"
//#include "Squire.h"
//#include "Team.h"
//#include <iostream>

void IdleState::OnEnter(NPC* p)
{
	// Initialize idle behavior
	std::cout << "Entering IdleState\n" << endl;
	Squire* s = (Squire*)p;
	int id = s->GetTeamID().team;
	s->SetIsMoving(true);
	Position teammatePos = Team::findNearestTeammate(p);
	NPC* teammateNPC = Team::GetNPCByPosition(teammatePos, s->GetTeamID().team, s->GetTeamID().place, TEAMMATE);
	if (teammateNPC == nullptr || !teammateNPC->GetIsAlive())
	{
		s->SetPrevPosition(s->GetPosition());
		s->GetState()->Transition(p);
		return;
	}
	int roomIndex = GetRoomIndex(teammatePos);
	if (roomIndex == -1)
		roomIndex = (teammateNPC)->getPrevRoomIndex();

	if (Team::GetEnemiesPositionsInRoom(roomIndex, id, true).size() > 0 || !Team::Teams.at(id)->woundedWarriors.empty())
	{
		s->SetPrevPosition(s->GetPosition());
		s->GetState()->Transition(p);
		return;
	}
	Position roomCenter = Team::findRoomCenter(roomIndex);
	if (s->isValidPos(roomCenter))
		s->MoveToTeamMate(roomCenter);
	else
	{
		s->SetPrevPosition(s->GetPosition());
		std::cout << "Error: no room center pos found from IdleState" << endl;
	}
	std::cout << "Exiting IdleState\n" << endl;
}

void IdleState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	p->SetState(new FleeState());

	delete currentState; // Clean up the old state
	currentState = nullptr;

	// entering new state
  //  p->GetState()->OnEnter(p);
}

void IdleState::OnExit(NPC* p)
{
	// Clean up idle behavior
	p->SetIsMoving(true);
	std::cout << "Exiting IdleState" << endl;
}
