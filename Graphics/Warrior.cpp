#include "Warrior.h"

Warrior::Warrior(Position startPos, TeamID teamID) : NPC(startPos, teamID)
{
	bullets = MAX_BULLETS_WARRIOR;
	grenades = MAX_GRENADES_WARRIOR;
	aggressive = (rand() % 2 == 0);
}

std::vector<Cell*> Warrior::RunAStar(int targetRow, int targetCol, int maze[MSZ][MSZ])
{
    priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
    vector<Cell> grays;
    vector<Cell> black;

    Cell* start = new Cell(pos.row, pos.col, targetRow, targetCol, 0, nullptr, nullptr);
    pq.push(start);
    grays.push_back(*start);

    while (!pq.empty()) {
        Cell* pCurrent = pq.top();
        if (pCurrent->getRow() == targetRow && pCurrent->getCol() == targetCol) {
            // Path found, return the path
            vector<Cell*> path;
            while (pCurrent != nullptr) {
                path.push_back(pCurrent);
                pCurrent = pCurrent->getParent();
            }
            reverse(path.begin(), path.end()); // Reverse to get the path from start to target
            return path;
        }
        pq.pop();

        // Add neighbors to the priority queue
        int r = pCurrent->getRow();
        int c = pCurrent->getCol();

        // Up
        if (r + 1 < MSZ) AddNeighbor(r + 1, c, pCurrent, pq, grays, black, maze);
        // Down
        if (r - 1 >= 0) AddNeighbor(r - 1, c, pCurrent, pq, grays, black, maze);
        // Left
        if (c - 1 >= 0) AddNeighbor(r, c - 1, pCurrent, pq, grays, black, maze);
        // Right
        if (c + 1 < MSZ) AddNeighbor(r, c + 1, pCurrent, pq, grays, black, maze);
    }

    return {}; // No path found
}

void Warrior::AddNeighbor(int r, int c, Cell* pCurrent, priority_queue<Cell*, vector<Cell*>, CompareCells>& pq, vector<Cell>& grays, vector<Cell>& black, int maze[MSZ][MSZ])
{
    double newg, cost;
    vector<Cell>::iterator itGray;
    vector<Cell>::iterator itBlack;

    if (maze[r][c] == WALL) cost = WALL_COST;
    else cost = SPACE_COST;

    newg = pCurrent->getG() + cost;
    Cell* pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(), newg, pCurrent, nullptr);

    itGray = find(grays.begin(), grays.end(), *pNeighbor);
    itBlack = find(black.begin(), black.end(), *pNeighbor);

    if (itBlack == black.end()) { // Not black
        if (itGray == grays.end()) { // Not gray => white
            pq.push(pNeighbor);
            grays.push_back(*pNeighbor);
        }
        else if (pNeighbor->getF() < itGray->getF()) { // Gray and better path
            grays.erase(itGray);
            grays.push_back(*pNeighbor);

            vector<Cell*> tmp;
            while (!pq.empty() && !((*pq.top()) == (*pNeighbor))) {
                tmp.push_back(pq.top());
                pq.pop();
            }
            if (!pq.empty()) {
                pq.pop();
                pq.push(pNeighbor);
            }
            while (!tmp.empty()) {
                pq.push(tmp.back());
                tmp.pop_back();
            }
        }
    }
}

void Warrior::move(int maze[MSZ][MSZ])
{

}
