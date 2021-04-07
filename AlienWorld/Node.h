#ifndef NODE_H
#define NODE_H

// Node used in path creation and A* search
struct NODE
{
	int totalCost;
	int x, y;
	int startCost;
	int parentX, parentY;
};

#endif // !NODE_H
