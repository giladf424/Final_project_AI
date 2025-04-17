
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include <sstream>
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "definitions.h"
#include "Team.h"
#include "Corridor.h"
#include "Warrior.h"

using namespace std;

const int WIDTH = 700;
const int HEIGHT = 700;



Room* rooms[NUM_ROOMS];
RoomScope roomScopes[NUM_ROOMS];
vector<Corridor> corridors;
vector<Position> ammunitionStash;
vector<Position> medicineStash;

bool bulletFired = false;
bool grenadeThrown = false;
bool paused = true;
bool showSecurityMap = false;
bool setGridlines = false;
Bullet* pb = nullptr;
Grenade* pg = nullptr;



int maze[MSZ][MSZ] = { 0 }; // WALLs
int dupMaze[MSZ][MSZ] = { 0 }; // copy of maze
double security_map[MSZ][MSZ] = { 0 }; // security map
double dupSecurityMap[MSZ][MSZ] = { 0 }; // copy of security_map


void RestorePath(Cell* pc)
{
	int r, c;
	while (pc != nullptr)
	{
		r = pc->getRow();
		c = pc->getCol();
		if (maze[r][c] == WALL)
			maze[r][c] = SPACE;
		pc = pc->getParent();
	}
}

// row, col are the indices of neighbor cell
void AddNeighbor(int r, int c, Cell* pCurrent,
	priority_queue<Cell*, vector<Cell*>, CompareCells>& pq,
	vector <Cell>& grays, vector <Cell>& black) // blacks shouldn't be changed
{
	double newg, cost;
	vector<Cell>::iterator itGray;
	vector<Cell>::iterator itBlack;


	if (maze[r][c] == WALL) cost = WALL_COST;
	else if (maze[r][c] == SPACE) cost = SPACE_COST;
	else
	{
		Cell* temp = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(), 0, pCurrent, nullptr);
		black.push_back(*temp);
		return;
	}
	newg = pCurrent->getG() + cost;
	Cell* pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		newg, pCurrent, nullptr);
	// check what to do with the neighbor Cell
	// 1. if the neighbor is black: do nothing
	// 2. if the neighbor is white: add it to PQ and to grays
	// 3. if it is gray:
	// 3.1 if F of neighbor is not below the neighbor copy that is stored in PQ then do nothing
	// 3.2 otherwise then we must update the PQ and grays
	itGray = find(grays.begin(), grays.end(), *pNeighbor);
	itBlack = find(black.begin(), black.end(), *pNeighbor);

	if (itBlack == black.end()) // then it is not black
	{
		if (itGray == grays.end()) // then it is not gray => it is white
		{
			// paint it gray
			pq.push(pNeighbor);
			grays.push_back(*pNeighbor);
		}
		else // it is gray
		{
			if (pNeighbor->getF() < itGray->getF()) // then we found a better path and we have to exchange it
			{
				grays.erase(itGray);
				grays.push_back(*pNeighbor);

				// and do the same with PQ
				vector<Cell*> tmp;
				while (!pq.empty() && !((*pq.top()) == (*pNeighbor)))
				{
					tmp.push_back(pq.top()); // save the top element in tmp
					pq.pop(); // remove top element from pq
				}
				if (pq.empty()) // ERROR!
				{
					cout << "PQ is empty\n";
					exit(2);
				}
				else // we have found the neighbor cell in PQ
				{
					pq.pop(); // remove old neighbor from pq
					pq.push(pNeighbor);
					// now restore pq
					while (!tmp.empty())
					{
						pq.push(tmp.back());
						tmp.pop_back();
					}
				}
			}
		}
	}


}



// run A* from room at index1 to room at index2
void BuildPath(int index1, int index2)
{
	int r, c, tr, tc;

	r = rooms[index1]->getCenterY();
	c = rooms[index1]->getCenterX();
	tr = rooms[index2]->getCenterY();
	tc = rooms[index2]->getCenterX();
	Cell* pCurrent;
	Cell* start = new Cell(r, c, tr, tc, 0, nullptr, nullptr);
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	vector <Cell> grays;
	vector <Cell> black;
	vector<Cell>::iterator itGray;

	pq.push(start);
	grays.push_back(*start);
	// pq shouldn't be empty because we are going to reach the target beforehand
	while (!pq.empty())
	{
		pCurrent = pq.top();
		if (pCurrent->getH() <= 1) // this is a targt cell
		{
			RestorePath(pCurrent);
			return;
		}
		else // target hasn't been reached
		{
			// 1. remove pCurrent from pq 
			pq.pop();
			// 2. find and remove pCurrent from grays
			itGray = find(grays.begin(), grays.end(), *pCurrent);
			if (itGray == grays.end()) // pCurrent wasn't found
			{
				cout << "Error: pCurrent wasn't found in grays\n";
				exit(1);
			}
			grays.erase(itGray);
			// 3. paint pCurrent black
			black.push_back(*pCurrent);
			// 4. take care of neighbors
			r = pCurrent->getRow();
			c = pCurrent->getCol();
			// up
			if (r + 1 < MSZ)
				AddNeighbor(r + 1, c, pCurrent, pq, grays, black);
			// down
			if (r - 1 >= 0)
				AddNeighbor(r - 1, c, pCurrent, pq, grays, black);
			// left
			if (c - 1 >= 0)
				AddNeighbor(r, c - 1, pCurrent, pq, grays, black);
			// right
			if (c + 1 < MSZ)
				AddNeighbor(r, c + 1, pCurrent, pq, grays, black);
		}

	}
	cout << "Error pq is empty, target hasn't been found\n";
}

void BuildPathBetweenTheRooms()
{
	int i, j;

	for (i = 0; i < NUM_ROOMS; i++)
		for (j = i + 1; j < NUM_ROOMS; j++)
		{
			BuildPath(i, j); // A*
			cout << "The path from " << i << " to " << j << " has been paved\n";
		}
}

Position randomPositionInRoom(Room* room)
{
	int row = room->getCenterY() - room->getHeight() / 2 + (rand() % room->getHeight());
	int col = room->getCenterX() - room->getWidth() / 2 + (rand() % room->getWidth());
	Position p = { row, col };
	return p;
}

Position RandomRoomPlacement()
{
	// Randomly select a room
	int roomIndex = rand() % NUM_ROOMS;
	Room* room = rooms[roomIndex];

	// Randomly select a position within the room
	return randomPositionInRoom(room);
}

void placeResources()
{
	int placed = 0;
	while (placed < AMMUNITION_PACK_AMOUNT)
	{
		Position p = RandomRoomPlacement();
		if (maze[p.row][p.col] == SPACE || maze[p.row][p.col] == WALL)
		{
			maze[p.row][p.col] = AMMUNITION_PACK;
			ammunitionStash.push_back(p);
			placed++;
		}
	}

	placed = 0;
	while (placed < MEDICINE_PACK_AMOUNT)
	{
		Position p = RandomRoomPlacement();
		if (maze[p.row][p.col] == SPACE || maze[p.row][p.col] == WALL)
		{
			maze[p.row][p.col] = MEDICINE_PACK;
			medicineStash.push_back(p);
			placed++;
		}
	}
}

void initTeams()
{
	vector<array<double, 3>> colors1 = { team_colors.at("FRed"), team_colors.at("SRed") };
	vector<array<double, 3>> colors2 = { team_colors.at("FYellow"), team_colors.at("SYellow") };
	vector<vector<array<double, 3>>> colors = { colors1, colors2 };

	int r;
	for (int i = 0; i < TEAM_RANK; i++)
	{
		Team* t = new Team(colors.at(i));
		int roomIndex = rand() % NUM_ROOMS;

		if (i == 0)
			r = roomIndex;

		while (i > 0 && roomIndex == r)
			roomIndex = rand() % NUM_ROOMS;

		Room* room = rooms[roomIndex];
		Position p[TEAM_SIZE];
		for (int j = 0; j < TEAM_SIZE; j++) {
			p[j] = randomPositionInRoom(room);
			if (j > 0)
			{
				while (p[j].row == p[j - 1].row && p[j].col == p[j - 1].col)
					p[j] = randomPositionInRoom(room);
			}
			t->addTeammate(p[j], true);
		}
		Position pos = randomPositionInRoom(room);
		while ((pos.row == p[0].row && pos.col == p[0].col) || (pos.row == p[1].row && pos.col == p[1].col))
			pos = randomPositionInRoom(room);

		t->addTeammate(pos, false);
		Team::Teams.push_back(t);
	}
}

bool isCorridor(Position p)
{
	for (Corridor* c : Corridor::corridors)
	{
		if (c->isEntrance(p))
			return true;
	}
	return false;
}

void initCorridors()
{
	DuplicateMaze(maze, dupMaze);
	int sRow, sCol, eRow, eCol;
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		sRow = roomScopes[i].startRow;
		sCol = roomScopes[i].startCol;
		eRow = roomScopes[i].endRow;
		eCol = roomScopes[i].endCol;
		// up
		if (sRow - 1 >= 0)
		{
			for (int j = sCol; j <= eCol; j++)
			{
				if (maze[sRow - 1][j] == SPACE && GetRoomIndex({ sRow - 1, j }) == -1 && !isCorridor({ sRow - 1, j }))
				{
					Position sRoom = { sRow, j };
					Position sCorridor = { sRow - 1, j };
					if (GetRoomIndex(sRoom) == -1)
						std::cout << "Error: room not found\n";
					Corridor* corridor = new Corridor();
					corridor->CorridorRunBFS(sRoom, sCorridor, dupMaze);
					Corridor::corridors.push_back(corridor);
				}
			}
		}
		// down
		if (eRow + 1 < MSZ)
		{
			for (int j = sCol; j <= eCol; j++)
			{
				if (maze[eRow + 1][j] == SPACE && GetRoomIndex({ eRow + 1, j }) == -1 && !isCorridor({ eRow + 1, j }))
				{
					Position sRoom = { eRow, j };
					Position sCorridor = { eRow + 1, j };
					if (GetRoomIndex(sRoom) == -1)
						std::cout << "Error: room not found\n";
					Corridor* corridor = new Corridor();
					corridor->CorridorRunBFS(sRoom, sCorridor, dupMaze);
					Corridor::corridors.push_back(corridor);
				}
			}
		}
		// left
		if (sCol - 1 >= 0)
		{
			for (int j = sRow; j <= eRow; j++)
			{
				if (maze[j][sCol - 1] == SPACE && GetRoomIndex({ j, sCol - 1 }) == -1 && !isCorridor({ j, sCol - 1 }))
				{
					Position sRoom = { j, sCol };
					Position sCorridor = { j, sCol - 1 };
					if (GetRoomIndex(sRoom) == -1)
						std::cout << "Error: room not found\n";
					Corridor* corridor = new Corridor();
					corridor->CorridorRunBFS(sRoom, sCorridor, dupMaze);
					Corridor::corridors.push_back(corridor);
				}
			}
		}
		// right
		if (eCol + 1 < MSZ)
		{
			for (int j = sRow; j <= eRow; j++)
			{
				if (maze[j][eCol + 1] == SPACE && GetRoomIndex({ j, eCol + 1 }) == -1 && !isCorridor({ j, eCol + 1 }))
				{
					Position sRoom = { j, eCol };
					Position sCorridor = { j, eCol + 1 };
					if (GetRoomIndex(sRoom) == -1)
						std::cout << "Error: room not found\n";
					Corridor* corridor = new Corridor();
					corridor->CorridorRunBFS(sRoom, sCorridor, dupMaze);
					Corridor::corridors.push_back(corridor);
				}
			}
		}
	}
}

void PrintCorridors()
{
	for (size_t i = 0; i < Corridor::corridors.size(); ++i)
	{
		Corridor* corridor = Corridor::corridors[i];
		cout << "Corridor " << i + 1 << ":\n";
		cout << "Path: ";
		for (const Position& pos : corridor->getPath())
		{
			cout << "(" << pos.row << ", " << pos.col << ") ";
		}
		cout << "\nEntrances: ";
		for (const Position& entrance : corridor->getEntrances())
		{
			cout << "(" << entrance.row << ", " << entrance.col << ") ";
			// Check if the entrance is adjacent to a room
			bool isAdjacentToRoom = false;
			for (int r = entrance.row - 1; r <= entrance.row + 1; ++r)
			{
				for (int c = entrance.col - 1; c <= entrance.col + 1; ++c)
				{
					if (r >= 0 && r < MSZ && c >= 0 && c < MSZ && maze[r][c] == SPACE)
					{
						isAdjacentToRoom = true;
						break;
					}
				}
				if (isAdjacentToRoom) break;
			}
			if (!isAdjacentToRoom)
			{
				cout << " (Error: Not adjacent to a room)";
			}
		}
		cout << "\nConnected Rooms: ";
		for (int j = 0; j < corridor->getNumOfRooms(); ++j)
		{
			int roomIndex = corridor->getConnectedRoom(j);
			cout << roomIndex << " ";
			// Check if the room index is valid
			if (roomIndex < 0 || roomIndex >= NUM_ROOMS)
			{
				cout << "(Error: Invalid room index)";
			}
		}
		cout << "\n\n";
	}
}


void PrintRoomScopes()
{
	for (int i = 0; i < NUM_ROOMS; ++i)
	{
		RoomScope scope = roomScopes[i];
		cout << "Room " << i << " Scope:\n";

		// Print top edge
		cout << "Top Edge: ";
		for (int col = scope.startCol; col <= scope.endCol; ++col)
		{
			cout << "(" << scope.startRow << ", " << col << ") ";
		}
		cout << "\n";

		// Print bottom edge
		cout << "Bottom Edge: ";
		for (int col = scope.startCol; col <= scope.endCol; ++col)
		{
			cout << "(" << scope.endRow << ", " << col << ") ";
		}
		cout << "\n";

		// Print left edge
		cout << "Left Edge: ";
		for (int row = scope.startRow; row <= scope.endRow; ++row)
		{
			cout << "(" << row << ", " << scope.startCol << ") ";
		}
		cout << "\n";

		// Print right edge
		cout << "Right Edge: ";
		for (int row = scope.startRow; row <= scope.endRow; ++row)
		{
			cout << "(" << row << ", " << scope.endCol << ") ";
		}
		cout << "\n\n";
	}
}



void SetupDungeon()
{
	int i, j;
	int cx, cy, w, h;
	bool hasOverlap;

	for (i = 0; i < NUM_ROOMS; i++)
	{
		do
		{
			hasOverlap = false;
			w = 6 + rand() % (MSZ / 5);
			h = 6 + rand() % (MSZ / 5);
			cx = 2 + w / 2 + rand() % (MSZ - w - 4);
			cy = 2 + h / 2 + rand() % (MSZ - h - 4);
			for (j = 0; j < i && !hasOverlap; j++)
				hasOverlap = rooms[j]->Overlap(cx, cy, w, h);
		} while (hasOverlap); // check the validity of the room

		rooms[i] = new Room(cx, cy, w, h, maze);
		roomScopes[i] = { cy - h / 2, cy + h / 2, cx - w / 2, cx + w / 2 };
	}

	for (i = 0; i < 700; i++)
		maze[rand() % MSZ][rand() % MSZ] = WALL;

	initTeams();
	for (Team* t : Team::Teams)
	{
		for (NPC* n : t->GetTeammates())
		{
			Position p = n->GetPosition();
			maze[p.row][p.col] = NPC_;
		}
	}
	placeResources();
	isAstar = true;
	BuildPathBetweenTheRooms();
	isAstar = false;
	initCorridors();
	/*PrintCorridors();
	PrintRoomScopes();*/
}


void ShowDungeon()
{
	int i, j;
	double s;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			if (showSecurityMap) s = security_map[i][j];
			else s = 0;
			//1. set color of cell
			switch (maze[i][j])
			{
			case SPACE:
				glColor3d(1 - s, 1 - s, 1 - s); // gray
				break;
			case WALL:
				glColor3d(0.3, 0.3, 0.4); // dark gray
				break;
			case AMMUNITION_PACK:
				glColor3d(0, 0, 1); // Blue for ammunition
				break;
			case MEDICINE_PACK:
				glColor3d(0, 1, 0); // Green for medicine
				break;
			case NPC_:
				for (Team* t : Team::Teams) {
					for (NPC* n : t->GetTeammates()) {
						if (n->GetPosition().row == i && n->GetPosition().col == j) {
							if (!strcmp(n->getType(), "Warrior"))
								glColor3d(t->getWarriorColor()[0], t->getWarriorColor()[1], t->getWarriorColor()[2]);
							else
								glColor3d(t->getSquireColor()[0], t->getSquireColor()[1], t->getSquireColor()[2]);
						}

					}
				}
				break;
			}
			// show cell
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j + 1, i);
			glEnd();

			// set grid lines
			if (setGridlines) {
				glColor3d(0, 0, 0);
				glBegin(GL_LINE_LOOP);
				glVertex2d(j, i);
				glVertex2d(j, i + 1);
				glVertex2d(j + 1, i + 1);
				glVertex2d(j + 1, i);
				glEnd();
			}
		}
}

void GenerateSecurityMap()
{
	int numSimulations = 3000;
	int i;

	for (i = 0; i < numSimulations; i++)
	{
		Grenade* g = new Grenade(rand() % MSZ, rand() % MSZ);

		g->SimulateExplosion(maze, security_map);
		delete g;
		g = nullptr;
	}

}

void GenerateSecurityMapForSpecificNPC(NPC* n)
{
	DuplicateSecurityMap(security_map, dupSecurityMap);
	int roomIndex = n->getRoomIndex();
	if (roomIndex == -1)
		roomIndex = n->getPrevRoomIndex();
	vector<Position> enemiesPos = Team::GetEnemiesPositionsInRoom(roomIndex, n->GetTeamID().team, true);

	int numSimulations = 100;
	if (enemiesPos.size() > 0)
	{
		enemiesPos.push_back(n->GetPosition());
		for (Position p : enemiesPos)
		{
			for (int i = 0; i < numSimulations; i++)
			{
				Grenade* g = new Grenade(p.row, p.col);
				g->SimulateExplosion(maze, dupSecurityMap);
				delete g;
				g = nullptr;
			}
		}
	}
	/*else
	{
		vector<Position> entrances = n->GetAllEntrancesToMyRoom(roomIndex, true);
		entrances.push_back(n->GetPosition());
		for (Position p : entrances)
		{
			for (int i = 0; i < numSimulations; i++)
			{
				Grenade* g = new Grenade(p.row, p.col);
				g->SimulateExplosion(maze, dupSecurityMap);
				delete g;
				g = nullptr;
			}
		}
	}*/
}


void init()
{
	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(3);

	SetupDungeon();
	GenerateSecurityMap();
}

void renderBitmapString(float x, float y, void* font, const string& str) {
	glRasterPos2f(x, y);
	for (char c : str) {
		glutBitmapCharacter(font, c);
	}
}

void drawHUD()
{
	// a HUD for our convinenece
	float y_offset = MSZ - 2;
	float x_offset = 1;

	glColor3d(0, 0, 0);
	int soldierIndex = 1;

	for (Team* t : Team::Teams) {
		for (NPC* s : t->GetTeammates()) {
			stringstream ss;
			ss << " [" << (dynamic_cast<Warrior*>(s) ? "Warrior" : "Squire") << "] "
				<< "HP: " << s->GetHp() << " Ammo: " << s->getAmmo() << " State: " << s->GetState()->toString()
				<< " Team: " << ((t->GetTeamID().team == 0) ? "Red" : "Yellow");
			renderBitmapString(x_offset, y_offset, GLUT_BITMAP_HELVETICA_12, ss.str());

			y_offset -= 2; // Move down for next soldier
			if (y_offset < MSZ - 10) break; // Prevent overlapping
		}
		x_offset += MSZ / 2;
		y_offset = MSZ - 2;
		soldierIndex = 1;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowDungeon();
	drawHUD();

	if (pb != nullptr)
		pb->show();
	if (pg != nullptr)
		pg->show();
	for (Bullet* b : Bullet::bullets)
		b->show();
	for (Grenade* g : Grenade::grenades)
		g->show();

	glutSwapBuffers(); // show all
}

template <typename T>
void safe_delete(T*& ptr) {
	if (ptr) {
		delete ptr;
		ptr = nullptr;
	}
}

void idle()
{
	Sleep(150);
	if (bulletFired)
		pb->move(maze);
	if (grenadeThrown)
		pg->expand(maze);
	if (!paused)
	{
		// move all NPCs
		for (Team* t : Team::Teams)
		{
			for (NPC* n : t->GetTeammates())
			{
				GenerateSecurityMapForSpecificNPC(n);
				cout << " [" << (dynamic_cast<Warrior*>(n) ? "Warrior" : "Squire") << "] "
					<< "HP: " << n->GetHp() << " Ammo: " << n->getAmmo() << " Grenades: " << n->getGrenades() << " State: " << n->GetState()->toString()
					<< " Team: " << ((t->GetTeamID().team == 0) ? "Red " : "Yellow ");
				if (dynamic_cast<Warrior*>(n))
				{
					Warrior* w = (Warrior*)n;
					cout << "Target: " << (w->GetTarget() != nullptr ? w->GetTarget()->GetTeamID().place : -1) << "\n";
				}
				else
					cout << "\n";
				n->GetState()->OnEnter(n);
				n->SetPrevStep((Team::isAnyBodyInMyPosition(n->GetPosition(), n->GetTeamID().team, n->GetTeamID().place) ? NPC_ : SPACE));
			}
		}

		// move all bullets and grenades and check if a bullet or grenade attribute is moving == false remove it from the vector and delete him
		for (Bullet* b : Bullet::bullets)
		{
			b->move(maze);
			/*if (!b->getIsMoving())
			{
				Bullet::bullets.erase(std::remove(Bullet::bullets.begin(), Bullet::bullets.end(), b), Bullet::bullets.end());
				delete b;
				b = nullptr;
			}*/
		}
		for (Grenade* g : Grenade::grenades)
		{
			g->expand(maze);
			/*if (!g->getIsExpending())
			{
				Grenade::grenades.erase(std::remove(Grenade::grenades.begin(), Grenade::grenades.end(), g), Grenade::grenades.end());
				delete g;
				g = nullptr;
			}*/
		}

		Bullet::bullets.erase(
			remove_if(Bullet::bullets.begin(), Bullet::bullets.end(), [](Bullet* b) {
				if (!b->getIsMoving()) {
					delete b;
					b = nullptr;
					return true;
				}
				return false;
				}),
			Bullet::bullets.end()
		);
		Grenade::grenades.erase(
			remove_if(Grenade::grenades.begin(), Grenade::grenades.end(), [](Grenade* g) {
				if (!g->getIsExpending()) {
					delete g;
					g = nullptr;
					return true;
				}
				return false;
				}),
			Grenade::grenades.end()
		);
		// check if anyone is dead
		for (Team* t : Team::Teams)
		{
			for (NPC* n : t->GetTeammates())
			{
				if (!n->GetIsAlive())
				{
					t->removeTeammate(n);
					safe_delete(n);
					/*delete n;
					n = nullptr;*/
				}
			}
			if (t->GetTeammates().size() == 0)
			{
				paused = true;
				cout << "Game Over! Team " << ((t->GetTeamID().team == 0) ? "Yellow" : "Red") << " is won!\n";
			}
		}
	}
	glutPostRedisplay(); // indirect call to display
}

void menu(int choice)
{
	switch (choice)
	{
	case 1: // fire bullet
		bulletFired = true;
		pb->setIsMoving(true);
		break;
	case 2: // throw grenade
		grenadeThrown = true;
		pg->explode();
		break;
	case 3: // security map
		GenerateSecurityMap();
		break;
	}
}
//void mouse(int button, int state, int x, int y)
//{
//	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
//	{
////		pb = new Bullet(MSZ*x/(double)WIDTH,
////			MSZ* (HEIGHT - y) / (double)HEIGHT, (rand() % 360)* PI / 180);
//
//		pg = new Grenade(MSZ * (HEIGHT - y) / (double)HEIGHT, MSZ * x / (double)WIDTH);
//	}
//}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'g')
		setGridlines = !setGridlines;		 // toggle gridlines
	if (key == ' ')
		paused = !paused;
	if (key == 's')
		showSecurityMap = !showSecurityMap;
	if (key == 'q')
		exit(0);
}
LONG MyFilter(LONG excode) {
	return (excode == EXCEPTION_ACCESS_VIOLATION) ?
		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char* argv[])
{
	__try
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		glutInit(&argc, argv);
		// definitions for visual memory (Frame buffer) and double buffer
		glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
		glutInitWindowSize(WIDTH, HEIGHT);
		glutInitWindowPosition(600, 20);
		glutCreateWindow("BFS");

		// display is a refresh function
		glutDisplayFunc(display);
		// idle is a update function
		glutIdleFunc(idle);

		//glutMouseFunc(mouse);
		glutKeyboardFunc(keyboard);

		// menu
		/*glutCreateMenu(menu);
		glutAddMenuEntry("Fire bullet", 1);
		glutAddMenuEntry("Throw Grenade", 2);
		glutAddMenuEntry("Generate Security Map", 3);
		glutAttachMenu(GLUT_RIGHT_BUTTON);*/


		init();

		glutMainLoop();

	}
	__except (MyFilter(GetExceptionCode())) {
		std::cerr << "Access violation caught!" << std::endl;
		// Handle the exception (e.g., clean up resources)
	}

	return 0;
}