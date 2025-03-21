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
