#pragma once
#include "Cell.h"

class CompareCells
{
public:
	bool operator () (Cell* pc1, Cell* pc2)
	{
		if (isAstar)
			return pc1->getF() > pc2->getF();
		else
			return pc1->getH() < pc2->getH();
	}
};

