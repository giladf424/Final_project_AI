#include "Cell.h"
#include <math.h>
bool isAstar = false;

Cell::Cell()
{
	parent = nullptr;
	col = 0;
	row = 0;
	target_col = 0;
	target_row = 0;
	h = 0;
	g = 0;
	f = 0;
}

// gets row, col, parent
Cell::Cell(int row, int col, Cell* p, double sec_map[MSZ][MSZ])
{
	this->row = row;
	this->col = col;
	target_col = 0;
	target_row = 0;
	parent = p;
	g = 0;
	ComputeH(sec_map);
	f = h + g;
}

Cell::Cell(int r, int c, int tr, int tc, Cell* p)
{
	row = r;
	col = c;
	target_row = tr;
	target_col = tc;
	parent = p;
	g = 0;
	f = 0;
	CalcH(nullptr, {});
}

// gets row, col, target row, target col, parent g and parent itself
Cell::Cell(int r, int c, int tr, int tc, double newg, Cell* p, double sec_map[MSZ][MSZ])
{
	row = r;
	col = c;
	parent = p;
	target_row = tr;
	target_col = tc;
	g = newg;
	//DuplicateSecurityMap(sec_map, security_map);
	ComputeH(sec_map);
	f = h + g;
}

// h is computed basing on Manhattan distance
void Cell::ComputeH(double sec_map[MSZ][MSZ])
{
	if (sec_map == nullptr)
		h = abs(row - target_row) + abs(col - target_col);
	else if (target_col == 0 && target_row == 0)
	{
		double security = sec_map[row][col];
		h = security * SECURITY_COEFFICIENT;
	}
	else
	{
		double distance = sqrt(pow(row - target_row, 2) + pow(col - target_col, 2));
		double security = sec_map[row][col];
		h = distance + security * SECURITY_COEFFICIENT;
	}
}

void Cell::CalcH(double sec_map[MSZ][MSZ], vector<Position> positions)
{
	if (sec_map == nullptr)
		h = sqrt(pow(row - target_row, 2) + pow(col - target_col, 2));
	else
	{
		for (Position p : positions)
		{
			h += sqrt(pow(row - p.row, 2) + pow(col - p.col, 2));
		}
		double security = sec_map[row][col];
		h -= security * SECURITY_COEFFICIENT;
	}
}
