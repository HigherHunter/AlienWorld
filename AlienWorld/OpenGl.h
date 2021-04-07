#ifndef OPENGL_H
#define OPENGL_H

#include <glut\glut.h>
#include <soil\SOIL.h>
#include "Utils.h"

// Loads .png as texture
GLuint LoadTexture(char *fileName);

// Draws hollow cirlce 
void drawHollowCircle(GLfloat x, GLfloat y, GLfloat radius, float r, float g, float b);

// Draws text on screen
void drawStrokeText(char*string, int x, int y);

// Draws arrow
void drawArrow(double startX, double startY, double endX, double endY, int arrowSize);

#endif // !OPENGL_H
