#include "Squire.h"

Squire::Squire(Position startPos, int id) : NPC(startPos, id)
{
	bulletsPack = MAX_BULLETS_SQUIRE;
	grenadesPack = MAX_GRENADES_SQUIRE;
	healthPack = MAX_HP_SQUIRE;
}
