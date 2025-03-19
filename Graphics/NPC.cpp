#include "NPC.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	pos = startPos;
	pCurrentState = nullptr;
}
