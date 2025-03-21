
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Grenade.h"
#include "definitions.h"
#include "Team.h"

using namespace std;

const int WIDTH = 700;
const int HEIGHT = 700;

const int NUM_ROOMS = 12;

Room* rooms[NUM_ROOMS];

bool bulletFired = false;
bool grenadeThrown = false;
Bullet* pb=nullptr;
Grenade* pg = nullptr;



int maze[MSZ][MSZ] = { 0 }; // WALLs
double security_map[MSZ][MSZ] = {0}; // 


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
	vector <Cell>& grays,  vector <Cell> &black) // blacks shouldn't be changed
{
	double newg,cost;
	vector<Cell>::iterator itGray;
	vector<Cell>::iterator itBlack;


	if (maze[r][c] == WALL) cost = WALL_COST;
	else cost = SPACE_COST;
	newg = pCurrent->getG() + cost;
	Cell* pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		newg, pCurrent);
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
				while (!pq.empty() &&  !((*pq.top()) == (*pNeighbor)))
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
	Cell* start = new Cell(r,c ,tr ,tc , 0, nullptr);
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
		if (pCurrent->getH() < 0.001) // this is a targt cell
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
			if (r - 1 >=0)
				AddNeighbor(r - 1, c, pCurrent, pq, grays, black);
			// left
			if (c - 1 >= 0)
				AddNeighbor(r , c-1, pCurrent, pq, grays, black);
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

	for (i = 0;i < NUM_ROOMS;i++)
		for (j = i + 1;j < NUM_ROOMS;j++)
		{
			BuildPath(i, j); // A*
			cout << "The path from " << i << " to " << j << " has been paved\n";
		}
}

Position RandomRoomPlacement()
{
	// Randomly select a room
	int roomIndex = rand() % NUM_ROOMS;
	Room* room = rooms[roomIndex];

	// Randomly select a position within the room
	int row = room->getCenterY() - room->getHeight() / 2 + (rand() % room->getHeight());
	int col = room->getCenterX() - room->getWidth() / 2 + (rand() % room->getWidth());
	Position p = { row, col };

	return p;
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

		if(i == 0)
			r = roomIndex;

		while (i > 0 && roomIndex == r)
			roomIndex = rand() % NUM_ROOMS;

		Room* room = rooms[roomIndex];
		
		for (int j = 0; j < TEAM_SIZE; j++) {
			int row = room->getCenterY() - room->getHeight() / 2 + (rand() % room->getHeight());
			int col = room->getCenterX() - room->getWidth() / 2 + (rand() % room->getWidth());
			Position p = { row , col };
			t->addTeammate(p, true);
		}
		t->addTeammate(Position{
			room->getCenterY() - room->getHeight() / 2 + (rand() % room->getHeight()),
			room->getCenterX() - room->getWidth() / 2 + (rand() % room->getWidth())
			},
			false);
		Team::Teams.push_back(t);
	}
}

void SetupDungeon()
{
	int i,j;
	int cx, cy, w, h;
	bool hasOverlap;

	for (i = 0;i < NUM_ROOMS;i++)
	{
		do
		{
			hasOverlap = false;
			w = 6 + rand() % (MSZ / 5);
			h = 6 + rand() % (MSZ / 5);
			cx = 2 + w / 2 + rand() % (MSZ - w - 4);
			cy = 2 + h / 2 + rand() % (MSZ - h - 4);
			for(j=0;j<i && !hasOverlap;j++)
				hasOverlap = rooms[j]->Overlap(cx, cy, w, h);
		} while (hasOverlap); // check the validity of the room
			
		rooms[i] = new Room(cx, cy, w, h,maze);
	}

	for (i = 0;i < 700;i++)
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
	BuildPathBetweenTheRooms();
}

void init()
{
	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	SetupDungeon();
}

void ShowDungeon()
{
	int i, j;
	double s;

	for(i=0;i<MSZ;i++)
		for (j = 0;j < MSZ;j++)
		{
			s = security_map[i][j];
			//1. set color of cell
			switch (maze[i][j])
			{
			case SPACE:
				glColor3d(1-s, 1-s, 1-s); // gray
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
			glVertex2d(j+1, i + 1);
			glVertex2d(j + 1, i );
			glEnd();
		}
}

void GenerateSecurityMap()
{
	int numSimulations = 1000;
	int i;

	for (i = 0;i < numSimulations;i++)
	{
		Grenade* g = new Grenade(rand() % MSZ, rand() % MSZ);

		g->SimulateExplosion(maze, security_map);
	}

}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowDungeon();
	if (pb != nullptr)
		pb->show();
	if (pg != nullptr)
		pg->show();

	glutSwapBuffers(); // show all
}

void idle() 
{
	if (bulletFired)
		pb->move(maze);
	if (grenadeThrown)
		pg->expand(maze);
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
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
//		pb = new Bullet(MSZ*x/(double)WIDTH,
//			MSZ* (HEIGHT - y) / (double)HEIGHT, (rand() % 360)* PI / 180);

		pg = new Grenade(MSZ * (HEIGHT - y) / (double)HEIGHT, MSZ * x / (double)WIDTH);
	}
}
void main(int argc, char* argv[]) 
{
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
	
	glutMouseFunc(mouse);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Fire bullet", 1);
	glutAddMenuEntry("Throw Grenade", 2);
	glutAddMenuEntry("Generate Security Map", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop();
}