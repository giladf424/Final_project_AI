#include "NPC.h"

NPC::NPC(Position startPos, int id)
{
	hp = MAX_HP;
	teamID = id;
	isMoving = false;
	pos = startPos;
	pCurrentState = nullptr;
}
