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
	std::cout << "Entering FleeState" << endl;
	Squire* s = (Squire*)p;
	s->SetIsMoving(true);
	Position target = Team::findNearestTeammate(p);
	NPC* targetNPC = Team::GetNPCByPosition(target, s->GetTeamID().team, s->GetTeamID().place, TEAMMATE);
	if(targetNPC != nullptr && targetNPC->GetIsAlive())
	{
		if (!Team::Teams.at(s->GetTeamID().team)->woundedWarriors.empty())
		{
			s->SetPrevPosition(s->GetPosition());
			s->GetState()->Transition(p);
			return;
		}
		int roomIndex = GetRoomIndex(target);

		if (roomIndex == -1)
			roomIndex = targetNPC->getPrevRoomIndex();

		vector<RoomDetails> connectedRooms = Team::getConnectedRooms(roomIndex, s->GetTeamID().team);
		int roomIndexToScan = s->findSafestRoom(connectedRooms);
		s->SetSafetyRoomIndex(roomIndexToScan);
		Position roomCenter = Team::findRoomCenter(roomIndexToScan);
		if (s->isValidPos(roomCenter))
			s->MoveToTeamMate(roomCenter);
		else
		{
			s->SetPrevPosition(s->GetPosition());
			std::cout << "Error: no room center pos found from FleeState" << endl;
		}
	}
	else
		s->runAwayFromEnemy();
	std::cout << "Exiting FleeState" << endl;
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
