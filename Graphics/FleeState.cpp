#include "FleeState.h"
//#include "PatrolState.h"
//#include "RequestResourcesState.h"
#include "DeliverResourcesState.h"
#include "Warrior.h"
#include <iostream>

void FleeState::OnEnter(NPC* p)
{
	// Initialize combat behavior
	//Warrior* w = (Warrior*)p;
	//w->SetIsMoving(true);
	std::cout << "Entering FleeState\n";
	Squire* s = (Squire*)p;
	if (!Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
	{
		s->SetPrevPosition(s->GetPosition());
		s->GetState()->Transition(p);
		return;
	}
	s->SetIsMoving(true);
	Position target = Team::findNearestTeammate(p);
	int roomIndex = GetRoomIndex(target);

	if (roomIndex == -1)
		roomIndex = (Team::GetNPCByPosition(target, s->GetTeamID().team, s->GetTeamID().place))->getPrevRoomIndex();

	vector<RoomDetails> connectedRooms = Team::getConnectedRooms(roomIndex, s->GetTeamID().team);
	int roomIndexToScan = s->findSafestRoom(connectedRooms);
	Position roomCenter = Team::findRoomCenter(roomIndexToScan);
	if (s->isValidPos(roomCenter))
		s->MoveToTeamMate(roomCenter);
	else
	{
		s->SetPrevPosition(s->GetPosition());
		std::cout << "Error: no room center pos found from FleeState\n";
	}
	//w->SetIsMoving(false);
	std::cout << "Exiting FleeState\n";
}

void FleeState::Transition(NPC* p)
{
	// Exiting from current state
	OnExit(p);

	//Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	p->SetState(new DeliverResourcesState());

	delete currentState; // Clean up the old state
	currentState = nullptr;
	// entering new state
  //  p->GetState()->OnEnter(p);

}

void FleeState::OnExit(NPC* p)
{
	// Clean up flee behavior
	p->SetIsMoving(true);
	//std::cout << "Exiting FleeState\n";
}
