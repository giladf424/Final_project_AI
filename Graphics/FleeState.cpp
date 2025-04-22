#include "FleeState.h"
#include "DeliverResourcesState.h"
#include "Warrior.h"
#include <iostream>

void FleeState::OnEnter(NPC* p)
{
	Squire* s = (Squire*)p;
	s->SetIsMoving(true);
	Position target = Team::findNearestTeammate(p);
	NPC* targetNPC = Team::GetNPCByPosition(target, s->GetTeamID().team, s->GetTeamID().place, TEAMMATE);
	if (targetNPC != nullptr && targetNPC->GetIsAlive())
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
			s->SetPrevPosition(s->GetPosition());
	}
	else
		s->runAwayFromEnemy();
}

void FleeState::Transition(NPC* p)
{
	OnExit(p);

	State* currentState = p->GetState();
	p->SetState(new DeliverResourcesState());

	delete currentState;
	currentState = nullptr;
}

void FleeState::OnExit(NPC* p)
{
	p->SetIsMoving(true);
}
