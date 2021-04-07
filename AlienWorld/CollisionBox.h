#ifndef COLLISION_BOX_H
#define COLLISION_BOX_H

#include "Vector2D.h"
#include "OpenGl.h"

class CollisionBox
{
private:

	// min - left down spot, max - right upper spot
	Vector2D min, max;

public:

	void SetMin(Vector2D new_min) { min = new_min; }
	Vector2D GetMin()const { return min; }

	void SetMax(Vector2D new_max) { max = new_max; }
	Vector2D GetMax()const { return max; }

	void Render()
	{
		glBegin(GL_QUADS);
		glColor3f(0, 0, 0);
		glVertex2f(min.x , min.y);
		glVertex2f(max.x, min.y);
		glVertex2f(max.x, max.y);
		glVertex2f(min.x, max.y);
		glEnd();
	}
};

#endif // !COLLISION_BOX_H
