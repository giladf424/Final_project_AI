#pragma once
#include "definitions.h"
#include "State.h"

class State;

class NPC
{
protected:
	int hp;
	int teamID;
	bool isAlive = true;
	bool isMoving;
	Position pos;
	State* pCurrentState;
public:
	NPC(Position startPos, int id);
	void SetState(State* pState) { pCurrentState = pState; }
	State* GetState() { return pCurrentState; }
	void SetPosition(Position p) { pos = p; }
	Position GetPosition() { return pos; }
	void SetIsMoving(bool value) { isMoving = value; }
	bool GetIsMoving() { return isMoving; }
	void SetHp(int h) { hp = h; }
	int GetHp() { return hp; }
	void SetIsAlive(bool value) { isAlive = value; }
	bool GetIsAlive() { return isAlive; }
	int GetTeamID() { return teamID; }
};

