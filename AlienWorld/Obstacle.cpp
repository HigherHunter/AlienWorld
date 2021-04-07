#include "Obstacle.h"
#include "OpenGl.h"

Obstacle::Obstacle(double x, double y, double r) : BaseGameEntity(0, Vector2D(x, y), r)
{
	image = LoadTexture((char*)"Assets\\Sprites\\Obstacle\\rock.png");
	box.SetMin(Vector2D(GetPos().x - (GetBRadius() - GetBRadius() / 5), GetPos().y - (GetBRadius() - GetBRadius() / 5)));
	box.SetMax(Vector2D(GetPos().x + (GetBRadius() - GetBRadius() / 5), GetPos().y + (GetBRadius() - GetBRadius() / 5)));
}

Obstacle::Obstacle(Vector2D pos, double radius) : BaseGameEntity(0, pos, radius)
{
	image = LoadTexture((char*)"Assets\\Sprites\\Obstacle\\rock.png");
	box.SetMin(Vector2D(GetPos().x - (GetBRadius() - GetBRadius() / 5), GetPos().y - (GetBRadius() - GetBRadius() / 5)));
	box.SetMax(Vector2D(GetPos().x + (GetBRadius() - GetBRadius() / 5), GetPos().y + (GetBRadius() - GetBRadius() / 5)));
}

void Obstacle::Render()
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Reset color
	glColor3f(1.0f, 1.0f, 1.0f); // white

	glBindTexture(GL_TEXTURE_2D, image); // bind image

	glPushMatrix();

	// Apply texture
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(GetPos().x - GetBRadius(), GetPos().y - GetBRadius());
	glTexCoord2f(1.0f, 1.0f); glVertex2f(GetPos().x + GetBRadius(), GetPos().y - GetBRadius());
	glTexCoord2f(1.0f, 0.0f); glVertex2f(GetPos().x + GetBRadius(), GetPos().y + GetBRadius());
	glTexCoord2f(0.0f, 0.0f); glVertex2f(GetPos().x - GetBRadius(), GetPos().y + GetBRadius());
	glEnd();

	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}