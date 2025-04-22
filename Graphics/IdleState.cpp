#include "IdleState.h"
#include "FleeState.h"

void IdleState::OnEnter(NPC* p)
{
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
		s->SetPrevPosition(s->GetPosition());
}

void IdleState::Transition(NPC* p)
{
	OnExit(p);

	Squire* s = (Squire*)p;
	State* currentState = p->GetState();
	p->SetState(new FleeState());

	delete currentState;
	currentState = nullptr;
}

void IdleState::OnExit(NPC* p)
{
	p->SetIsMoving(true);
}
