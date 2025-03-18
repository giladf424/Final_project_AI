#pragma once
#include "definitions.h"
#include "State.h"
#include "NPC.h"

class Warrior : public NPC
{
private:
	int bullets, grenades;
public:
	Warrior(Position startPos);
	void SetBullets(int b) { bullets = b; }
	int GetBullets() { return bullets; }
	void SetGrenades(int g) { grenades = g; }
	int GetGrenades() { return grenades; }
// test
};

