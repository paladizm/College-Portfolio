//Zac Paladino
//CPS460
//cps460-01.16
//3D Project - Ship in Space
//04/28/2009

#include <cmath>
#include <iostream>
using namespace std;
#include <GL\glut.h>       // Header File For The OpenGL32 Library
#include <gl\GLU.h>			// Header File For The GLu32 Library
#include <gl\GLAux.h>

//Creation of the verticies for the Space Cube.
GLfloat vertices[][3] = {{-1000.0,-1000.0,-1000.0},{1000.0,-1000.0,-1000.0},
    {1000.0,1000.0,-1000.0}, {-1000.0,1000.0,-1000.0}, {-1000.0,-1000.0,1000.0},
    {1000.0,-1000.0,1000.0}, {1000.0,1000.0,1000.0}, {-1000.0,1000.0,1000.0}};
//Colors of the colorcube/SpaceCube
GLfloat colors[][4] = {{1.0,0.0,0.0,0.5},{1.0,0.0,0.0,0.5},
    {1.0,1.0,0.0,0.5}, {0.0,1.0,0.0,0.5}, {0.0,0.0,1.0,0.5},
    {1.0,0.0,1.0,0.5}, {1.0,1.0,1.0,0.5}, {0.0,1.0,1.0,0.5}};

//To store the textures.
GLuint texture[10];

//The globals for the Ships position. 
GLfloat ShipZPos = -500.0, ShipXPos = 0.0, ShipYPos = 0.0, Roll = 0.0;

//Bools for displaying the spacecube, Left view, rightview, start(for the starting
//position of the ship), and if the PC is HighRes.
bool disp = true, leftview = false, rightview = false, start=true, HighRes=false;

// this loads a bitmap
// it takes the bitmaps filename
// it returns the bitmap upon success else returns NULL
AUX_RGBImageRec* LoadBMP(  char strFName[] )
{
	AUX_RGBImageRec* pBitmap = NULL;

      if ( strFName == NULL || !strcmp( strFName, "" ) )
            return NULL;

      // load the bitmap
	pBitmap = auxDIBImageLoad( strFName );

	// return the bitmap
      return pBitmap;
} // end LoadBMP

// this frees a loaded bitmap loaded from LoadBMP
// it takes the bitmap struct
// it returns true upon success else returns false
bool DestroyBMP( AUX_RGBImageRec* pBitmap )
{
      if ( !pBitmap )
            return false;

      // free the bitmap
      free( pBitmap );
      pBitmap = NULL;

      return true;
} // end DestroyBMP

//creation of the polygon face of the spacecube.
void polygon(int a, int b, int c, int d)
{
	glBindTexture(GL_TEXTURE_2D, texture[0]);
    glBegin(GL_POLYGON);
    glColor4fv(colors[a]);
    glTexCoord2f(0.0,0.0);
    glVertex3fv(vertices[a]);
    glColor4fv(colors[b]);
    glTexCoord2f(0.0,1.0);
    glVertex3fv(vertices[b]);
    glColor4fv(colors[c]);
    glTexCoord2f(1.0,1.0);
    glVertex3fv(vertices[c]);
    glColor4fv(colors[d]);
    glTexCoord2f(1.0,0.0);
    glVertex3fv(vertices[d]);
    glEnd();
}
//Creation of the spacecube.
void colorcube()
{

/* map vertices to faces */
	if(disp){
		polygon(0,3,2,1);
		polygon(2,3,7,6);
		polygon(0,4,7,3);
		polygon(1,2,6,5);
		polygon(4,5,6,7);
		polygon(0,1,5,4);
	}
}
//These are the constants for rotation.
static GLfloat theta[] = {0.0,0.0,0.0};
static GLint axis = 2;

//This is the namespace for all the conversion constants.
namespace GEO {
  const double PI = 3.14159265359;
  const double TWOPI = 6.28318530718;
  const double DE2RA = 0.01745329252;
  const double RA2DE = 57.2957795129;
  const double FLATTENING = 1.0/298.26;
  const double PIOVER2 = 1.570796326795;
}

//This function creates texturemapped spheres in a given size R.
void CreateSphere(GLfloat R){

	glBegin( GL_TRIANGLES );

	GLint start_lat = -90;
	GLfloat start_lon = 0.0;
	GLfloat NumLatitudes = 100, NumLongitudes = 100; 

	GLfloat lat_incr = 180.0 / NumLatitudes;
	GLfloat lon_incr = 360.0 / NumLongitudes;

	int row, col;

	for (col = 0; col < NumLongitudes; col++){
	  GLfloat phi1 = (start_lon + col * lon_incr) * GEO::DE2RA;
	  GLfloat phi2 = (start_lon + (col + 1) * lon_incr) * GEO::DE2RA;
	  for (row = 0; row < NumLatitudes; row++){
		GLfloat theta1 = (start_lat + row * lat_incr) * GEO::DE2RA;
		GLfloat theta2 = (start_lat + (row + 1) * lat_incr) * GEO::DE2RA;
		GLdouble u[3], v[3], w[3];
		u[0] = R * cos(phi1) * cos(theta1);    //x
		u[1] = R * sin(theta1);                //y
		u[2] = R * sin(phi1) * cos(theta1);    //z

		v[0] = R * cos(phi1) * cos(theta2);    //x
		v[1] = R * sin(theta2);                //y
		v[2] = R * sin(phi1) * cos(theta2);    //z

		w[0] = R * cos(phi2) * cos(theta2);    //x
		w[1] = R * sin(theta2);                //y
		w[2] = R * sin(phi2) * cos(theta2);    //z

		
		glTexCoord2d((180.0 - phi1*GEO::RA2DE)/360.0,
					 (theta1 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(u);
		glTexCoord2d((180.0 - phi1*GEO::RA2DE)/360.0,
					 (theta2 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(v);
		glTexCoord2d((180.0 - phi2*GEO::RA2DE)/360.0,
					 (theta2 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(w);

		v[0] = R * cos(phi2) * cos(theta1);    //x
		v[1] = R * sin(theta1);                //y
		v[2] = R * sin(phi2) * cos(theta1);    //z
		
		glTexCoord2d((180.0 - phi1*GEO::RA2DE)/360.0,
					 (theta1 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(u);
		glTexCoord2d((180.0 - phi2*GEO::RA2DE)/360.0,
					 (theta2 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(w);
		glTexCoord2d((180.0 - phi2*GEO::RA2DE)/360.0,
					 (theta1 + GEO::PIOVER2)*GEO::RA2DE/180.0);
		glVertex3dv(v);

		glDisable( GL_TEXTURE_2D );
	  }
	}
	glEnd();
}
GLfloat Tred=1.0, Tblue=0.0, Tgreen = 0.0;
//This is the function that creates the Ship and textures the different
//sides and areas.
void CreateShip(){	
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	//Top
	glColor3f(.5,.5,.5);
	glBegin(GL_POLYGON);	    
		 glTexCoord2f(0.0,0.0);
		 glVertex3f(0.0,0.0,-500);		
		 glTexCoord2f(0.0,1);
		 glVertex3f(0.0,5.0,-500);		
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(5.0,5.0,-500);		
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(5.0,0.0,-500);	
	glEnd();
	glColor3f(.7,.7,.7);
	glBegin(GL_POLYGON);
		 glVertex3f(0.0,5.0,-500);		 
		 glTexCoord2f(1,1);
		 glVertex3f(2.5,10.0,-501.5);			
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-500);	
    glEnd();
	//Bottom
	glColor3f(.5,.5,.5);
	glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);
		 glVertex3f(0.0,0.0,-503);
		 glTexCoord2f(0.0,0.5);
		 glVertex3f(0.0,5.0,-503);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(5.0,5.0,-503);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(5.0,0.0,-503);
	 glEnd();
	 glColor3f(.7,.7,.7);
	 glBegin(GL_POLYGON);
		 glVertex3f(0.0,5.0,-503);		 
		 glTexCoord2f(1,1);
		 glVertex3f(2.5,10.0,-501.5);			
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-503);	
    glEnd();
	 //Side
	 glColor3f(.5,.5,.5);
	 glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);		 
		 glVertex3f(0.0,0.0,-503);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(0.0,5.0,-503);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(0.0,5.0,-500);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(0.0,0.0,-500);
	 glEnd();
	 glBegin(GL_POLYGON); 
		 glTexCoord2f(0.0,0.0);
		 glVertex3f(0.0,5.0,-503);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(2.5,10.0,-501.5);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(2.5,10.0,-501.5);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(0.0,5.0,-500);
	 glEnd();
	 //Other Side
	 glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);
		 glVertex3f(5.0,0.0,-500);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-500);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(5.0,5.0,-503);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(5.0,0.0,-503);
	 glEnd();
	 glBegin(GL_POLYGON); 
		 glTexCoord2f(0.0,0.0);
		 glVertex3f(5.0,5.0,-500);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(2.5,10.0,-501.5);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(2.5,10.0,-501.5);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-503);
	 glEnd();
	 //Backend
	 glColor3f(.9,.9,.9);
	 glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);		 
		 glVertex3f(0.0,0.0,-503);
		 glTexCoord2f(0.0,1.0);	
		 glVertex3f(0.0,0.0,-500);
		 glTexCoord2f(1.0,1.0);	
		 glVertex3f(5.0,0.0,-500);
		 glTexCoord2f(1.0,0.0);	
		 glVertex3f(5.0,0.0,-503);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	//Wing1
	glColor3f(0.0,1.0,1.0);
	glBegin(GL_POLYGON);	     
	     glTexCoord2f(0.0,0.0);
		 glVertex3f(0.0,0.0,-501);
		 glTexCoord2f(0.0,1);
		 glVertex3f(0.0,5.0,-501);
		 glTexCoord2f(1.0,1);
		 glVertex3f(-3.0,0.0,-501);
	glEnd();	
	glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);
	     glVertex3f(0.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(0.0,5.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(-3.0,0.0,-502);
	glEnd();
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);
	     glVertex3f(0.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(-3.0,0.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(-3.0,0.0,-501);	
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(0.0,0.0,-501);
	glEnd();
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);	
	     glTexCoord2f(0.0,0.0);
		 glVertex3f(-3.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(0.0,5.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(0.0,5.0,-501);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(-3.0,0.0,-501);		 
	glEnd();
	//Wing2
	glColor3f(0.0,1.0,1.0);
	glBegin(GL_POLYGON);
		 glTexCoord2f(0.0,0.0);
	     glVertex3f(5.0,0.0,-501);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-501);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(8.0,0.0,-501);
	glEnd();
	glBegin(GL_POLYGON);
	     glTexCoord2f(0.0,0.0);
		 glVertex3f(5.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(8.0,0.0,-502);
	glEnd();
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);
	     glTexCoord2f(0.0,0.0);
	     glVertex3f(5.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(8.0,0.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(8.0,0.0,-501);	
		 glTexCoord2f(1.0,.0);
		 glVertex3f(5.0,0.0,-501);
	glEnd();
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);
	     glTexCoord2f(0.0,0.0);
	     glVertex3f(8.0,0.0,-502);
		 glTexCoord2f(0.0,1.0);
		 glVertex3f(5.0,5.0,-502);
		 glTexCoord2f(1.0,1.0);
		 glVertex3f(5.0,5.0,-501);
		 glTexCoord2f(1.0,0.0);
		 glVertex3f(8.0,0.0,-501);		 
	glEnd();
	//BackWing
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);
	    glTexCoord2f(0.0,0.0);
		glVertex3f(2.2,0.3,-500);
		glTexCoord2f(0.0,1.0);
		glVertex3f(2.2,0.3,-498);
		glTexCoord2f(1.0,1.0);
		glVertex3f(2.7,0.3,-498);
		glTexCoord2f(1.0,0.0);
		glVertex3f(2.7,0.3,-500);
	glEnd();
	glColor3f(0.0,1.0,1.0);
	glBegin(GL_POLYGON);	
	    glTexCoord2f(0.0,0.0);
		glVertex3f(2.2,0.3,-498);
		glTexCoord2f(0.0,1.0);
		glVertex3f(2.2,2.3,-500);
		glTexCoord2f(1.0,1.0);
		glVertex3f(2.7,2.3,-500);
		glTexCoord2f(1.0,0.0);
		glVertex3f(2.7,0.3,-498);
	glEnd();
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_POLYGON);
	    glTexCoord2f(0.0,0.0);
		glVertex3f(2.2,0.3,-498);
		glTexCoord2f(0.0,1.0);
		glVertex3f(2.2,2.3,-500);
		glTexCoord2f(1.0,1.0);
		glVertex3f(2.2,0.3,-500);		
	glEnd();
	glBegin(GL_POLYGON);
	    glTexCoord2f(0.0,0.0);
		glVertex3f(2.7,0.3,-498);
		glTexCoord2f(0.0,1.0);
		glVertex3f(2.7,2.3,-500);
		glTexCoord2f(1.0,1.0);
		glVertex3f(2.7,0.3,-500);		
	glEnd();
	//Thrusters
    glColor3f(Tred,Tgreen,Tblue);
	glPushMatrix();
	    glTranslated(1.0,-0.0,-501.5);
		glutSolidSphere(.5,100,100);
	glPopMatrix();
	glPushMatrix();
	    glTranslated(2.5,-0.0,-501.5);
		glutSolidSphere(.5,100,100);
	glPopMatrix();
	glPushMatrix();
	    glTranslated(4,-0.0,-501.5);
		glutSolidSphere(.5,100,100);
	glPopMatrix();
    //Windows
	glColor3f(1.0,1.0,1.0);
	glBindTexture(GL_TEXTURE_2D, texture[3]);	
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0,0.0);
		glVertex3f(1.5,5.0,-500);
		glTexCoord2f(0.0,1.0);
		glVertex3f(1.5,6.5,-500);
		glTexCoord2f(1.0,1.0);
		glVertex3f(3.5,6.5,-500);
		glTexCoord2f(1.0,0.0);
		glVertex3f(3.5,5.0,-500);
	glEnd();		
	glBegin(GL_POLYGON);
	    glTexCoord2f(0.0,0.0);
		glVertex3f(1.5,6.5,-500);
		glTexCoord2f(0.0,1.0);
		glVertex3f(1.5,8,-501);
		glTexCoord2f(1.0,1.0);
		glVertex3f(3.5,8,-501);
		glTexCoord2f(1.0,0.0);
		glVertex3f(3.5,6.5,-500);
	glEnd();
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_POLYGON);
		glVertex3f(1.5,5.0,-500);
		glVertex3f(1.5,6.5,-500);
		glVertex3f(1.5,8,-501);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3f(3.5,5.0,-500);
		glVertex3f(3.5,6.5,-500);
		glVertex3f(3.5,8,-501);
	glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();  
	//Display Space cube.
    colorcube();
	if(HighRes){
		//Display the Sun.
		glLoadIdentity();
		glPushMatrix();  
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(0.0,-400.0,700.0);	
		  glRotatef(theta[2], 0.0, 1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[9]);
		  CreateSphere(300);  // draw the sphere
		glPopMatrix();
		//Display Mercury
		glLoadIdentity();
		glPushMatrix();   
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(0.0,-300.0,200.0);	
		  glRotatef(theta[2], 0.0, -1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[8]);
		  CreateSphere(50);  // draw the sphere
		glPopMatrix();
		//Display Venus
		glLoadIdentity();
		glPushMatrix();   
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(150.0,-100.0,0.0);	
		  glRotatef(theta[2], 0.0, 1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[7]);
		  CreateSphere(100);  // draw the sphere
		glPopMatrix();
	}
	//Display Earth
	if(HighRes || ShipYPos-20 >= -600){
		glLoadIdentity();
		glPushMatrix();   
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(0.0,100.0,-300.0);	
		  glRotatef(theta[2], 0.0, -1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[4]);
		  CreateSphere(75);  // draw the sphere
		glPopMatrix();	
	}
	//Display Mars
	if(HighRes || ShipYPos-20 >= -340){
		glLoadIdentity();	
		glPushMatrix();   
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(150.0,300.0,-600.0);	
		  glRotatef(theta[2], 0.0, 1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[5]);
		  CreateSphere(75);  // draw the sphere
		glPopMatrix();		
	}
	//Display Jupiter.
	if(HighRes || ShipYPos-20 >= -250){
		glLoadIdentity();
		glPushMatrix();   
		  glColor3f(1.0,1.0,1.0);
		  glTranslated(-100.0,500.0,-700.0);	
		  glRotatef(theta[2], 0.0, -1.0, 0.0);
		  glBindTexture(GL_TEXTURE_2D, texture[6]);
		  CreateSphere(150);  // draw the sphere
		glPopMatrix();	
	}
	//Display and create ship.
	glLoadIdentity();
	glPushMatrix();		
		glTranslated(ShipXPos,ShipYPos,ShipZPos);
		glRotatef(90,1.0,0.0,0.0);
		glRotatef(180,0.0,1.0,0.0);
		glRotatef(Roll,0.0,1.0,0.0);	
		glTranslated(-ShipXPos,-ShipYPos,-ShipZPos);	
		CreateShip();
	glPopMatrix();
	//Starting possition of the ship.
	if(start){
		ShipYPos+=480;		
		glMatrixMode(GL_PROJECTION);	
			glTranslated(0.0,-480.0,480.0);
		glMatrixMode(GL_MODELVIEW);
		start = !start;
	}
    glutSwapBuffers();
}

void spinCube()
{
    //Spins the planets
	theta[axis] += 2.0;
    if( theta[axis] > 360.0 ) theta[axis] -= 360.0;
    glutPostRedisplay();
	Sleep(50);
}
//Mouse Commands
void mouse(int btn, int state, int x, int y)
{
	//Left button crates a left hand view for the ship
	//Also can go back to original if pressed again.
	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		leftview = !leftview;
		if(leftview){
			glMatrixMode(GL_PROJECTION);	
				glTranslated(-15.0,-15.0,0.0);
			glMatrixMode(GL_MODELVIEW);	
		}
		else{
			glMatrixMode(GL_PROJECTION);	
				glTranslated(15.0,15.0,0.0);
			glMatrixMode(GL_MODELVIEW);	
		}
	}
	//Shoots ship back 50 spaces to go back to the begining of
	//the seen faster.
	if(btn==GLUT_MIDDLE_BUTTON && state == GLUT_DOWN){
		ShipYPos+=50;		
		glMatrixMode(GL_PROJECTION);	
			glTranslated(0.0,-50.0,50.0);
		glMatrixMode(GL_MODELVIEW);				
	}
	//shows the right view of the Ship and if pressed again
	//goes back to the original view.
	if(btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		rightview = !rightview;
		if(rightview){
			glMatrixMode(GL_PROJECTION);	
				glTranslated(15.0,-15.0,0.0);
			glMatrixMode(GL_MODELVIEW);	
		}
		else{
			glMatrixMode(GL_PROJECTION);	
				glTranslated(-15.0,15.0,0.0);
			glMatrixMode(GL_MODELVIEW);	
		}
	}
}
//Keyboard commands.
void key(unsigned char k, int x, int y)
{
	//Shoots the ship forward through the seen a bit. and
	//changed the thrusters to red.
	if(k == 'w'){
		ShipYPos-=5;		
		glMatrixMode(GL_PROJECTION);	
			glTranslated(0.0,5.0,-5.0);
		glMatrixMode(GL_MODELVIEW);	
		Tred = 1.0;
		Tblue = 0.0;
		Tgreen = 0.0;
	}
	//Shoots the ship backa bit and turns the thrusters
	//blue.
	if(k == 's'){
		ShipYPos+=5;		
		glMatrixMode(GL_PROJECTION);	
			glTranslated(0.0,-5.0,5.0);
		glMatrixMode(GL_MODELVIEW);	
		Tblue = 1.0;
		Tred = 0.0;
		Tgreen = 0.0;
	}
	//Rolls the ship the the right and thurns thrusters 
	//green
	if(k=='d'){
		Roll += 5.0;
		if( Roll > 360.0 ) Roll -= 360.0;
		Tblue = 0.0;
		Tred = 0.0;
		Tgreen = 1.0;

	}
	//Rolls the ship to the left and turns thrusters 
	//green
	if(k=='a'){
		Roll -= 5.0;
		if( Roll > -360.0 ) Roll += 360.0;
		Tblue = 0.0;
		Tred = 0.0;
		Tgreen = 1.0;
	}
}

//This function sets all of the textures for the seen so they can be rendered.
void Textures(){
	glEnable(GL_TEXTURE_2D);
	AUX_RGBImageRec *TextureImage[10];			// Create Storage Space For The Texture
	memset(TextureImage,0,sizeof(void *)*1);    // Set The Pointer To NULL

	//This is the same code as the texture example in class just with my bitmaps.
	// Load The Bitmap, Check For Errors, If Bitmap's Not Found Quit
	//This is for the spacecube.
	if (TextureImage[0]=LoadBMP("space.bmp"))
	{				
		glGenTextures(1, &texture[0]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
	}
	//This is the Ship texture.
	if (TextureImage[1]=LoadBMP("ship.bmp"))
	{				
		glGenTextures(1, &texture[1]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[1]->sizeX, TextureImage[1]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[1]->data);
	}	
	//This is for the wings.
	if (TextureImage[2]=LoadBMP("rocks.bmp"))
	{				
		glGenTextures(1, &texture[2]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[2]->sizeX, TextureImage[2]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[2]->data);
	}
	//Glass of the ship.
	if (TextureImage[3]=LoadBMP("frostedglass.bmp"))
	{				
		glGenTextures(1, &texture[3]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[3]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[3]->sizeX, TextureImage[3]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[3]->data);
	}
	//Earths texture.
	if (TextureImage[4]=LoadBMP("et.bmp"))
	{				
		glGenTextures(1, &texture[4]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[4]->sizeX, TextureImage[4]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[4]->data);
	}
	//Mars Texture
	if (TextureImage[5]=LoadBMP("mt.bmp"))
	{				
		glGenTextures(1, &texture[5]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[5]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[5]->sizeX, TextureImage[5]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[5]->data);
	}
	//Jupiter texture
	if (TextureImage[6]=LoadBMP("jt.bmp"))
	{				
		glGenTextures(1, &texture[6]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[6]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[6]->sizeX, TextureImage[6]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[6]->data);
	}
	//Venus texture.
	if (TextureImage[7]=LoadBMP("venus.bmp"))
	{				
		glGenTextures(1, &texture[7]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[7]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[7]->sizeX, TextureImage[7]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[7]->data);
	}
	//Mercury texture.
	if (TextureImage[8]=LoadBMP("merc.bmp"))
	{				
		glGenTextures(1, &texture[8]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[8]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[8]->sizeX, TextureImage[8]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[8]->data);
	}
	//Sun texture.
	if (TextureImage[9]=LoadBMP("Sun.bmp"))
	{				
		glGenTextures(1, &texture[9]);					// Create The Texture

		// Typical Texture Generation Using Data From The Bitmap
		glBindTexture(GL_TEXTURE_2D, texture[9]);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[9]->sizeX, TextureImage[9]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[9]->data);
	}
	//dynamically creating texture.
	for(int i = 0; i < 10; i++){
		if (TextureImage[i])									// If Texture Exists
		{
			if (TextureImage[i]->data)							// If Texture Image Exists
			{
				free(TextureImage[i]->data);					// Free The Texture Image Memory
			}
			free(TextureImage[i]);								// Free The Image Structure
		}
	}	
	//modulating the texture environment so everything blends.
	 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void Init(){
	glMatrixMode(GL_PROJECTION);    
	//Starting perspective view.
	gluPerspective(100.0,1.0,.3,-40000.0);	
	gluLookAt(-2.5,3.5,-520.0,-3,2.5,-501,0.0,1.0,0.0);	
    glMatrixMode(GL_MODELVIEW);		
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
	glEnable(GL_DEPTH_TEST);	
	//Asking if the machine is Highres or not (Most can handle this.)
	int t;
	cout << "Is this a HighRes Machine? (0 No - 1 Yes): ";
	cin >> t;
	if(t==1){
		HighRes = true;
	}	
}

int main(int argc, char **argv)
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);	
    glutCreateWindow("Zac Paladino - Ship In Space");
	Init();   	
	//Purposly left out a reshape so it can go full screen
    glutDisplayFunc(display);	
    glutIdleFunc(spinCube);
    glutMouseFunc(mouse);           
    glutKeyboardFunc(key);
    glClearColor(1.0,1.0,1.0,1.0);
	Textures();		
    glutMainLoop();
}