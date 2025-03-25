#pragma once
#include "definitions.h"
class Cell
{
private:
	int row, col;
	int target_row, target_col;
	Cell* parent;
	double f, g, h;
public:
	Cell();
	Cell(int row, int col, Cell* p, double sec_map[MSZ][MSZ]);
	Cell(int r, int c,int tr,int tc, double newg,Cell* p, double sec_map[MSZ][MSZ]);
	int getRow() { return row; }
	int getCol() { return col; }
	Cell* getParent() { return parent; }
	void ComputeH(double sec_map[MSZ][MSZ]);

	double getF() { return f; }
	double getH() { return h; }
	double getG() { return g; }
	int getTargetRow() { return target_row; }
	int getTargetCol() { return target_col; }

	bool operator == (const Cell& other) { return row == other.row && col == other.col; }

};
