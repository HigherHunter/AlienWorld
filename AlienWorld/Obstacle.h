#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "BaseGameEntity.h"

class Obstacle : public BaseGameEntity
{
private:

	// Static image of an obstacle
	GLuint image;

public:

	Obstacle() {}

	Obstacle(double x, double y, double r);

	Obstacle(Vector2D pos, double radius);

	virtual ~Obstacle() {}

	void Render();
};

#endif // !OBSTACLE_H

