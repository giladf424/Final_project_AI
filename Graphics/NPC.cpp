#include "NPC.h"
#include "Grenade.h"

NPC::NPC(Position startPos, TeamID teamID)
{
	hp = MAX_HP;
	id = teamID;
	isMoving = false;
	isAlive = true;
	isStuck = false;
	stuckness = 0;
	pos = startPos;
	prevPos = { -1, -1 };
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
	vector<Cell*> toDelete;
	Cell* pc = new Cell(GetPosition().row, GetPosition().col, target.row, target.col, 0, nullptr, dupMap);
	grays.push(pc);
	toDelete.push_back(pc);
	Cell* nextStep = nullptr;

	while (!nextStep)
		nextStep = RunAStarIteration(target, grays, dupMaze, dupMap, numSteps, toDelete);

	Position nextPos = Position{ nextStep->getRow(), nextStep->getCol() };

	while (!grays.empty())
	{
		Cell* c = grays.top();
		grays.pop();
	}
	while (!toDelete.empty())
	{
		Cell* c = toDelete.back();
		toDelete.pop_back();
		delete c;
		c = nullptr;
	}
	isAstar = false;
	return nextPos;
}

Cell* NPC::RunAStarIteration(Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps, vector<Cell*>& toDelete)
{
	if (grays.empty())
		return nullptr;

	Cell* go_on = nullptr;
	Cell* pCurrent = grays.top();
	grays.pop();

	int row = pCurrent->getRow();
	int col = pCurrent->getCol();

	if (row == target.row && col == target.col)
		return pCurrent;
	
	dupMaze[row][col] = BLACK;

	if (dupMaze[row + 1][col] != WALL && dupMaze[row + 1][col] != BLACK)
		go_on = CheckNeighbor(row + 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps, toDelete);
	if (!go_on && dupMaze[row - 1][col] != WALL && dupMaze[row - 1][col] != BLACK)
		go_on = CheckNeighbor(row - 1, col, pCurrent, target, grays, dupMaze, dupMap, numSteps, toDelete);
	if (!go_on && dupMaze[row][col - 1] != WALL && dupMaze[row][col - 1] != BLACK)
		go_on = CheckNeighbor(row, col - 1, pCurrent, target, grays, dupMaze, dupMap, numSteps, toDelete);
	if (!go_on && dupMaze[row][col + 1] != WALL && dupMaze[row][col + 1] != BLACK)
		go_on = CheckNeighbor(row, col + 1, pCurrent, target, grays, dupMaze, dupMap, numSteps, toDelete);

	return go_on;
}

Cell* NPC::CheckNeighbor(int r, int c, Cell* pCurrent, Position target, priority_queue<Cell*, vector<Cell*>, CompareCells>& grays, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], int* numSteps, vector<Cell*>& toDelete)
{
	if (r == target.row && c == target.col)
	{
		if (pCurrent->getParent() == nullptr && (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_))
		{
			*numSteps = 1;
			return new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap);
		}
		return RestorePath(pCurrent, numSteps);
	}

	if (dupMaze[r][c] == SPACE || (dupMaze[r][c] == NPC_)) // && pCurrent->getParent() != nullptr
	{
		Cell* pc = new Cell(r, c, target.row, target.col, 0, pCurrent, dupMap); //pCurrent->getG() + 1
		grays.push(pc);
		toDelete.push_back(pc);
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
	return pc;
}

Position NPC::RunAStarFlee(int dupMaze[MSZ][MSZ])
{
	isAstar = false;
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	queue<Cell*> grays;
	vector<Position> warriorsEnemy = Team::findAllWarriorsEnemies(this->GetTeamID().team);
	Cell* first = nullptr;
	Position nextPos = { -1, -1 };
	Cell* pc = new Cell(GetPosition().row, GetPosition().col, 0, first, warriorsEnemy);
	grays.push(pc);
	pq.push(pc);
	int r, c;
	int i = 0;
	while (!grays.empty() && i < BFS_RADIUS_SQUIRE)
	{
		Cell* pCurrent = grays.front();
		grays.pop();
		r = pCurrent->getRow();
		c = pCurrent->getCol();
		dupMaze[r][c] = BLACK;

		if (dupMaze[r + 1][c] != WALL && dupMaze[r + 1][c] != BLACK)
			AStarFleeCheckNeighbor(r + 1, c, pCurrent, pq, grays, dupMaze, warriorsEnemy);
		if (dupMaze[r - 1][c] != WALL && dupMaze[r - 1][c] != BLACK)
			AStarFleeCheckNeighbor(r - 1, c, pCurrent, pq, grays, dupMaze, warriorsEnemy);
		if (dupMaze[r][c + 1] != WALL && dupMaze[r][c + 1] != BLACK)
			AStarFleeCheckNeighbor(r, c + 1, pCurrent, pq, grays, dupMaze, warriorsEnemy);
		if (dupMaze[r][c - 1] != WALL && dupMaze[r][c - 1] != BLACK)
			AStarFleeCheckNeighbor(r, c - 1, pCurrent, pq, grays, dupMaze, warriorsEnemy);
		i++;
	}
	
	if (!pq.empty())
	{
		Cell* pCurrent = pq.top();
		pq.pop();
		nextPos = Position{ pCurrent->getRow(), pCurrent->getCol() };
		delete pCurrent;
		pCurrent = nullptr;
		while (!pq.empty())
		{
			Cell* c = pq.top();
			pq.pop();
			delete c;
			c = nullptr;
		}
	}
	return nextPos;
}

void NPC::AStarFleeCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& grays, int dupMaze[MSZ][MSZ], vector<Position>& warriorsEnemy)
{
	if (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_)
	{
		Cell* pNeighbor = new Cell(r, c, 0, pCurrent, warriorsEnemy);
		dupMaze[r][c] = GRAY;
		grays.push(pNeighbor);
		pq.push(pNeighbor);
	}
}

Position NPC::RunBFS(Position target, int dupMaze[MSZ][MSZ], int* numSteps)
{
	queue<Cell*> q;
	vector<Cell*> toDelete;
	Cell* pc = new Cell(GetPosition().row, GetPosition().col, target.row, target.col, nullptr);
	q.push(pc);
	toDelete.push_back(pc);
	Cell* nextStep = nullptr;
	while (!nextStep)
		nextStep = RunBFSIteration(target, dupMaze, q, toDelete, numSteps);

	Position nextPos = Position{ nextStep->getRow(), nextStep->getCol() };

	while (!q.empty())
	{
		Cell* c = q.front();
		q.pop();
	}
	while (!toDelete.empty())
	{
		Cell* c = toDelete.back();
		toDelete.pop_back();
		delete c;
		c = nullptr;
	}
	return nextPos;
}

Cell* NPC::RunBFSIteration(Position target, int dupMaze[MSZ][MSZ], queue<Cell*>& q, vector<Cell*>& toDelete, int* numSteps)
{
	if (q.empty())
		return nullptr;

	Cell* go_on = nullptr;
	Cell* pCurrent = q.front();
	q.pop();
	int row = pCurrent->getRow();
	int col = pCurrent->getCol();
	if (row == target.row && col == target.col)
		return pCurrent;

	dupMaze[row][col] = BLACK;
	if (dupMaze[row + 1][col] != WALL && dupMaze[row + 1][col] != BLACK)
		go_on = BFSCheckNeighbor(row + 1, col, pCurrent, target, dupMaze, q, toDelete, numSteps);
	if (!go_on && dupMaze[row - 1][col] != WALL && dupMaze[row - 1][col] != BLACK)
		go_on = BFSCheckNeighbor(row - 1, col, pCurrent, target, dupMaze, q, toDelete, numSteps);
	if (!go_on && dupMaze[row][col - 1] != WALL && dupMaze[row][col - 1] != BLACK)
		go_on = BFSCheckNeighbor(row, col - 1, pCurrent, target, dupMaze, q, toDelete, numSteps);
	if (!go_on && dupMaze[row][col + 1] != WALL && dupMaze[row][col + 1] != BLACK)
		go_on = BFSCheckNeighbor(row, col + 1, pCurrent, target, dupMaze, q, toDelete, numSteps);

	return go_on;
}

Cell* NPC::BFSCheckNeighbor(int r, int c, Cell* pCurrent, Position target, int dupMaze[MSZ][MSZ], queue<Cell*>& q, vector<Cell*>& toDelete, int* numSteps)
{
	if (r == target.row && c == target.col)
	{
		return RestorePath(pCurrent, numSteps);
	}
	if (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_)
	{
		Cell* pc = new Cell(r, c, target.row, target.col, pCurrent);
		q.push(pc);
		toDelete.push_back(pc);
		dupMaze[r][c] = GRAY;
	}
	return nullptr;
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

bool NPC::isAdjacentToMyPos(Position p)
{
	if ((p.row == GetPosition().row + 1 && p.col == GetPosition().col) ||
		(p.row == GetPosition().row - 1 && p.col == GetPosition().col) ||
		(p.row == GetPosition().row && p.col == GetPosition().col + 1) ||
		(p.row == GetPosition().row && p.col == GetPosition().col - 1))
		return true;
	return false;
}

bool NPC::isSamePositions(Position p1, Position p2)
{
	if (p1.row == p2.row && p1.col == p2.col)
		return true;
	return false;
}

Position NPC::FindFreeSpaceToMove()
{
	Position nextPos = { -1, -1 };
	Position myPos = GetPosition();
	if (maze[myPos.row + 1][myPos.col] == SPACE || maze[myPos.row + 1][myPos.col] == NPC_)
		return { myPos.row + 1, myPos.col };
	else if (maze[myPos.row - 1][myPos.col] == SPACE || maze[myPos.row - 1][myPos.col] == NPC_)
		return { myPos.row - 1, myPos.col };
	else if (maze[myPos.row][myPos.col + 1] == SPACE || maze[myPos.row][myPos.col + 1] == NPC_)
		return { myPos.row, myPos.col + 1 };
	else if (maze[myPos.row][myPos.col - 1] == SPACE || maze[myPos.row][myPos.col - 1] == NPC_)
		return { myPos.row, myPos.col - 1 };
	else
		return nextPos;
}

Position NPC::FindFreeSpaceToMoveInLoop()
{
	Position nextPos = { -1, -1 };
	Position myPos = GetPosition();
	Position prevPos = GetPrevPosition();
	if ((maze[myPos.row + 1][myPos.col] == SPACE || maze[myPos.row + 1][myPos.col] == NPC_) && !isSamePositions({myPos.row + 1, myPos.col}, prevPos))
		return { myPos.row + 1, myPos.col };
	else if ((maze[myPos.row - 1][myPos.col] == SPACE || maze[myPos.row - 1][myPos.col] == NPC_) && !isSamePositions({ myPos.row - 1, myPos.col }, prevPos))
		return { myPos.row - 1, myPos.col };
	else if ((maze[myPos.row][myPos.col + 1] == SPACE || maze[myPos.row][myPos.col + 1] == NPC_) && !isSamePositions({ myPos.row, myPos.col + 1 }, prevPos))
		return { myPos.row, myPos.col + 1 };
	else if ((maze[myPos.row][myPos.col - 1] == SPACE || maze[myPos.row][myPos.col - 1] == NPC_) && !isSamePositions({ myPos.row, myPos.col - 1 }, prevPos))
		return { myPos.row, myPos.col - 1 };
	else
		return nextPos;
}

Position NPC::checkIfStuck(Position nextPos, Position targetPos)
{
	Position defaultPos = { -1, -1 };
	NPC* n = Team::GetNPCByPosition(this->pos, this->id.team, this->id.place, BOTH);
	if (n != nullptr && isSamePositions(n->GetPrevPosition(), this->prevPos) && !isSamePosAsMyPos(this->prevPos))
		return pos;

	if (isSamePositions(nextPos, this->prevPos) && !GetStuck())
		SetStuck(true);
	else if (isSamePositions(nextPos, this->prevPos) && GetStuck() && GetStuckness() == 0)
	{
		SetStuckness(STUCK_TIME);
		//setSecurityMapToZero(dupSecurityMap);
		DuplicateMaze(maze, dupMaze);
		int numSteps = 0;
		Position nextPos2 = RunBFS(targetPos, dupMaze, &numSteps);
		return nextPos2;
	}
	else if (GetStuck() && GetStuckness() > 0)
	{
		SetStuckness(GetStuckness() - 1);
		if (GetStuckness() == 0)
		{
			SetStuck(false);
			return defaultPos;
		}
		setSecurityMapToZero(dupSecurityMap);
		DuplicateMaze(maze, dupMaze);
		int numSteps = 0;
		Position nextPos2 = RunBFS(targetPos, dupMaze, &numSteps);
		return nextPos2;
	}
	else
	{
		if (this->GetStuck())
			SetStuck(false);
	}
	return defaultPos;
}

void NPC::move(Position nextPos)
{
	prevStep = (Team::isAnyBodyInMyPosition(GetPosition(), this->id.team, this->id.place) ? NPC_ : SPACE);
	std::cout << "Moving from (" << GetPosition().row << ", " << GetPosition().col << ") to (" << nextPos.row << ", " << nextPos.col << ") OUT!!!!!!!!!!!!!!!!" << endl;
	if (!isSamePosAsMyPos(nextPos) && isValidPos(nextPos))
	{
		std::cout << "Moving from (" << GetPosition().row << ", " << GetPosition().col << ") to (" << nextPos.row << ", " << nextPos.col << ")" << endl;
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

Position NPC::BFSRadius(Position start, vector <Position>& enemyPos, int radius, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	isAstar = true;
	queue<Cell*> q;
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	vector<Position> enemiesInRoom = Team::GetEnemiesPositionsInRoom(GetRoomIndex(start), GetTeamID().team, false);
	Cell* pc = new Cell(start.row, start.col, nullptr, dupMap, enemiesInRoom);
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
				BFSRadiusCheckNeighbor(r + 1, c, pCurrent, pq, q, dupMaze, dupMap, enemiesInRoom);
			if (dupMaze[r - 1][c] != WALL && dupMaze[r - 1][c] != BLACK)
				BFSRadiusCheckNeighbor(r - 1, c, pCurrent, pq, q, dupMaze, dupMap, enemiesInRoom);
			if (dupMaze[r][c + 1] != WALL && dupMaze[r][c + 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c + 1, pCurrent, pq, q, dupMaze, dupMap, enemiesInRoom);
			if (dupMaze[r][c - 1] != WALL && dupMaze[r][c - 1] != BLACK)
				BFSRadiusCheckNeighbor(r, c - 1, pCurrent, pq, q, dupMaze, dupMap, enemiesInRoom);
		}
		currentRadius++;
	}
	Position bestPos = { -1, -1 };
	bool isWarrior = (strcmp(getType(), "Warrior") == 0);
	bool onlyWarriors = true;
	Position target = { -1, -1 };
	if (isWarrior)
	{
		target = { enemyPos[0].row, enemyPos[0].col };
		onlyWarriors = false;
		enemyPos.pop_back();
	}
	int min_max_NumEnemiesinRange = (isWarrior ? -1 : INT_MAX); // min for warrior, max for squire 
	int startRoomIndex = (GetRoomIndex(start) == -1 ? this->getPrevRoomIndex() : GetRoomIndex(start));
	
	while (!pq.empty())
	{
		Cell* isBest = pq.top();
		pq.pop();
		Position temp = { isBest->getRow(), isBest->getCol() };
		if (Team::findDistance(GetPosition(), temp) < 2 && !Team::isSafePosition(temp, GetTeamID().team, GetTeamID().place))
		{
			delete isBest;
			isBest = nullptr;
			continue;
		}
		enemyPos = Team::GetEnemiesPositionsInRoom(GetRoomIndex(temp), GetTeamID().team, onlyWarriors);
		int numEnemiesinRange = GetEnemiesInHitRange(temp, enemyPos).size();
		bool isTargetInRange = (isWarrior ? (IsEnemyInHitRange(temp, target) >= 0) : false);
		bool isSameRoom = (GetRoomIndex(temp) == startRoomIndex);
		if ((isWarrior && isSameRoom && (numEnemiesinRange > min_max_NumEnemiesinRange || isTargetInRange)) || (!isWarrior && isSameRoom && numEnemiesinRange < min_max_NumEnemiesinRange)) // && (isBest->getRow() != enemyPos.row || isBest->getCol() != enemyPos.col)
		{
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

void NPC::BFSRadiusCheckNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, queue<Cell*>& q, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ], vector<Position>& enemiesInRoom)
{
	if (dupMaze[r][c] == SPACE || dupMaze[r][c] == NPC_) // && pCurrent->getParent() != nullptr
	{
		Cell* pc = new Cell(r, c, pCurrent, dupMap, enemiesInRoom);
		q.push(pc);
		dupMaze[r][c] = GRAY;
		for (Position p : enemiesInRoom)
		{
			if (Team::findDistance(Position{ r, c }, p) <= 2)
				return;
		}
		pq.push(pc);
	}
}

double NPC::IsEnemyInHitRange(Position myPos, Position enemyPos)
{
	if (Team::findDistance(myPos, enemyPos) > 5)
		return -1;
	Grenade* pg = new Grenade(myPos.row, myPos.col);

	double hitRange = pg->IsEnemyFoundByExplosion(maze, enemyPos);

	delete pg;
	pg = nullptr;
	return hitRange;
}

Position NPC::getEntranceToCorridor(int corridorIndex, int roomIndex, bool forSecMap)
{
	Corridor* corridor = Corridor::getCorridorById(corridorIndex);
	if (corridor == nullptr)
		return { -1, -1 };

	for (Position p : corridor->getEntrances())
	{
		Position p2 = { p.row, p.col };
		if (GetRoomIndex({ p.row + 1, p.col }) == roomIndex)
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
	return { -1, -1 };
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

void NPC::UpdateSecurityMap(vector<Position>& positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
{
	int numSimulations = 10;
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

vector<HitDetails> NPC::GetEnemiesInHitRange(Position myPos, vector<Position>& enemiesPos)
{
	int numEnemies = 0;
	vector<HitDetails> enemiesPositions;
	for (Position p : enemiesPos)
	{
		if (Team::findDistance(myPos, p) > 5)
			continue;
		double hitRange = IsEnemyInHitRange(myPos, p);
		if (hitRange != -1)
			enemiesPositions.push_back({p, hitRange});
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
	std::cout << "\nHit by bullet, Team: " << ((id.team == 0) ? "Red " : "Yellow ") << "Place: " << id.place << " Pos: (" << pos.row << ", " << pos.col << ") HP: " << hp << endl;
}


