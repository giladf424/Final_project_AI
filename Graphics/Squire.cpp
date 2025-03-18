#include "Squire.h"

Squire::Squire(Position startPos) : NPC(startPos)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_SQUIRE;
}
