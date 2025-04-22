#pragma once
#include "NPC.h"
#include <queue>

class Squire : public NPC
{
private:
	int bulletsPack, grenadesPack, healthPack;
	int safetyRoomIndex;
	bool reStocking;
public:
	Squire(Position startPos, TeamID teamID);
	void SetBulletsPack(int b) { bulletsPack = b; }
	void SetGrenadesPack(int g) { grenadesPack = g; }
	void SetHealthPack(int h) { healthPack = h; }
	int GetHealthPack() { return healthPack; }
	int GetBulletsPack() { return bulletsPack; }
	int GetGrenadesPack() { return grenadesPack; }
	void SetReStocking(bool r) { reStocking = r; }
	bool GetReStocking() { return reStocking; }
	int GetSafetyRoomIndex() { return safetyRoomIndex; }
	void SetSafetyRoomIndex(int index) { safetyRoomIndex = index; }

	const char* getType() override { return "Squire"; }
	int getAmmo() override { return bulletsPack; }
	int getGrenades() override { return grenadesPack; }

	void MoveToTeamMate(Position sRoomCenter);
	int findSafestRoom(vector<RoomDetails>& connectedRooms);

	bool deliverToWarrior(NPC* w);
	bool moveToWarrior(Position warriorPos);
	void refillResources();
	Position findNearestStash(int stashType);
	void runAwayFromEnemy();

};

