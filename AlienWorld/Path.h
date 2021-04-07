#ifndef PATH_H
#define PATH_H

#include "Vector2D.h"
#include "Node.h"

class Path
{
private:

	// A container with all point of path
	std::list<Vector2D> m_WayPoints;

	// Points to the current waypoint
	std::list<Vector2D>::iterator curWaypoint;

	// Flag to indicate if the path should be looped (The last waypoint connected to the first)
	bool m_bLooped;

	// A* variables
	std::vector<NODE> openList, closedList;
	std::map<std::pair<int, int>, int> map;
	int start_x, start_y, goal_x, goal_y;
	enum { ground = 0, obstacle = 1, start = 8, goal = 9};

public:

	Path() :m_bLooped(false) {}

	// Returns the current waypoint
	Vector2D GetCurrentWaypoint()const { return *curWaypoint; }

	// Returns true if the end of the list has been reached
	bool IsFinished() { return curWaypoint == m_WayPoints.end(); }

	// Returns true if the current waypoint is also the last one
	bool IsLastPoint()
	{
		std::list<Vector2D>::const_iterator i = m_WayPoints.end();
		--i;
		return curWaypoint == i;
	}

	void LoopOn() { m_bLooped = true; }
	void LoopOff() { m_bLooped = false; }
	bool IsLooped() { return m_bLooped; }

	// Adds a waypoint to the end of the path
	void AddWayPoint(Vector2D new_point);

	// Moves the iterator on to the next waypoint in the list
	void SetNextWaypoint();

	// Methods for setting the path with either another Path or a list of vectors
	void Set(std::list<Vector2D> new_path) { m_WayPoints = new_path; curWaypoint = m_WayPoints.begin(); }
	void Set(const Path& path) { m_WayPoints = path.GetPath(); curWaypoint = m_WayPoints.begin(); }

	// Reverses the list with waypoints, so that first one is last and last is first
	void ReversePath();

	// Clears waypoints
	void Clear() { m_WayPoints.clear(); }

	// Return waypoints of the path
	std::list<Vector2D> GetPath()const { return m_WayPoints; }

	// Renders the path
	void Render()const;

	// A* functions
	void SetupMap(std::map<std::pair<int, int>, int> gameMap);
	void InsertIntoOpenList(int x, int y);
	void InsertIntoClosedList(int total_cost, int start_cost, int x, int y);
	void Create();
	std::vector<NODE> BackTrack();
	NODE MinNode(std::vector<NODE>);
	bool IsNodePresent(int x, int y);
	NODE GetNodeAt(int x, int y);
	std::vector<NODE> DeleteElement(std::vector<NODE> nodesList, int x, int y);
	void ClearAll();
};

#endif // !PATH_H