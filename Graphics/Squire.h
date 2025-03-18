#pragma once
#include "NPC.h"

class Squire : public NPC
{
private:
	int bulletsPack, grenadesPack, healthPack;
public:
	Squire(Position startPos);
	void SetBulletsPack(int b) { bulletsPack = b; }
	int GetBulletsPack() { return bulletsPack; }
	void SetGrenadesPack(int g) { grenadesPack = g; }
	int GetGrenadesPack() { return grenadesPack; }
	void SetHealthPack(int h) { healthPack = h; }
	int GetHealthPack() { return healthPack; }

};

