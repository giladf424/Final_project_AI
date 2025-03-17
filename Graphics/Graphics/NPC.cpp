#include "NPC.h"

NPC::NPC(Position startPos)
{
	hp = MAX_HP;
	isMoving = false;
	pos = startPos;
	pCurrentState = nullptr;
}
