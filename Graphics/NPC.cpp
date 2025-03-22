#include "NPC.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	pos = startPos;
}

void NPC::move(Position p)
{
	maze[GetPosition().row][GetPosition().col] = SPACE;
	maze[pos.row][pos.col] = NPC_;
	SetPosition(p);
}
