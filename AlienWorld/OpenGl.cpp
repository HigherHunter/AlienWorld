#include "OpenGl.h"

GLuint LoadTexture(char *fileName)
{
	GLuint ID;
	GLubyte *pixels;
	int width, height;
	// read in the pixel data
	pixels = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGBA);

	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	return ID;
}

void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius, float r, float g, float b) {
	int i;
	int lineAmount = 100; //# of triangles used to draw circle

	GLfloat twicePi = 2.0f * Pi;
	glColor3f(r, g, b);
	glBegin(GL_LINE_LOOP);
	for (i = 0; i <= lineAmount; i++) {
		glVertex2f(
			x + (radius * cos(i *  twicePi / lineAmount)),
			y + (radius* sin(i * twicePi / lineAmount))
		);
	}
	glEnd();
}

void drawStrokeText(char*string, int x, int y)
{
	char *c;
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(0.15f, 0.15f, 0);
	for (c = string; *c != '\0'; c++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
	}
	glPopMatrix();
}

void drawArrow(double startX, double startY, double endX, double endY, int arrowSize) {

	double angle = atan2(endY - startY, endX - startX) - Pi / 2.0;
	double sinAngle = sin(angle);
	double cosAngle = cos(angle);

	// point 1
	double x1 = (-0.5 * cosAngle + 0.8 * sinAngle) * arrowSize + endX;
	double y1 = (-0.5 * sinAngle - 0.8 * cosAngle) * arrowSize + endY;
	// point 2
	double x2 = (0.5 * cosAngle + 0.8 * sinAngle) * arrowSize + endX;
	double y2 = (0.5 * sinAngle - 0.8 * cosAngle) * arrowSize + endY;

	glColor3f(1.0f, 0.0f, 0.0f); // Red
	glLineWidth(8);

	glBegin(GL_LINES);
	glVertex2f(startX, startY);
	glVertex2f(endX, endY);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(endX, endY);
	glVertex2f(x1, y1);
	glEnd();

	glBegin(GL_LINES);
	glVertex2f(endX, endY);
	glVertex2f(x2, y2);
	glEnd();
	glLineWidth(1);
}