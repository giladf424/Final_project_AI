#include "Corridor.h"

vector<Corridor*> Corridor::corridors;
Corridor::Corridor()
{

}

Corridor::Corridor(vector<Position> p, Position e[MAX_CORRIDORS_ENTRANCES], int c[MAX_CORRIDORS_ENTRANCES])
{
	path = p;
	for (int i = 0; i < MAX_CORRIDORS_ENTRANCES; i++)
	{
		entrances[i] = e[i];
		connectedRooms[i] = c[i];
	}
}

void Corridor::FindAllCorridors()
{
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
				if (maze[sRow - 1][j] == SPACE && GetRoomIndex({ sRow - 1, j }) == -1)
				{
					Position sRoom = { sRow, j };
					Position sCorridor = { sRow - 1, j };
					if (GetRoomIndex(sRoom) == -1)
						std::cout << "Error: room not found\n";
				}
			}
		}
	}
}

void Corridor::CorridorRunBFS(Position sRoom, Position sCorridor)
{
	priority_queue<Cell*, vector<Cell*>, CompareCells> grays;
	Cell* pc = new Cell(sCorridor.row, sCorridor.col, sRoom.row, sRoom.col, nullptr);
	grays.push(pc);
	Cell* nextStep = nullptr;
	Corridor* corridor = new Corridor();
	corridor->setEntrance(sCorridor, 0);
	corridor->setConnectedRoom(GetRoomIndex(sRoom), 0);
	vector<Position> path;

}
