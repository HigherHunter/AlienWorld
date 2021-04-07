#ifndef DIALOG_BOX_H
#define DIALOG_BOX_H

#include "OpenGl.h"
#include "Globals.h"
#include "Utils.h"

class DialogBoxDisplay
{
private:

	int currentSelection;
	int maxItemsCount;
	int menu_width, menu_height;
	GLuint background;
	std::vector<std::string> elements;
	std::vector<std::string>::iterator it;

public:

	DialogBoxDisplay(int width, int height, std::vector<std::string> v)
	{
		elements = v;
		maxItemsCount = elements.size();
		currentSelection = 1;
		menu_width = width;
		menu_height = height;
		background = LoadTexture((char*)"Assets\\Sprites\\DialogBox\\DialogBoxBackground.png");
		it = elements.begin();
	}

	void RenderDialogBox(int xPos, int yPos, int mouseXPos, int mouseYPos)
	{
		int counter = 1;

		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, background);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(xPos - menu_width / 2, yPos - menu_height / 2);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(xPos + menu_width / 2, yPos - menu_height / 2);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(xPos + menu_width / 2, yPos + menu_height / 2);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(xPos - menu_width / 2, yPos + menu_height / 2);
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		std::string element;

		int startSpacing = (menu_height - 20) / elements.size();

		if (elements.size() == 1)
		{
			startSpacing = menu_height / 2;
		}
		else if (elements.size() == 2)
		{
			startSpacing = menu_height / 3;
		}

		int spacing = startSpacing;

		while (it != elements.end())
		{
			if ((unsigned)(mouseXPos - (xPos - menu_width / 2)) <= ((unsigned)(xPos + menu_width / 2) - (xPos - menu_width / 2)))
			{
				if ((unsigned)(mouseYPos - (yPos - menu_height / 2)) <= ((unsigned)(yPos + menu_height / 2) - (yPos - menu_height / 2)))
				{
					if ((unsigned)(mouseYPos - ((yPos + menu_height / 2 - spacing) - 10)) <= ((unsigned)((yPos + menu_height / 2 - spacing) + 10) -
						((yPos + menu_height / 2 - spacing) - 10)))
					{
						currentSelection = counter;
					}
				}
			}
			if ((it - elements.begin() + 1) == currentSelection)
				glColor3f(1.0f, 1.0f, 0.0f); // Yellow
			else
				glColor3f(0.0, 0.0, 1.0); // Blue
			element = *it;
			drawStrokeText((char*)element.c_str(),
				xPos - element.length() * 5, // in the middle, alligh with the center of the text
				yPos + menu_height / 2 - spacing); // draw from top of dialog box with equal spacing
			spacing += startSpacing;
			it++;
			counter++;
		}
		it = elements.begin();
		glColor3f(1.0, 1.0, 1.0);
	}

	void SetNextMenuItem()
	{
		if (currentSelection == maxItemsCount)
			currentSelection = 1;
		else
			currentSelection++;
	}

	void SetPrevMenuItem()
	{
		if (currentSelection == 1)
			currentSelection = maxItemsCount;
		else
			currentSelection--;
	}

	int GetCurrentSelection()const { return currentSelection; }
	void SetCurrentSelection(int selection) { currentSelection = selection; }

	int GetMaxItemCount()const { return maxItemsCount; }
	void SetMaxItemCount(int max) { maxItemsCount = max; }
};

#endif // !DIALOG_BOX_H
