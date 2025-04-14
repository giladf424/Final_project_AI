#include "definitions.h"

void DuplicateMaze(int source[MSZ][MSZ], int target[MSZ][MSZ])
{
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			target[i][j] = source[i][j];
}

void DuplicateSecurityMap(double source[MSZ][MSZ], double target[MSZ][MSZ])
{
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			target[i][j] = source[i][j];
}

//void UpdateSecurityMap(vector<Position> positions, int dupMaze[MSZ][MSZ], double dupMap[MSZ][MSZ])
//{
//	for (Position p : positions)
//	{
//		Grenade* g = new Grenade(p.row, p.col);
//		g->SimulateExplosion(dupMaze, dupMap);
//		delete g;
//		g = nullptr;
//	}
//}

int GetRoomIndex(Position pos)
{
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		if (pos.row >= roomScopes[i].startRow && pos.row <= roomScopes[i].endRow && pos.col >= roomScopes[i].startCol && pos.col <= roomScopes[i].endCol)
			return i;
	}
	return -1;
}

void printMaze(int maze[MSZ][MSZ])
{
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			if (maze[i][j] == WALL)
				std::cout << "#";
			else if (maze[i][j] == SPACE)
				std::cout << ".";
			else if (maze[i][j] == NPC_)
				std::cout << "N";
			else if (maze[i][j] == GRAY)
				std::cout << "G";
			else if (maze[i][j] == BLACK)
				std::cout << "B";
			else
				std::cout << " ";
		}
		std::cout << "\n";
	}
}