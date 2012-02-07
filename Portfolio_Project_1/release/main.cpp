//main.cpp

#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <string>
#include <cmath>
#include <iostream>
#include <list>
using namespace std;
#include <gl\glut.h>			// Header File For The OpenGL32 Library
#include <gl\GLU.h>			// Header File For The GLu32 Library
#include <gl\GLAux.h>
#include "AlphaBeta.h"
#include "button.h"

#define winWidth 800.0
#define winHeight 600.0


void initTextures(); //to be called in init(), loads and binds textures
void DrawWall(); //draws wall texture
void DrawFloor(); // draws floor texture
int loadBMP(char* filename); //function to load textures
void display(); //main display function
void drawBoard(); //draws the Connect 4 board and pieces
void linePiece(double scaleY); //helper function to drawBoard
void drawShadow(); //draws shadow of the board
void dropPiece(int count); //controls positions of falling piece
void startLightShow(int val); //initializes light show values
void lightShow(int val); //moves light positions
void win(char who); //function to call when someone wins, who is COMPUTER or HUMAN
void setLights(GLfloat positions[3],int& xDownUp,int& yDownUp); //helper functions to lightShow
void newGame(); //resets values to begin a new game
void myKeyboard(unsigned char key,int, int); // keyboard function
void drawFinPiece(); //draws one "blue fin" piece to the side of the main board
void initStart(); // the initialization function for the very start, choosing who goes first
void setStart(); // the display function for the very start, choosing who goes first
void hBtnMouseUp(); // the button up function for choosing human plays first
void cBtnMouseUp(); // the button up function for choosing cpu plays first
void startMouseFunc(int btn, int state, int x, int y); // the mouse handler for the very start, choosing who plays first

//timer functions for glowing pieces of victor
void glowHPiece(int increase);
void glowCPiece(int increase);

int textureCount = 0; //number of textures loaded

//Material properties
GLfloat board_ambient [] = {0.8f, 1.0f, 0.1f, 1.0f};  
GLfloat board_diffuse [] = {0.8f, 1.0f, 0.1f, 1.0f};
GLfloat board_specular [] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat board_shininess[] = {80.0f};

GLfloat boardFins_ambient [] = {0.0f, 0.0f, 1.0f, 1.0f};  
GLfloat boardFins_diffuse [] = {0.0f, 0.0f, 1.0f, 0.8f};
GLfloat boardFins_specular [] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat boardFins_shininess[] = {70.0f};

GLfloat hoveringPieceCoords[3] = {1.0,7.5,-1.0};
GLfloat hover_diffuse [] = {1.0f, 0.0f, 0.0f, 0.5f}; 
GLfloat hover_specular [] = {1.0f, 1.0f, 1.0f, 1.0f};  
GLfloat hover_shininess[] = {0.50f};

GLfloat hPiece_ambient [] = {1.0f, 0.0f, 0.0f, 1.0f};  
GLfloat hPiece_diffuse [] = {1.0f, 0.0f, 0.0f, 1.0f};
GLfloat hPiece_specular [] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat hPiece_shininess[] = {80.0f};
GLfloat hPiece_emission[] = {0.0, 0.0, 0.0, 1.0};

GLfloat cPiece_ambient [] = {0.1f, 0.1f, 0.1f, 1.0f};  
GLfloat cPiece_diffuse [] = {0.1f, 0.1f, 0.1f, 1.0f};
GLfloat cPiece_specular [] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat cPiece_shininess[] = {80.0f};
GLfloat cPiece_emission[] = {0.0, 0.0, 0.0, 1.0};

//variables for rotating scene based on mouseclick
double xDegrees = 0.0, yDegrees = 0.0;
double xOrigin,yOrigin;

float shadow_color[4] = {0.35,0.35,0.35,1};		//Shadows are dark gray

// set the light source properties
//position of the lights
GLfloat color[] = {.6,.6,.6,1};
GLfloat acolor[] = {0.7, 0.7, 0.7, 1};
GLfloat light_position[4] = {2.0f,6.0f,3.0f,1.0f}; // Light x,y,z positions, then w for directions(0) or positional

//quadric object for cylinders
GLUquadricObj* quadric;

bool falling = false;
bool rotating = false;

GLfloat dontShow[2] = {-1,-1}; //a twople (x,y) for telling the display function not to show a piece
GLfloat fallingPieceCoords[3] = {1.0,7.5,-1.0};

int dropTimer = 75; //miliseconds

//game variables
ConnectFourBoard theBoard;
AlphaBeta test;
int turn = 1;//for tracking whose turn it is to go (-1 = CPU, 1= HUMAN)
bool cpuThinking = false; // indicates whether CPU is currently thinking or not.
char wentLast = EMPTY; //for tracking who went last (COMPUTER or HUMAN)

//lightShow variables
int xIncDec = 0;
int yIncDec = 0;
int xIncDec2 = 0;
int yIncDec2 = 0;
int xIncDec3 = 0;
int yIncDec3 = 0;
int lightShowTimer = 100; // miliseconds
int GLOW_TIMER = 75; // miliseconds, how quickly to change glow effect of piece
bool lightShowOn = false;
bool gameOver = false;
char* startNewGame = "Press 'N' to start a new game";
char* cpuDeciding = "CPU is thinking...";

//Spotlight properties
GLfloat light1_ambient[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light1_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light1_specular[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light1_position[] = {1.0f,1.0f,6.0f,1.0f};

GLfloat light2_ambient[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light2_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light2_specular[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat light2_position[] = {6.0f,1.0f,6.0f,1.0f};

GLfloat light3_ambient[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat light3_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat light3_specular[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat light3_position[] = {3.0f,5.0f,6.0f,1.0f};

GLfloat spot_direction[] = { 0.0, 0.0, -1.0 };

//buttons
list<button> btnList;



void setStart()
{
	glClear(GL_COLOR_BUFFER_BIT); // clear the screen

	list<button>::iterator buttonIter = btnList.begin();
	while(buttonIter!=btnList.end())
	{
		(*buttonIter).drawButton();
		buttonIter++;
	}

	glutSwapBuffers();
	
}
void display()
{
	// start drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the screen
	glShadeModel(GL_SMOOTH);

	// set the camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,winWidth/winHeight,5,-50);
	glMatrixMode(GL_MODELVIEW);      // position and aim the camera
	glLoadIdentity();
	gluLookAt(4, 3, 10.0, 4, 3, -5.0 ,0.0, 1.0, 0.0);

	if (gameOver)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
		glShadeModel(GL_FLAT);

		glColor3f(0.0,0.0,0.0);
		glRasterPos2f(1.0,8.2);
		for (int i=0;startNewGame[i] != '\0';i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,startNewGame[i]);

		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glShadeModel(GL_SMOOTH);
		glPopMatrix();
		glPopAttrib();

	}
	if (cpuThinking)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
		glShadeModel(GL_FLAT);

		glColor3f(0.0,0.0,0.0);
		glRasterPos2f(2.0,7.2);
		for (int i=0;cpuDeciding[i] != '\0';i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,cpuDeciding[i]);

		glEnable(GL_LIGHTING);
		glEnable(GL_NORMALIZE);
		glShadeModel(GL_SMOOTH);
		glPopMatrix();
		glPopAttrib();
	}

	//rotate based on mouseDrag
	glRotatef(yDegrees,0.0,1.0,0.0);
	glRotatef(xDegrees,1.0,0.0,0.0);
	
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glTranslatef(0.0,-1.0,0.0);
	drawShadow();
	glTranslatef(0.0,1.0,0.0);
	drawBoard();
	glPopMatrix();
	glPopAttrib();

	//drawBoard();
	glTranslatef(0.0,-1.0,0.0);
	DrawFloor();
	glTranslatef(0.0,1.0,0.0);
	DrawWall();

	if (turn == 1 && !falling)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(1.0,0.0,0.0);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, hover_diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, hover_specular);
			glMaterialfv(GL_FRONT, GL_SHININESS, hover_shininess);
			glTranslatef(hoveringPieceCoords[0],hoveringPieceCoords[1],hoveringPieceCoords[2]);
			glScalef(1.0,1.0,0.3);
			glutSolidSphere(0.4,20,20);
			glScalef(1.0,1.0,-0.3);
		glPopMatrix();
		glPopAttrib();
		glDisable(GL_COLOR_MATERIAL);
	}

	glRotatef(-yDegrees,0.0,1.0,0.0);
	glRotatef(-xDegrees,1.0,0.0,0.0);

	glutSwapBuffers();
	if (turn == -1 && !falling)
	{
		int cpuChoice = test.AlphaBetaSearch(&theBoard);
		int cpuCol;
		cpuCol = theBoard.insertPiece(cpuChoice,COMPUTER);
		if (cpuCol != -1) {
			fallingPieceCoords[0] = cpuChoice*1.19;
			dontShow[0] = cpuCol;
			dontShow[1] = cpuChoice;
			wentLast = COMPUTER;
			falling = true;
			glutTimerFunc(dropTimer,dropPiece,cpuCol);
			cpuThinking = false;
			glutPostRedisplay();
		}
		else
			cout << "ERROR, no spaces left for CPU to go!" << endl;

		
	}

	
}
void drawBoard()
{
	//render falling piece, if there is one falling
	if (falling)
	{
		
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (wentLast == HUMAN)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, hPiece_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, hPiece_diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, hPiece_specular);
			glMaterialfv(GL_FRONT, GL_SHININESS, hPiece_shininess);
			
		}
		else if (wentLast == COMPUTER)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT, cPiece_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, cPiece_diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, cPiece_specular);
			glMaterialfv(GL_FRONT, GL_SHININESS, cPiece_shininess);
			
		}
			
		glTranslatef(fallingPieceCoords[0],fallingPieceCoords[1],fallingPieceCoords[2]);
		glScalef(1.0,1.0,0.3);
		glutSolidSphere(0.4,20,20);
		glTranslatef(fallingPieceCoords[0],fallingPieceCoords[1],fallingPieceCoords[2]);
		glScalef(1.0,1.0,-0.3);

		glPopMatrix();
		glPopAttrib();
	}

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glMaterialfv(GL_FRONT, GL_AMBIENT, board_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, board_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, board_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, board_shininess);
		glPushMatrix();
		//render toruses first
		for (GLfloat i=0.0;i<7.0;i++)
		{
			for (GLfloat j=0.0;j<6.0;j++)
			{
				glPushMatrix();
				glTranslatef(i+i*0.2,j+j*0.2,0.0);
				glPushMatrix();
					glTranslatef(0.0,0.0,-1.0);
					glScaled(1.0,1.0,0.865);
					glutSolidTorus(0.15,0.5,30,30);
					glScaled(1.0,1.0,-0.865);
					glTranslatef(0.0,0.0,1.0);
				glPopMatrix();
				glPopMatrix();
			}
		}
		glPopMatrix();

		//render lines to cover "in-between" toruses
		glPushMatrix();
			glTranslatef(-.54,3.0,-1.0);
			for (int i=0;i<8;i++){
				glPushMatrix();
					glTranslatef(i*1.18,0.0,0.0);
					linePiece(14.5);
					glTranslatef(-i*1.18,0.0,0.0);
				glPopMatrix();
			}
		glPopMatrix();

		glPushMatrix();
		glTranslatef(3.6,-0.54,-1.0);
		glRotatef(90.0,0.0,0.0,1.0);
		for (int i=0;i<7;i++){
			glPushMatrix();
				glTranslatef(i*1.18,0.0,0.0);
				linePiece(17.21);
				glTranslatef(-i*1.18,0.0,0.0);
			glPopMatrix();
		}
		glTranslatef(-3.6,0.54,1.0);
		glPopMatrix();

		//draw the 'fins' on the side of the board
		glPushMatrix();
		//glScaled(1.0,1.0,1.0);
		glTranslatef(-1.0,-1.0,-1.0);
		drawFinPiece();
		glTranslatef(8.6,0.0,0.0);
		drawFinPiece();
		glPopMatrix();
		glPopAttrib();


		//draw all the pieces
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		for (GLfloat i=0.0;i<7.0;i++)
		{
			for (GLfloat j=0.0;j<6.0;j++)
			{
				if (theBoard.board[static_cast<int>(j)][static_cast<int>(i)] != EMPTY && (j != dontShow[0] || i != dontShow[1]))
				{
					if (theBoard.board[static_cast<int>(j)][static_cast<int>(i)] == HUMAN)
					{
						glMaterialfv(GL_FRONT, GL_AMBIENT, hPiece_ambient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, hPiece_diffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, hPiece_specular);
						glMaterialfv(GL_FRONT, GL_SHININESS, hPiece_shininess);
						glMaterialfv(GL_FRONT, GL_EMISSION, hPiece_emission);
					}
					else
					{
						glMaterialfv(GL_FRONT, GL_AMBIENT, cPiece_ambient);
						glMaterialfv(GL_FRONT, GL_DIFFUSE, cPiece_diffuse);
						glMaterialfv(GL_FRONT, GL_SPECULAR, cPiece_specular);
						glMaterialfv(GL_FRONT, GL_SHININESS, cPiece_shininess);
						glMaterialfv(GL_FRONT, GL_EMISSION, cPiece_emission);
					}
					glPushMatrix();
					glTranslatef(i+i*0.2,j+j*0.2,0.0);
					glPushMatrix();
						glTranslatef(0.0,0.0,-1.0);
						glScalef(1.0,1.0,0.3);
						glutSolidSphere(0.4,20,20);
						glScalef(1.0,1.0,-0.3);
						glTranslatef(0.0,0.0,1.0);
					glPopMatrix();
					glPopMatrix();
				}
			}
		}
				
		glPopMatrix();
		glPopAttrib();

	glPopMatrix();
	glPopAttrib();

	
}
void drawShadow() 
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//Draw Shadow of board
	//Taken from simple shadows program from the share drive
	// Set up the shadow-projection matrix
	float shadow_matrix[16];
	int i;
	for (i=0;i<=15;i++) shadow_matrix[i]=0.0;
	shadow_matrix[0] = shadow_matrix[5] = shadow_matrix[10] = 1;
	shadow_matrix[7] = -1/(light_position[1]); // Project onto x-z plane

	// Temporarily disable the depth test and lighting:
	// Project the object onto the shadow plane
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glColor3f(shadow_color[0], shadow_color[1], shadow_color[2]);
		glTranslatef(light_position[0], light_position[1]-.9, light_position[2]+10); 
		glMultMatrixf(shadow_matrix); 
		glTranslatef(-light_position[0], -light_position[1]+.9, -light_position[2]-10); 

		// Draw the board
		glScaled(.96,0.3,0.3);
		drawBoard();
		glScaled(.96,-0.3,0.3);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	glPopAttrib();
	glPopMatrix();
	// Re-enable depth test and lighting:
	
	glPopMatrix();
	glPopAttrib();
}

void linePiece(double scaleY)
{
	glPushMatrix();
		glScaled(0.7,scaleY,0.5);
		glutSolidCube(0.5);
	glPopMatrix();
}

void myReshape(int w, int h)
{
	//disallow reshape
	glutReshapeWindow(winWidth,winHeight);
}
void motion(int x, int y)
{
	if (rotating)
	{
	y = winHeight -y;
	xDegrees +=  (winHeight/360 *  (yOrigin -y));
	yOrigin = y;

	yDegrees += (winWidth/360  *  (x - xOrigin));
	xOrigin = x;
	glutPostRedisplay();
	}
}//end of motion function

void passiveMotion (int x, int y)
{
	y = winHeight -y;
	x = (x-177)/60;
	if (x >= 0 && x <= 6)
	{
		hoveringPieceCoords[0] = x*1.2;
		glutPostRedisplay();
	}
}

void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		xOrigin = x;
		yOrigin = winHeight -y;
		rotating = true;
	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !rotating)
	{
		y = winHeight-y;
		//cout << x << ", " << y << endl;
		if ((175 <= x && x <= 585) && (140 < y && y < 535) && turn == 1 && !falling)
		{
			x = (x-177)/60;
			int col;
			fallingPieceCoords[0] = x*1.19;
			if ((col = theBoard.insertPiece(x,HUMAN)) != -1)
			{
				dontShow[0] = col;
				dontShow[1] = x;
				falling = true;
				wentLast = HUMAN;
				glutTimerFunc(dropTimer,dropPiece,col);
				glutPostRedisplay();
				
				
			}
		}
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		//snap picture back into place
		xDegrees = yDegrees = xOrigin = yOrigin= 0.0;
		rotating = false;
		glutPostRedisplay();
	}
	

}//end of mouse function

void DrawWall()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glShadeModel(GL_FLAT);
	glBindTexture(GL_TEXTURE_2D, 2);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-20.0, -10.0, -15.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(40.0, -10.0, -15.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(40.0, 25.0, -15.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-20.0, 25.0, -15.0);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

void DrawFloor()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glShadeModel(GL_FLAT);
	glBindTexture(GL_TEXTURE_2D, 1);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-7.0, -1.0, 2.0);
		glTexCoord2f(1.0, 0.0); glVertex3f(20.0, -1.0, 2.0);
		glTexCoord2f(1.0, 1.0); glVertex3f(20.0, -1.0, -15.0);
		glTexCoord2f(0.0, 1.0); glVertex3f(-7.0, -1.0, -15.0);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

int loadBMP(char *filename)
{
	//function courtesy of http://www.spacesimulator.net/tut3_texturemapping.html
   unsigned char *l_texture;
   int i, j=0;
   FILE *l_file;
   BITMAPFILEHEADER fileheader; 
   BITMAPINFOHEADER infoheader;
   RGBTRIPLE rgb;
 
   if( (l_file = fopen(filename, "rb"))==NULL) return (-1);
	
   fread(&fileheader, sizeof(fileheader), 1, l_file);
   fseek(l_file, sizeof(fileheader), SEEK_SET);
   fread(&infoheader, sizeof(infoheader), 1, l_file);
   l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
  
   memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
   for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
   { 
      fread(&rgb, sizeof(rgb), 1, l_file); 

      l_texture[j+0] = rgb.rgbtRed; // Red component
      l_texture[j+1] = rgb.rgbtRed; // Green component
      l_texture[j+2] = rgb.rgbtBlue; // Blue component
      l_texture[j+3] = 255; // Alpha value
      j += 4; // Go to the next position
   }
	fclose(l_file); // Closes the file stream
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	textureCount++;
	glBindTexture(GL_TEXTURE_2D, textureCount);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);
	free(l_texture);
	
	return textureCount;
}//end of loadBMP function

void initTextures()
{
	if (loadBMP("wood.bmp") == -1) // wood is now index 1
		MessageBox(NULL,"Image file: wood.bmp not found", "Error",MB_OK | MB_ICONERROR);
	if (loadBMP("brick.bmp") == -1) //brick is now index 2
		MessageBox(NULL,"Image file: brick.bmp not found", "Error",MB_OK | MB_ICONERROR);
}
void initStart()
{
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_NORMALIZE);
	glDisable(GL_BLEND); 
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0,1.0,1.0);

	button hBtn,cBtn;

	//Initialize buttons
	hBtn.text = "HUMAN plays first";
	cBtn.text = "COMPUTER plays first";
	hBtn.key = "HUMAN";
	cBtn.key = "CPU";
	hBtn.enableBtn();
	cBtn.enableBtn();
	hBtn.xCoords[0] = 190.0;
	hBtn.xCoords[1] = 320.0;
	hBtn.yCoords[0] = 400.0;
	hBtn.yCoords[1] = 350.0;
	cBtn.xCoords[0]= 400.0;
	cBtn.xCoords[1]= 550.0;
	cBtn.yCoords[0]= 400.0;
	cBtn.yCoords[1]= 350.0;
	hBtn.upFunc = &hBtnMouseUp;
	cBtn.upFunc = &cBtnMouseUp;
	btnList.push_back(hBtn);
	btnList.push_back(cBtn);
	glClearColor (0.0, 0.3, 0.3, 1.0);
	glMatrixMode (GL_PROJECTION);    
	glLoadIdentity ();    
	glOrtho(-1.0, winWidth, -1.0, winHeight, -1.0, 1.0);
}
void init()
{
		
	glColor3f(1.0,1.0,1.0);
	//glLoadIdentity();
	glDisable(GL_COLOR_MATERIAL);
	//initTextures();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, acolor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);

	////SPOT LIGHT PROPERTIES

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 10.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 3.0);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 10.0);
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 3.0);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

	glLightfv(GL_LIGHT3, GL_AMBIENT, light3_ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light3_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light3_specular);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 10.0);
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 3.0);
	glLightfv(GL_LIGHT3, GL_POSITION, light3_position);

	////END SPOT LIGHT PROPERTIES

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//initialize quadric for drawing cylinders
	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricOrientation(quadric, GLU_OUTSIDE);

	glEnable(GL_DEPTH_TEST); 
	glEnable(GL_NORMALIZE);
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0,1.0,1.0,1.0);
	glMatrixMode (GL_PROJECTION);    
	glLoadIdentity(); 
	glOrtho(0.0,winWidth,0.0,winHeight,-5.0,5.0);
	//glViewport(0,0,winWidth,winHeight);

	btnList.clear();
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(passiveMotion);
	glutKeyboardFunc(myKeyboard);
}

int main(int argc, char** argv)
{
	//seed random # generator
	srand((unsigned)time(0));

	//GUI INITIALIZATION FUNCTIONS
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(winWidth,winHeight);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Connect 4");

	//load bitmaps
	initTextures();
	initStart();
	glutDisplayFunc(setStart);
	glutReshapeFunc(myReshape);
	glutMouseFunc(startMouseFunc);
	//END GUI INITIALIZATION FUNCTIONS

	glutMainLoop();
	
}

void dropPiece(int col)
{

	if (fallingPieceCoords[1] > col+col*0.5)
	{
		fallingPieceCoords[1]-=0.5;
		glutTimerFunc(dropTimer,dropPiece,col);
	}
	else
	{
		fallingPieceCoords[1]=7.5;
		falling = false;
		dontShow[0] = dontShow[1] = -1;
		if (theBoard.winningBoard() != EMPTY)
		{
			win(wentLast);
			turn = 0;
			return;
		}
		if (theBoard.fullBoard())
		{
			cout << "CAT's GAME!" << endl;
			turn = 0;
			gameOver = true;
			glutPostRedisplay();
			return;
		}
		else
		{
			turn*= -1;
			
			if (wentLast == HUMAN)
				cpuThinking = true;
			
		}
	}
	glutPostRedisplay();


}
void startLightShow(int val)
{
	lightShowOn = true;
	gameOver = true;
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
}
void lightShow(int duration)
{
	//only change current x,y values by a little, but never allow x to go over 7.0 or below 0
	//and never allow 7 to go over 7.0 or below 0.
	//only allow a change of 0.2 in either direction.
	// rand = 0 ==> increase, rand !=0 ==> decrease;
	if (lightShowOn)
	{
		if (duration <= 0)
		{
			xIncDec = rand() % 2;
			yIncDec = rand() % 2;

			xIncDec2 = rand() % 2;
			yIncDec2 = rand() % 2;

			xIncDec3= rand() % 2;
			yIncDec3 = rand() % 2;

			duration = 10;
		}

		setLights(light1_position,xIncDec,yIncDec);
		setLights(light2_position,xIncDec2,yIncDec2);
		setLights(light3_position,xIncDec3,yIncDec3);

		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
		glLightfv(GL_LIGHT3, GL_POSITION, light3_position);
		duration--;
		
		glutTimerFunc(lightShowTimer,lightShow,duration);
		glutPostRedisplay();
	}
}

void setLights(GLfloat positions[3],int& xDownUp,int& yDownUp)
{
	if (positions[0] >= 6.0)
	{
		positions[0]-= 0.2;
		xDownUp = 1;
	}
	else if (positions[0] <= 0.0)
	{
		xDownUp =0;
		positions[0] += 0.2;
	}
	else if (xDownUp == 0)
		positions[0] += 0.2;
	else
		positions[0] -=0.2;

	if (positions[1] >= 6.0)
	{
		positions[1] -= 0.2;
		yDownUp = 1;
	}
	else if (positions[1] <= 0.0)
	{
		positions[1] += 0.2;
		yDownUp =0;
	}
	else if (yDownUp == 0)
		positions[1] += 0.2;
	else
		positions[1] -=0.2;

}

void win(char who)
{
	gameOver = true;
	glutTimerFunc(lightShowTimer+90,startLightShow,0);
	glutTimerFunc(lightShowTimer+100,lightShow,0);
	if (who == COMPUTER)
	{
		cPiece_emission[3] =  0.0;
		glutTimerFunc(GLOW_TIMER,glowCPiece,1);
		printf("Computer wins!\n");

	}
	else
	{
		hPiece_emission[0] = 0.0;
		hPiece_emission[3] = 0.0;
		glutTimerFunc(GLOW_TIMER,glowHPiece,1);
		printf("Human wins!\n");
	}

		
}

void newGame()
{
	//clear the board
	//turn the lightshow off
	//reset variables
	theBoard.clearBoard();
	lightShowOn = false;
	gameOver = false;
	glDisable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	falling = false;
	//turn = 1;

	//reset lightshow positions
	GLfloat light1p[] = {1.0f,1.0f,6.0f,1.0f};
	GLfloat light2p[] = {6.0f,1.0f,6.0f,1.0f};
	GLfloat light3p[] = {3.0f,5.0f,6.0f,1.0f};
	light1_position[0] = 1.0;
	light1_position[1] = 1.0;
	light1_position[2] = 6.0;
	light1_position[3] = 1.0;
	light2_position[0] = 6.0;
	light2_position[1] = 1.0;
	light2_position[2] = 6.0;
	light2_position[3] = 1.0;
	light3_position[0] = 3.0;
	light3_position[1] = 5.0;
	light3_position[2] = 6.0;
	light3_position[3] = 1.0;

	
	cPiece_emission[0] = 0.0;
	cPiece_emission[1] = 0.0;
	cPiece_emission[2] = 0.0;
	cPiece_emission[3] = 1.0;

	hPiece_emission[0] = 0.0;
	hPiece_emission[3] = 1.0;
	

	initStart();
	glutMotionFunc(NULL);
	glutDisplayFunc(setStart);
	glutMouseFunc(startMouseFunc);
	glutPassiveMotionFunc(NULL);
	glutKeyboardFunc(NULL);
	glutPostRedisplay();

	
}
void myKeyboard(unsigned char key,int, int)
{
	//allow new game to start if previous game is over
	if (gameOver && (key == 'n' || key == 'N'))
	{
		newGame();
		glutPostRedisplay();
	}
}

void drawFinPiece()
{
	glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);
			glMaterialfv(GL_FRONT, GL_AMBIENT, boardFins_ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, boardFins_diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, boardFins_specular);
			glMaterialfv(GL_FRONT, GL_SHININESS, boardFins_shininess);
			glRotatef(90.0,0.0,1.0,0.0);
			gluCylinder(quadric,0.5,0.5,0.5,15,15);
			glRotatef(-90.0,0.0,1.0,0.0);
			glPushMatrix();
			glScalef(0.1,1.0,1.0);
			glutSolidSphere(0.5,10,10);
			glPopMatrix();
			glTranslatef(0.5,0.0,0.0);
			glScalef(0.1,1.0,1.0);
			glutSolidSphere(0.5,10,10);

		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.25,2.8,0.0);
			glScaled(0.5,5.0,0.6);
			glutSolidCube(1.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0.24,-1.0,0.0);
			glRotatef(-90,1.0,0.0,0.0);
			glScalef(0.7,1.0,1.0);
			gluCylinder(quadric,0.9,0.35,0.95,20,20);
		glPopMatrix();
}

void startMouseFunc(int btn, int state, int x, int y)
{
	list<button>::iterator buttonIter;
	xOrigin = x;
		yOrigin = winHeight -y;
	if (btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		
		//check for click on buttons
		buttonIter = btnList.begin();
		while(buttonIter != btnList.end())
		{
			if ((*buttonIter).xCoords[0] < xOrigin && (*buttonIter).xCoords[1] > xOrigin
				&& (*buttonIter).yCoords[1] < yOrigin && (*buttonIter).yCoords[0] > yOrigin)
			{
				(*buttonIter).mouseDown();
				glutPostRedisplay();
				return;
			}
			buttonIter++;
		}
	}
	else if (btn==GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		//check for click on buttons
		buttonIter = btnList.begin();
		while(buttonIter!= btnList.end())
		{
			//(*buttonIter).up = true;
			if ((*buttonIter).xCoords[0] < xOrigin && (*buttonIter).xCoords[1] > xOrigin
				&& (*buttonIter).yCoords[1] < yOrigin && (*buttonIter).yCoords[0] > yOrigin)
			{
				(*buttonIter).mouseUp();
				glutPostRedisplay();
				return;
			}
			buttonIter++;
		}
	}
	glutPostRedisplay();
	
}

void hBtnMouseUp()
{
	//human turn
	turn = 1;
	init();
	glutPostRedisplay();
}
void cBtnMouseUp()
{
	//computer's turn
	turn = -1;
	cpuThinking = true;
	init();
	glutPostRedisplay();
}

void glowHPiece(int increase)
{
	if (gameOver)
	{
		if (hPiece_emission[0] >= 1.0)
		{
			increase = 0;
			hPiece_emission[0] -= 0.1;
		}
		else if (hPiece_emission[0] <= 0.0)
		{
			increase = 1;
			hPiece_emission[0] += 0.1;
		}
		else if (increase == 0)
		{
			hPiece_emission[0] -= 0.1;
		}
		else
			hPiece_emission[0] += 0.1;

		glutTimerFunc(GLOW_TIMER,glowHPiece,increase);
		glutPostRedisplay();
	}
}
void glowCPiece(int increase)
{
	if (gameOver)
	{
		if (cPiece_emission[0] >= 0.8)
		{
			increase = 0;
			cPiece_emission[0] -= 0.1;
			cPiece_emission[1] -= 0.1;
			cPiece_emission[2] -= 0.1;
		}
		else if (cPiece_emission[0] <= 0.0)
		{
			increase = 1;
			cPiece_emission[0] += 0.1;
			cPiece_emission[1] += 0.1;
			cPiece_emission[2] += 0.1;
		}
		else if (increase == 0)
		{
			cPiece_emission[0] -= 0.1;
			cPiece_emission[1] -= 0.1;
			cPiece_emission[2] -= 0.1;
		}
		else
		{
			cPiece_emission[0] += 0.1;
			cPiece_emission[1] += 0.1;
			cPiece_emission[2] += 0.1;
		}

		glutTimerFunc(GLOW_TIMER,glowCPiece,increase);
		glutPostRedisplay();
	}
}