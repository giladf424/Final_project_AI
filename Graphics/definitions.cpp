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

void setSecurityMapToZero(double map[MSZ][MSZ])
{
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			map[i][j] = 0;
}

int GetRoomIndex(Position pos)
{
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		if (pos.row >= roomScopes[i].startRow && pos.row <= roomScopes[i].endRow && pos.col >= roomScopes[i].startCol && pos.col <= roomScopes[i].endCol)
			return i;
	}
	return -1;
}

void printSecurityMap(double map[MSZ][MSZ])
{
	vector<HitDetails> hitDetails;
	double maxVal = 0;
	std::cout << "Security Map: ################################################################################################\n\n";
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			if (map[i][j] > maxVal)
				maxVal = map[i][j];
			if (map[i][j] >= 1)
			{
				hitDetails.push_back({ {i, j}, map[i][j] });
			}
		}
	}
	std::cout << "Hit Details:\n";
	for (const auto& hit : hitDetails)
	{
		std::cout << "Position: (" << hit.hitPos.row << ", " << hit.hitPos.col << "), Value: " << hit.hitAngle << "\n";
	}
	std::cout << "Max Value: " << maxVal << "\n\n";
}
