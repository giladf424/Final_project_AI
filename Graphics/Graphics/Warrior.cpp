#include "Warrior.h"

Warrior::Warrior(Position startPos) : NPC(startPos)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
}
