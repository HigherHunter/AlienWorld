#include "Path.h"
#include "OpenGl.h"

void Path::AddWayPoint(Vector2D new_point)
{
	this->m_WayPoints.emplace_back(new_point);
	if (m_WayPoints.size() == 1)
	{
		curWaypoint = m_WayPoints.begin();
	}
}

void Path::SetNextWaypoint()
{
	assert(m_WayPoints.size() > 0);
	if (++curWaypoint == m_WayPoints.end())
	{
		if (m_bLooped)
		{
			curWaypoint = m_WayPoints.begin();
		}
	}
}

void Path::ReversePath()
{
	m_WayPoints.reverse();
	curWaypoint = m_WayPoints.begin();
}

void Path::Render()const
{
	std::list<Vector2D>::const_iterator it = m_WayPoints.begin();

	Vector2D firstPoint;
	Vector2D wp = *it++;

	while (it != m_WayPoints.end())
	{
		firstPoint = wp;
		wp = *it++;
		// Line
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f); // Red
		glVertex2f(firstPoint.x, firstPoint.y);
		glVertex2f(wp.x, wp.y);
		glEnd();
	}

	if (m_bLooped)
	{
		firstPoint = *m_WayPoints.begin();
		// Line
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f); // Red
		glVertex2f(wp.x, wp.y);
		glVertex2f(firstPoint.x, firstPoint.y);
		glEnd();
	}
}

//--------------------A*Functions-----------------------------
void Path::SetupMap(std::map<std::pair<int, int>, int> gameMap)
{
	map = gameMap;

	// We set the ground tiles to be zero
	// We also set obstacles to be 1
	// The start is set to 8 and goal is set to 9
	for (unsigned int i = 0; i < TILE_COUNT_WIDTH; ++i)
	{
		for (unsigned int j = 0; j < TILE_COUNT_HEIGHT; ++j)
		{
			if (map[{i, j}] == start)
			{
				start_x = i;
				start_y = j;
			}
			else if (map[{i, j}] == goal)
			{
				goal_x = i;
				goal_y = j;
			}
		}
	}
}

void Path::InsertIntoOpenList(int x, int y)
{
	// Node must be inside the boundary of the map
	if (x < TILE_COUNT_WIDTH && y < TILE_COUNT_HEIGHT && closedList.size() > 0 && !IsNodePresent(x, y))
	{
		NODE tempNode;
		// The tile must be ground tile, so it will be passable
		if (map[{x, y}] == ground)
		{
			int base_cost, cost_to_start, cost_to_goal;

			base_cost = map[{x, y}];
			cost_to_goal = abs(x - goal_x) + abs(y - goal_y);
			cost_to_start = closedList.back().startCost + base_cost;

			tempNode.startCost = cost_to_start;
			tempNode.totalCost = base_cost + cost_to_goal + cost_to_start;
			tempNode.x = x;
			tempNode.y = y;
			tempNode.parentX = closedList.back().x;
			tempNode.parentY = closedList.back().y;

			openList.push_back(tempNode);
		}
		else if (map[{x, y}] == goal)
		{
			tempNode.startCost = 0;
			tempNode.totalCost = 0;
			tempNode.x = x;
			tempNode.y = y;
			tempNode.parentX = closedList.back().x;
			tempNode.parentY = closedList.back().y;

			openList.push_back(tempNode);
		}
	}
}

void Path::InsertIntoClosedList(int total_cost, int start_cost, int x, int y)
{
	// Node must be inside the boundary of the map
	if (x < TILE_COUNT_WIDTH && y < TILE_COUNT_HEIGHT && !IsNodePresent(x, y))
	{
		NODE tempNode;

		tempNode.startCost = start_cost;
		tempNode.totalCost = total_cost;
		tempNode.x = x;
		tempNode.y = y;
		tempNode.parentX = x;
		tempNode.parentY = y;
		closedList.push_back(tempNode);
	}
}

void Path::Create()
{
	// Clear previous lists
	ClearAll();

	// Add the start node to the closed list
	InsertIntoClosedList(0, 0, start_x, start_y);

	int next_x = start_x;
	int next_y = start_y;
	while (map[{next_x, next_y}] != goal && next_x < TILE_COUNT_WIDTH && next_x >= 0 && next_y < TILE_COUNT_HEIGHT && next_y >= 0)
	{
		// Take all the neighboaring passable nodes and add them to the open list
		InsertIntoOpenList(next_x + 1, next_y);
		InsertIntoOpenList(next_x + 1, next_y + 1);
		InsertIntoOpenList(next_x, next_y + 1);
		InsertIntoOpenList(next_x - 1, next_y + 1);
		InsertIntoOpenList(next_x - 1, next_y);
		InsertIntoOpenList(next_x - 1, next_y - 1);
		InsertIntoOpenList(next_x, next_y - 1);
		InsertIntoOpenList(next_x + 1, next_y - 1);

		// Find the min node
		if (MinNode(openList).startCost != -1)
		{
			NODE minNode = MinNode(openList);

			closedList.push_back(minNode);

			next_x = minNode.x;
			next_y = minNode.y;

			// Delete the min node from the open list
			openList = DeleteElement(openList, next_x, next_y);
		}
	}
}

std::vector<NODE> Path::BackTrack()
{
	std::vector<NODE> pathL;

	for (int i = closedList.size() - 1; i > -1; i--)
	{
		NODE tempNode;
		tempNode = GetNodeAt(closedList[i].parentX, closedList[i].parentY);
		if (tempNode.totalCost != -1)
			pathL.push_back(tempNode);
	}
	return pathL;
}

NODE Path::MinNode(std::vector<NODE> nodesList)
{
	NODE minNode;
	if (nodesList.size() > 0)
	{
		minNode = nodesList[0];
		for (unsigned int i = 1; i < nodesList.size(); i++)
		{
			if (nodesList[i].totalCost < minNode.totalCost)
				minNode = nodesList[i];
		}
		return minNode;
	}
	else
	{
		// Return an error
		minNode.startCost = -1;
		minNode.totalCost = -1;
		minNode.x = 0;
		minNode.y = 0;

		return minNode;
	}
}

bool Path::IsNodePresent(int x, int y)
{
	// Check if the element exists in open or closed list or not
	for (unsigned int i = 0; i < openList.size(); i++)
	{
		if (openList[i].x == x && openList[i].y == y)
		{
			return true;
		}
	}
	for (unsigned int i = 0; i < closedList.size(); i++)
	{
		if (closedList[i].x == x && closedList[i].y == y)
		{
			return true;
		}
	}
	return false;
}

NODE Path::GetNodeAt(int x, int y)
{
	NODE tempNode;
	tempNode.totalCost = -1;
	for (unsigned int i = 0; i < closedList.size(); i++)
	{
		if (closedList[i].x == x && closedList[i].y == y)
		{
			tempNode = closedList[i];
		}
	}
	return tempNode;
}

std::vector<NODE> Path::DeleteElement(std::vector<NODE> nodesList, int x, int y)
{
	std::vector<NODE> tempL = nodesList;

	for (unsigned int i = 0; i < tempL.size(); i++)
	{
		if (tempL[i].x == x && tempL[i].y == y)
		{
			tempL.erase(tempL.begin() + i);
			break;
		}
	}
	return tempL;
}

void Path::ClearAll()
{
	openList.erase(openList.begin(), openList.begin() + openList.size());
	closedList.erase(closedList.begin(), closedList.begin() + closedList.size());
}