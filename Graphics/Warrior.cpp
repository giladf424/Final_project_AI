#include "Warrior.h"

Warrior::Warrior(Position startPos, int id) : NPC(startPos, id)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
}
