//button.h
//defines a generic button class for openGL

#ifndef BUTTON_H
#define BUTTON_H

#include "GL/glut.h"

class button
{
public:
	button()
	{
		up = true;
		disable = false;
		downFunc = NULL;
		upFunc = NULL;
		group = "default";
		key = "default";
	}
	GLfloat xCoords[2];
	GLfloat yCoords[2];
	bool up;
	char* text;
	char* key;
	char* group;
	
	void(*downFunc)(void);
	void(*upFunc)(void);
	void mouseDown()
	{
		up=false;
		if (disable)
			return;

		if (downFunc == NULL)
		{
			up=false;
			glutPostRedisplay();
			return;
		}

		(*downFunc)();
		glutPostRedisplay();
	}

	void disableBtn()
	{
		disable = true;
	}
	void enableBtn()
	{
		disable = false;
	}
	void mouseUp()
	{
		
		if (disable || upFunc == NULL)
			return;

		(*upFunc)();
		glutPostRedisplay();
	}

	void drawButton()
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glColor3f(.8, .8, 0.8);
		glBegin(GL_QUADS);
			glVertex2f(xCoords[0],yCoords[0]);
			glVertex2f(xCoords[1],yCoords[0]);
			glVertex2f(xCoords[1],yCoords[1]);
			glVertex2f(xCoords[0],yCoords[1]);
		glEnd();
		glColor3f(0.0, 0.0, 0.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2.0);
		if (up)
		{
			glBegin(GL_QUADS);
				glVertex2f(xCoords[0],yCoords[0]);
				glVertex2f(xCoords[1],yCoords[0]);
				glVertex2f(xCoords[1],yCoords[1]);
				glVertex2f(xCoords[0],yCoords[1]);
			glEnd();
		}

		up = true;
		if (disable)
			glColor3f(0.6, 0.6, 0.6);
		glRasterPos2f(xCoords[0]*1.03,yCoords[1]*1.07);
		for(int k=0;text[k] != '\0';k++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, text[k]);
	}
private:
	bool disable;
		
};
#endif