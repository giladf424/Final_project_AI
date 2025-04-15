#include "NPC.h"
#include "Grenade.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	isAlive = true;
	pos = startPos;
	prevPos = {-1, -1};
	corridorIndex = -1;
	prevStep = SPACE;
	prevRoomIndex = this->getRoomIndex();
	pCurrentState = nullptr;
}

int NPC::getRoomIndex()
{
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		if (pos.row >= roomScopes[i].startRow && pos.row <= roomScopes[i].endRow && pos.col >= roomScopes[i].startCol && pos.col <= roomScopes[i].endCol)
			return i;
	}
	return -1;
}

Position NPC::RunAStar(Position target, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	isAstar = true;
	priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
	Cell* pc = new Cell(GetPosition().row,GetPosition().col, target.row, target.col, 0, nullptr, dupMap);
	grays.push(pc);
	Cell* nextStep = nullptr;

	while (!nextStep)
		nextStep = RunAStarIteration(target, grays, dupMaze, dupMap, numSteps);
	
	Position nextPos = Position { nextStep->getRow(), nextStep->getCol() };

	while (!grays.empty())
	{
		Cell* c = grays.top();
		grays.pop();
		delete c;
		c = nullptr;
	}
	isAstar = false;
	return nextPos;
}

Cell* NPC::RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	if (grays.empty())
		return nullptr;

	Cell* go_on = nullptr;
	Cell* pCurrent = grays.top();
	grays.pop();

	int row = pCurrent->getRow();
	int col = pCurrent->getCol();

	if (row == target.row && col == target.col)
	{
		std::cout << "The target in the same position (" << row<< ", " << col << ")\n";
		return pCurrent;
	}
	// need to check if instead assign BLACK to dupMap, we need to assign it to maze
	dupMaze[row][col] = BLACK;
	
	if (dupMaze[row + 1][col] != WALL && dupMaze[row + 1][col] != BLACK)
		go_on = CheckNeighbor(row + 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row - 1][col] != WALL && dupMaze[row - 1][col] != BLACK)
		go_on = CheckNeighbor(row - 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row][col - 1] != WALL && dupMaze[row][col - 1] != BLACK)
		go_on = CheckNeighbor(row, col - 1, pCurrent, target, grays, dupMaze, dupMap, numSteps);
	if (!go_on && dupMaze[row][col + 1] != WALL && dupMaze[row][col + 1] != BLACK)
		go_on = CheckNeighbor(row, col + 1, pCurrent, target, grays, dupMaze, dupMap, numSteps);

	return go_on;
}

Cell* NPC::CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps)
{
	if (r == target.row && c == target.col)
	{
		if(pCurrent->getParent() == nullptr && (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_))
		{
			std::cout << "The target in the next position (" << r << ", " << c << ")\n";
			*numSteps = 1;
			return new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap);
		}
		return RestorePath(pCurrent, numSteps);
	}

	if (dupMaze[r][c] == SPACE || (dupMaze[r][c] == NPC_)) // && pCurrent->getParent() != nullptr
	{
		Cell* pc = new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap);
		grays.push(pc);
		dupMaze[r][c] = GRAY;
	}
	return nullptr;
}

Cell* NPC::RestorePath(Cell* pc, int* numSteps)
{
    *numSteps = 2;
	while (pc->getParent() && pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
		*numSteps += 1;
	}
	std::cout << "Radius : " << *numSteps << " Restroe path A*\n";
	return pc;
}

bool NPC::IsEnemyInCorridorConnectedToMyRoom(int corridorIndex)
{
	for (Corridor* c : Corridor::corridors)
	{
		if (c->getId() == corridorIndex)
			return c->isConnectedRoom(getRoomIndex());
	}
	return false;
}

bool NPC::IsEnemyInSameRoom(int roomIndex)
{
	int myRoomIndex = getRoomIndex();
	if (myRoomIndex != -1 && myRoomIndex == roomIndex)
		return true;
	return false;
}

bool NPC::isValidPos(Position nextStep)
{
	if (nextStep.row != -1 && nextStep.col != -1)
		return true;
	return false;
}

bool NPC::isSamePosAsMyPos(Position p)
{
	if (p.row == GetPosition().row && p.col == GetPosition().col)
		return true;
	return false;
}

bool NPC::isStillInSamePos()
{
	if (prevPos.row == pos.row && prevPos.col == pos.col)
		return true;
	return false;
}

void NPC::move(Position nextPos)
{
	prevStep = (Team::isAnyBodyInMyPosition(GetPosition(), this->id.team, this->id.place) ? NPC_ : SPACE);
	std::cout << "Moving from (" << GetPosition().row << ", " << GetPosition().col << ") to (" << nextPos.row << ", " << nextPos.col << ") OUT!!!!!!!!!!!!!!!!\n";
	if ((nextPos.row != GetPosition().row || nextPos.col != GetPosition().col) && isValidPos(nextPos))
	{
		std::cout << "Moving from (" << GetPosition().row << ", " << GetPosition().col << ") to (" << nextPos.row << ", " << nextPos.col << ")\n";
		maze[GetPosition().row][GetPosition().col] = this->prevStep;
		prevStep = maze[nextPos.row][nextPos.col];
		maze[nextPos.row][nextPos.col] = NPC_;
		SetPrevPosition(GetPosition());
		SetPosition(nextPos);
		if (getRoomIndex() == -1 && GetRoomIndex(GetPrevPosition()) != -1)
		{
			for (Corridor* c : Corridor::corridors)
			{
				if (c->isEntrance(GetPosition()))
				{
					SetCorridorIndex(c->getId());
					return;
				}
			}
		}
		else if (getRoomIndex() != -1 && GetRoomIndex(GetPrevPosition()) == -1)
		{
			SetCorridorIndex(-1);
			prevRoomIndex = getRoomIndex();
		}
	}
	else if (!isSamePosAsMyPos(prevPos))
		SetPrevPosition(GetPosition());
}

Position NPC::BFSRadius(Position start, vector <Position> enemyPos, int radius, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	isAstar = true;
	queue<Cell*> q;
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	Cell* pc = new Cell(start.row, start.col, nullptr, dupMap);
	q.push(pc);
	if (dupMaze[start.row][start.col] == SPACE || dupMaze[start.row][start.col] == NPC_)
		pq.push(pc);
	int r, c;
	int currentRadius = 0;

	while (!q.empty() && currentRadius <= radius)
	{
		int layerSize = q.size();
		for (int i = 0; i < layerSize; i++)
		{
			Cell* pCurrent = q.front();
			q.pop();
			r = pCurrent->getRow();
			c = pCurrent->getCol();
			dupMaze[r][c] = BLACK;

			if (dupMaze[r + 1][c] != WALL && dupMaze[r + 1][c] != BLACK)
				BFSRadiusCheckNeighbor(r + 1, c, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r - 1][c] != WALL && dupMaze[r - 1][c] != BLACK)
				BFSRadiusCheckNeighbor(r - 1, c, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r][c + 1] != WALL && dupMaze[r][c + 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c + 1, pCurrent, pq, q, dupMaze, dupMap);
			if (dupMaze[r][c - 1] != WALL && dupMaze[r][c - 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c - 1, pCurrent, pq, q, dupMaze, dupMap);
		}
		currentRadius++;
	}
	Position bestPos = {-1, -1};
	bool isWarrior = (strcmp(getType(), "Warrior") == 0);
	bool onlyWarriors = true;
	Position target = { -1, -1 };
	if(isWarrior)
	{
		target = { enemyPos[0].row, enemyPos[0].col };
		onlyWarriors = false;
		enemyPos.pop_back();
	}
	int min_max_NumEnemiesinRange = (isWarrior ? 0 : INT_MAX); // min for warrior, max for squire 
	int startRoomIndex = GetRoomIndex(start);
	bool isSameRoom = false;
	/*std::cout << "Size: " << q.size() << " ###############################\n\n";
	std::cout << "Current radius: " << currentRadius << "\nSize: " << pq.size() << "###############################\n\n";*/
	while (!pq.empty())
	{
		Cell* isBest = pq.top();
		pq.pop();
		Position temp = { isBest->getRow(), isBest->getCol() };
		if (this->isSamePosAsMyPos(temp) && Team::isAnyBodyInMyPosition(this->pos, this->id.team, this->id.place) && (Team::GetNPCByPosition(this->pos, this->id.team, this->id.place))->isStillInSamePos())
		{
			delete isBest;
			isBest = nullptr;
			continue;
		}
		enemyPos = Team::GetEnemiesPositionsInRoom(GetRoomIndex(temp), GetTeamID().team, onlyWarriors);
		int numEnemiesinRange = GetEnemiesInHitRange(temp, enemyPos).size();
		bool isTargetInRange = (isWarrior ? IsEnemyInHitRange(temp, target) : false);
		if (startRoomIndex == -1)
			isSameRoom = (GetRoomIndex(temp) != startRoomIndex);
		else
			isSameRoom = (GetRoomIndex(temp) == startRoomIndex);
		//std::cout << "Enemy in hit range: " << inRange << "--------------- out -------------\n";
		if ((isWarrior && isSameRoom && (numEnemiesinRange > min_max_NumEnemiesinRange || isTargetInRange)) || (!isWarrior && isSameRoom && numEnemiesinRange < min_max_NumEnemiesinRange)) // && (isBest->getRow() != enemyPos.row || isBest->getCol() != enemyPos.col)
		{
			//std::cout << "Enemy in hit range: " << inRange << "--------------------------------\n";
			bestPos.row = isBest->getRow();
			bestPos.col = isBest->getCol();
			if (isWarrior && isTargetInRange)
			{
				delete isBest;
				isBest = nullptr;
				break;
			}
			else
				min_max_NumEnemiesinRange = numEnemiesinRange;
		}
		delete isBest;
		isBest = nullptr;
	}
	while (!pq.empty())
	{
		Cell* p = pq.top();
		pq.pop();
		delete p;
		p = nullptr;
	}
	while (!q.empty())
	{
		Cell* p = q.front();
		q.pop();
		/*delete p;
		p = nullptr;*/
	}
	isAstar = false;
	return bestPos;
}

void NPC::BFSRadiusCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& q, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	if (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_) // && pCurrent->getParent() != nullptr
	{
		Cell* pc = new Cell(r, c, pCurrent, dupMap);
		pq.push(pc);
		q.push(pc);
		dupMaze[r][c] = GRAY;
	}
}

bool NPC::IsEnemyInHitRange(Position myPos, Position enemyPos)
{
	bool inRange = false;

	Grenade* pg = new Grenade(myPos.row, myPos.col);

	if (pg->IsEnemyFoundByExplosion(maze, enemyPos))
		inRange = true;

	delete pg;
	pg = nullptr;
	return inRange;
}

Position NPC::getEntranceToCorridor(int corridorIndex, int roomIndex, bool forSecMap)
{
	Corridor* corridor = Corridor::getCorridorById(corridorIndex);
	if (corridor == nullptr)
	{
		std::cout << "Error: corridor not found\n";
		return { -1, -1 };
	}
	for (Position p : corridor->getEntrances())
	{
		Position p2 = { p.row, p.col };
		if (GetRoomIndex({p.row + 1, p.col}) == roomIndex)
		{
			p2.row = p.row + 1;
			return (forSecMap) ? p2 : p;
		}
		if (GetRoomIndex({ p.row - 1, p.col }) == roomIndex)
		{
			p2.row = p.row - 1;
			return (forSecMap) ? p2 : p;
		}
		if (GetRoomIndex({ p.row, p.col + 1 }) == roomIndex)
		{
			p2.col = p.col + 1;
			return (forSecMap) ? p2 : p;
		}
		if (GetRoomIndex({ p.row, p.col - 1 }) == roomIndex)
		{
			p2.col = p.col - 1;
			return (forSecMap) ? p2 : p;
		}
	}
	std::cout << "Error: entrance not found\n";
	return {-1, -1};
}

vector<Position> NPC::GetAllEntrancesToMyRoom(int roomIndex, bool forSecMap)
{
	vector<Position> entrances;
	for (Corridor* c : Corridor::corridors)
	{
		if (c->isConnectedRoom(roomIndex))
			entrances.push_back(getEntranceToCorridor(c->getId(), roomIndex, forSecMap));
	}
	return entrances;
}

void NPC::UpdateSecurityMap(vector<Position> positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	int numSimulations = 100;
	for (Position p : positions)
	{
		for (int i = 0; i < numSimulations; i++)
		{
			Grenade* g = new Grenade(p.row, p.col);
			g->SimulateExplosion(dupMaze, dupMap);
			delete g;
			g = nullptr;
		}
	}
}

vector<Position> NPC::GetEnemiesInHitRange(Position myPos, vector<Position> enemiesPos)
{
	int numEnemies = 0;
	vector<Position> enemiesPositions;
	for (Position p : enemiesPos)
	{
		if (IsEnemyInHitRange(myPos, p))
			enemiesPositions.push_back(p);
	}
	return enemiesPositions;
}

void NPC::hitByBullet()
{
	hp -= DAMAGE;
	if (hp <= 0)
	{
		isAlive = false;
		maze[pos.row][pos.col] = (Team::isAnyBodyInMyPosition(pos, this->id.team, this->id.place) ? NPC_ : SPACE);
	}
}


