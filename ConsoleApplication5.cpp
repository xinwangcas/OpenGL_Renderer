// ConsoleApplication5.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_DEPRECATE

#include "stdafx.h"

/****************************************************************************
Author: Xin Wang

  Reference:
  example5.cpp

  A GLUI program demonstrating subwindows, rotation controls,
	translation controls, and listboxes

  Paul Rademacher (rademach@cs.unc.edu)

****************************************************************************/

#include <string.h>
#include <GL/glui.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl/GL.H>
#include <gl/GLU.H>
#include <gl/glut.h>
#include <gl/GLAUX.H>
#include <string>
#include <vector>

using namespace std;

GLboolean doubleBuffer;
const GLfloat PI = 3.14;

GLuint	filter;									// filter type
volatile float xd = 0, yd = 0, zd = 0;
volatile float xpre = 0, ypre = 0;
//volatile float scale = 5;
volatile float xmin = 50, ymin = 50, zmin = 50, xmax = 0, ymax = 0, zmax = 0;
volatile GLfloat	depth=-15.0f;								// depth into the screen
GLboolean mouserdown = GL_FALSE;
GLboolean mouseldown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;
static GLint mousex = 0, mousey = 0;
static GLfloat yrotate = PI/4; /// angle between y-axis and look direction
static GLfloat xrotate = PI/4; /// angle between x-axis and look direction
static GLfloat celength = 20.0f;/// lenght between center and eye
static GLfloat mSpeed = 2.0f; /// center move speed
static GLfloat rSpeed = 2.0f; /// rotate speed
static GLfloat lSpeed = 2.0f; /// reserved
static GLfloat center[3] = {0.0f, 0.0f, 0.0f}; /// center position
static GLfloat eye[3]; /// eye's position
GLUI_Checkbox    *checkbox;

char* filename = new char[10];
char* filename2 = new char[10];
bool white = 0;
bool green = 0;
bool yellow = 0;

float xy_aspect;
int   last_x, last_y;
float rotationX = 0.0, rotationY = 0.0;

/** These are the live variables passed into GLUI ***/
int   wireframe = 0;
int   obj_type = 1;
int   segments = 8;
int   segments2 = 8;
int   light0_enabled = 1;
int   light1_enabled = 1;
float light0_intensity = 1.0;
float light1_intensity = .4;
int   main_window;
float scale = 5.0;
int   show_sphere=1;
int   show_torus=1;
int   show_axes = 1;
int   show_text = 1;
float sphere_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float torus_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };
char *string_list[] = { "Objects", "Protease", "Ligand", "axes" };
int   curr_string = 0;

/** Pointers to the windows and some of the controls we'll create **/
GLUI *glui, *glui2;
GLUI_Spinner    *light0_spinner, *light1_spinner;
GLUI_RadioGroup *radio;
GLUI_Panel      *obj_panel;

/********** User IDs for callbacks ********/
#define LIGHT0_ENABLED_ID    200
#define LIGHT1_ENABLED_ID    201
#define LIGHT0_INTENSITY_ID  250
#define LIGHT1_INTENSITY_ID  260
#define ENABLE_ID            300
#define DISABLE_ID           301
#define SHOW_ID              302
#define HIDE_ID              303


/********** Miscellaneous global variables **********/

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

/**************************************** control_cb() *******************/
/* GLUI control callback                                                 */

void control_cb( int control )
{
  if ( control == LIGHT0_ENABLED_ID ) {
    if ( light0_enabled ) {
      glEnable( GL_LIGHT0 );
      light0_spinner->enable();
    }
    else {
      glDisable( GL_LIGHT0 ); 
      light0_spinner->disable();
    }
  }
  else if ( control == LIGHT1_ENABLED_ID ) {
    if ( light1_enabled ) {
      glEnable( GL_LIGHT1 );
      light1_spinner->enable();
    }
    else {
      glDisable( GL_LIGHT1 ); 
      light1_spinner->disable();
    }
  }
  else if ( control == LIGHT0_INTENSITY_ID ) {
    float v[] = { 
      light0_diffuse[0],  light0_diffuse[1],
      light0_diffuse[2],  light0_diffuse[3] };
    
    v[0] *= light0_intensity;
    v[1] *= light0_intensity;
    v[2] *= light0_intensity;

    glLightfv(GL_LIGHT0, GL_DIFFUSE, v );
  }
  else if ( control == LIGHT1_INTENSITY_ID ) {
    float v[] = { 
      light1_diffuse[0],  light1_diffuse[1],
      light1_diffuse[2],  light1_diffuse[3] };
    
    v[0] *= light1_intensity;
    v[1] *= light1_intensity;
    v[2] *= light1_intensity;

    glLightfv(GL_LIGHT1, GL_DIFFUSE, v );
  }
  else if ( control == ENABLE_ID )
  {
    glui2->enable();
  }
  else if ( control == DISABLE_ID )
  {
    glui2->disable();
  }
  else if ( control == SHOW_ID )
  {
    glui2->show();
  }
  else if ( control == HIDE_ID )
  {
    glui2->hide();
  }
}

/**************************************** myGlutKeyboard() **********/

void myGlutKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: 
  case 'q':
    exit(0);
    break;
  };
  
  glutPostRedisplay();
}


/***************************************** myGlutMenu() ***********/

void myGlutMenu( int value )
{
  myGlutKeyboard( value, 0, 0 );
}


/***************************************** myGlutIdle() ***********/

void myGlutIdle( void )
{
  /* According to the GLUT specification, the current window is 
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window ) 
    glutSetWindow(main_window);  

  /*  GLUI_Master.sync_live_all();  -- not needed - nothing to sync in this
                                       application  */

  glutPostRedisplay();
}

/***************************************** myGlutMouse() **********/

void myGlutMouse(int button, int button_state, int x, int y )
{
}


/***************************************** myGlutMotion() **********/

void myGlutMotion(int x, int y )
{
  glutPostRedisplay(); 
}

/**************************************** myGlutReshape() *************/

void myGlutReshape( int x, int y )
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );

  glViewport( tx, ty, tw, th );

  xy_aspect =  (float)tw / (float)th;

  glutPostRedisplay();
}


/************************************************** draw_axes() **********/
/* Disables lighting, then draws RGB axes                                */

void draw_axes( float scale )
{
  glDisable( GL_LIGHTING );

  glPushMatrix();
  glScalef( scale, scale, scale );

  glBegin( GL_LINES );
 
  glColor3f( 1.0, 0.0, 0.0 );
  glVertex3f( .8f, 0.05f, 0.0 );  glVertex3f( 1.0, 0.25f, 0.0 ); /* Letter X */
  glVertex3f( 0.8f, .25f, 0.0 );  glVertex3f( 1.0, 0.05f, 0.0 );
  //glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 1.0, 0.0, 0.0 ); /* X axis      */
   glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 600.0, 0.0, 0.0 ); /* X axis      */

  glColor3f( 0.0, 1.0, 0.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 600.0, 0.0 ); /* Y axis      */

  glColor3f( 0.0, 0.0, 1.0 );
  glVertex3f( 0.0, 0.0, 0.0 );  glVertex3f( 0.0, 0.0, 600.0 ); /* Z axis    */
  glEnd();

  glPopMatrix();

  glEnable( GL_LIGHTING );
}

typedef struct tagVERTEX					
{
	float x, y, z;						
	float u, v, w;							
} VERTEX;

typedef struct tagTRIANGLE					
{
	VERTEX vertex[3];	
} TRIANGLE;	

typedef struct tagSECTOR					
{
	int numtriangles;						
	TRIANGLE* triangle;						
} SECTOR;

char* SURFfile = "1a0j-ms.SURF";//protein file
char* SURFfile2 = "1a0j.SURF";//ligand file
FILE *filein;
FILE *filein2;
string *position;
string *relation;
int numtriangles;
bool	light;									// control light on/off
bool	lp;									
bool	fp;									
SECTOR sector1;
SECTOR sector2;

GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 0.5f }; 
GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition_FRONT[]= { 0.0f, 0.0f, 1000.0f, 1.0f };
GLfloat LightPosition_BACK[]= { 0.0f, 0.0f, -1000.0f, 1.0f };
GLfloat LightPosition_UP[]= { 0.0f, 1000.0f, 0.0f, 1.0f };
GLfloat LightPosition_BOTTOM[]= { 0.0f, -1000.0f, 0.0f, 1.0f };
GLfloat LightPosition_LEFT[]= { 1000.0f, 0.0f, 0.0f, 1.0f };
GLfloat LightPosition_RIGHT[]= { -1000.0f, 0.0f, 0.0f, 1.0f };
GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

GLuint	texture[3];								

void SetupWorld()							
{
	char tmp[100] = {0};
	int num=0;
	char Geometry[100] = {0};
	char Topology[100] = {0};

	filein = fopen(SURFfile, "r");				
	if(NULL == filein)
		exit(1);

	for (int i = 0 ; i < 14 ; i ++){
		fgets(tmp, sizeof(char)*100,filein);
	}
	fgets(tmp,sizeof(char)*100,filein);
	sscanf(tmp,"%s %d",Geometry,&num);
	position = new string[num];
	for (int i = 0 ; i < num ; i ++){
		fgets(tmp, sizeof(char)*100,filein);
		position[i] = tmp;
	}
	for(int i = 0 ; i < 7 ; i ++){
		fgets(tmp, sizeof(char)*100,filein);
	}
	fgets(tmp,sizeof(char)*100,filein);
	sscanf(tmp,"%s %d",Topology,&num);
	numtriangles = num;
	relation = new string[num];
	for (int i = 0 ; i < num ; i ++){
		fgets(tmp, sizeof(char)*100,filein);
		relation[i] = tmp;
	}
	fclose(filein);		

	float x, y, z, u, v, w;
	int a, b, c;
	
	sector1.triangle = new TRIANGLE[numtriangles];	
	sector1.numtriangles = numtriangles;
	vector<int> myvector;

	for (int triloop = 0; triloop < numtriangles; triloop++)	
	{
		const char* top = relation[triloop].c_str();
        myvector.clear();
		sscanf(top, "%d %d %d", &a, &b, &c);
		myvector.push_back(a);
		myvector.push_back(b);
		myvector.push_back(c);
		int i = 0;
		for (int vertloop : myvector)		
		{
			const char* pos = position[vertloop].c_str();
			sscanf(pos, "%f %f %f %f %f %f", &x, &y, &z, &u, &v, &w);
			sector1.triangle[triloop].vertex[i].x = x;	
			sector1.triangle[triloop].vertex[i].y = y;	
			sector1.triangle[triloop].vertex[i].z = z;	
			sector1.triangle[triloop].vertex[i].u = u;	
			sector1.triangle[triloop].vertex[i].v = v;	
			sector1.triangle[triloop].vertex[i].w = w;	

			++i;
			xmax = (x>xmax)?x:xmax;
			ymax = (y>xmax)?y:ymax;
			zmax = (z>xmax)?z:zmax;
			xmin = (x<xmin)?x:xmin;
			ymin = (y<ymin)?y:ymin;
			zmin = (z<zmin)?z:zmin;
		}
	}

	filein2 = fopen(SURFfile2, "r");				
	if(NULL == filein2)
		exit(1);

	for (int i = 0 ; i < 14 ; i ++){
		fgets(tmp, sizeof(char)*100,filein2);
	}
	fgets(tmp,sizeof(char)*100,filein2);
	sscanf(tmp,"%s %d",Geometry,&num);
	position = new string[num];
	for (int i = 0 ; i < num ; i ++){
		fgets(tmp, sizeof(char)*100,filein2);
		position[i] = tmp;
	}
	for(int i = 0 ; i < 7 ; i ++){
		fgets(tmp, sizeof(char)*100,filein2);
	}
	fgets(tmp,sizeof(char)*100,filein2);
	sscanf(tmp,"%s %d",Topology,&num);
	numtriangles = num;
	relation = new string[num];
	for (int i = 0 ; i < num ; i ++){
		fgets(tmp, sizeof(char)*100,filein2);
		relation[i] = tmp;
	}
	fclose(filein2);		
	
	sector2.triangle = new TRIANGLE[numtriangles];	
	sector2.numtriangles = numtriangles;

	for (int triloop = 0; triloop < numtriangles; triloop++)	
	{
		const char* top = relation[triloop].c_str();
        myvector.clear();
		sscanf(top, "%d %d %d", &a, &b, &c);
		myvector.push_back(a);
		myvector.push_back(b);
		myvector.push_back(c);
		int i = 0;
		for (int vertloop : myvector)		
		{
			const char* pos = position[vertloop].c_str();
			sscanf(pos, "%f %f %f %f %f %f", &x, &y, &z, &u, &v, &w);
			sector2.triangle[triloop].vertex[i].x = x;	
			sector2.triangle[triloop].vertex[i].y = y;	
			sector2.triangle[triloop].vertex[i].z = z;	
			sector2.triangle[triloop].vertex[i].u = u;	
			sector2.triangle[triloop].vertex[i].v = v;	
			sector2.triangle[triloop].vertex[i].w = w;	

			++i;
			xmax = (x>xmax)?x:xmax;
			ymax = (y>xmax)?y:ymax;
			zmax = (z>xmax)?z:zmax;
			xmin = (x<xmin)?x:xmin;
			ymin = (y<ymin)?y:ymin;
			zmin = (z<zmin)?z:zmin;
		}
	}
}

void destroyWorld(void){

	delete position;
	delete relation;
	delete sector1.triangle;	
	delete sector2.triangle;
}


/***************************************** myGlutDisplay() *****************/

void myGlutDisplay( void )
{
  glClearColor( .9f, .9f, .9f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -xy_aspect*.04, xy_aspect*.04, -.04, .04, .1, 15.0 );

  glMatrixMode( GL_MODELVIEW );

  glLoadIdentity();
  glMultMatrixf( lights_rotation );
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  glLoadIdentity();
  glTranslatef( 0.0, 0.0, -2.6f );
  glTranslatef( obj_pos[0], obj_pos[1], -obj_pos[2] ); 
  glMultMatrixf( view_rotate );

  glScalef( scale, scale, scale );

  /*** Now we render object, using the variables 'obj_type', 'segments', and
    'wireframe'.  These are _live_ variables, which are transparently 
    updated by GLUI ***/

  if ( show_axes )
    draw_axes(.52f);
 glPushMatrix();

  glMultMatrixf( torus_rotate );
  if ( show_torus ){

	for (int loop_m = 0; loop_m < sector1.numtriangles; loop_m++)		// print all triangles
	{
		GLfloat x_m, y_m, z_m, u_m, v_m, w_m;
		GLfloat x_m1, y_m1, z_m1,x_m2, y_m2, z_m2,x_m3, y_m3, z_m3;
		if(wireframe){	
		
			x_m1 = sector1.triangle[loop_m].vertex[0].x;	
			y_m1 = sector1.triangle[loop_m].vertex[0].y;	
			z_m1 = sector1.triangle[loop_m].vertex[0].z;	
			
			x_m2 = sector1.triangle[loop_m].vertex[1].x;	
			y_m2 = sector1.triangle[loop_m].vertex[1].y;	
			z_m2 = sector1.triangle[loop_m].vertex[1].z;	

			x_m3 = sector1.triangle[loop_m].vertex[2].x;	
			y_m3 = sector1.triangle[loop_m].vertex[2].y;	
			z_m3 = sector1.triangle[loop_m].vertex[2].z;	

						
		glBegin(GL_LINES);					
		glVertex3f(x_m1*0.1,y_m1*0.1,z_m1*0.1);
		glVertex3f(x_m2*0.1,y_m2*0.1,z_m2*0.1);
		glColor3f(255.0,0.0,0.0);
		glEnd();						
		glBegin(GL_LINES);					
		glVertex3f(x_m1*0.1,y_m1*0.1,z_m1*0.1);
		glVertex3f(x_m3*0.1,y_m3*0.1,z_m3*0.1);
		glColor3f(0.0,255.0,0.0);
		glEnd();	
		glBegin(GL_LINES);					
		glVertex3f(x_m2*0.1,y_m2*0.1,z_m2*0.1);
		glVertex3f(x_m3*0.1,y_m3*0.1,z_m3*0.1);
		glColor3f(0.0,0.0,255.0);
		glEnd();	
		}
		else{
		glBegin(GL_TRIANGLES);					
		glColor3f(255.0,255.0,0.0);
			x_m = sector1.triangle[loop_m].vertex[0].x;	
			y_m = sector1.triangle[loop_m].vertex[0].y;	
			z_m = sector1.triangle[loop_m].vertex[0].z;	
			u_m = sector1.triangle[loop_m].vertex[0].u;	
			v_m = sector1.triangle[loop_m].vertex[0].v;	
			w_m = sector1.triangle[loop_m].vertex[0].w;	
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);	
			
			
			glColor3f(255.0,255.0,0.0);
			x_m = sector1.triangle[loop_m].vertex[1].x;	
			y_m = sector1.triangle[loop_m].vertex[1].y;	
			z_m = sector1.triangle[loop_m].vertex[1].z;	
			u_m = sector1.triangle[loop_m].vertex[1].u;	
			v_m = sector1.triangle[loop_m].vertex[1].v;	
			w_m = sector1.triangle[loop_m].vertex[1].w;	
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);	
			
			glColor3f(255.0,255.0,0.0);

			x_m = sector1.triangle[loop_m].vertex[2].x;	
			y_m = sector1.triangle[loop_m].vertex[2].y;	
			z_m = sector1.triangle[loop_m].vertex[2].z;	
			u_m = sector1.triangle[loop_m].vertex[2].u;	
			v_m = sector1.triangle[loop_m].vertex[2].v;	
			w_m = sector1.triangle[loop_m].vertex[2].w;	
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);			
			glColor3f(255.0,255.0,0.0);

		glEnd();						//
		}
	 }
  }

   glPopMatrix();

  glPushMatrix();
//  glTranslatef( .5, 0.0, 0.0 );
  glMultMatrixf( sphere_rotate );
   if(show_sphere){
		for (int loop_m = 0; loop_m < sector2.numtriangles; loop_m++)		// information of all triangles
	{
		GLfloat x_m, y_m, z_m, u_m, v_m, w_m;
		glBegin(GL_TRIANGLES);					
		
			x_m = sector2.triangle[loop_m].vertex[0].x;	
			y_m = sector2.triangle[loop_m].vertex[0].y;	
			z_m = sector2.triangle[loop_m].vertex[0].z;	
			u_m = sector2.triangle[loop_m].vertex[0].u;	
			v_m = sector2.triangle[loop_m].vertex[0].v;	
			w_m = sector2.triangle[loop_m].vertex[0].w;	
			glColor3f(1.0,0.0,0.0);
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);	
			
			x_m = sector2.triangle[loop_m].vertex[1].x;	
			y_m = sector2.triangle[loop_m].vertex[1].y;	
			z_m = sector2.triangle[loop_m].vertex[1].z;	
			u_m = sector2.triangle[loop_m].vertex[1].u;	
			v_m = sector2.triangle[loop_m].vertex[1].v;	
			w_m = sector2.triangle[loop_m].vertex[1].w;	
			glColor3f(1.0,0.0,0.0);
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);	

			x_m = sector2.triangle[loop_m].vertex[2].x;	
			y_m = sector2.triangle[loop_m].vertex[2].y;	
			z_m = sector2.triangle[loop_m].vertex[2].z;	
			u_m = sector2.triangle[loop_m].vertex[2].u;	
			v_m = sector2.triangle[loop_m].vertex[2].v;	
			w_m = sector2.triangle[loop_m].vertex[2].w;	
			glColor3f(1.0,0.0,0.0);
			glNormal3f(u_m,v_m,w_m);			
			glVertex3f(x_m*0.1,y_m*0.1,z_m*0.1);	

		glEnd();						// 
	 }
  }

   glPopMatrix();

  if ( show_text ) 
  {
    glDisable( GL_LIGHTING );  /* Disable lighting while we render text */
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0.0, 100.0, 0.0, 100.0  );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glColor3ub( 0, 0, 0 );
    glRasterPos2i( 10, 10 );

    /*  printf( "text: %s\n", text );              */

    /*** Render the live character array 'text' ***/
    int i;
    for( i=0; i<(int)strlen( string_list[curr_string] ); i++ )
      glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, string_list[curr_string][i] );
  }

  glEnable( GL_LIGHTING );


  glutSwapBuffers(); 
}


/**************************************** main() ********************/

int main(int argc, char* argv[])
{
  /****************************************/
  /*   Initialize GLUT and create window  */
  /****************************************/
	SetupWorld();
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  glutInitWindowPosition( 0, 0 );
  glutInitWindowSize( 800, 600 );
 
  main_window = glutCreateWindow( "CSE407 OpenGL Renderer" );
  glutDisplayFunc( myGlutDisplay );
  GLUI_Master.set_glutReshapeFunc( myGlutReshape );  
  GLUI_Master.set_glutKeyboardFunc( myGlutKeyboard );
  GLUI_Master.set_glutSpecialFunc( NULL );
  GLUI_Master.set_glutMouseFunc( myGlutMouse );
  glutMotionFunc( myGlutMotion );

  /****************************************/
  /*       Set up OpenGL lights           */
  /****************************************/

  glEnable(GL_LIGHTING);
  glEnable( GL_NORMALIZE );

  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

  glEnable(GL_LIGHT1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

  /****************************************/
  /*          Enable z-buferring          */
  /****************************************/

  glEnable(GL_DEPTH_TEST);

  /****************************************/
  /*         Here's the GLUI code         */
  /****************************************/

  printf( "GLUI version: %3.2f\n", GLUI_Master.get_version() );

  /*** Create the side subwindow ***/
  glui = GLUI_Master.create_glui_subwindow( main_window, 
					    GLUI_SUBWINDOW_RIGHT );

  obj_panel = new GLUI_Rollout(glui, "Properties", false );

  /***** Control for object params *****/

  new GLUI_Checkbox( obj_panel, "Wireframe", &wireframe, 1, control_cb );
  GLUI_Spinner *spinner = 
    new GLUI_Spinner( obj_panel, "Segments:", &segments);
  spinner->set_int_limits( 3, 60 );
  spinner->set_alignment( GLUI_ALIGN_RIGHT );

  GLUI_Spinner *scale_spinner = 
    new GLUI_Spinner( obj_panel, "Scale:", &scale);
  scale_spinner->set_float_limits( .2f, 4.0 );
  scale_spinner->set_alignment( GLUI_ALIGN_RIGHT );


  /******** Add some controls for lights ********/

  GLUI_Rollout *roll_lights = new GLUI_Rollout(glui, "Lights", false );

  GLUI_Panel *light0 = new GLUI_Panel( roll_lights, "Light 1" );
  GLUI_Panel *light1 = new GLUI_Panel( roll_lights, "Light 2" );

  new GLUI_Checkbox( light0, "Enabled", &light0_enabled,
                     LIGHT0_ENABLED_ID, control_cb );
  light0_spinner = 
    new GLUI_Spinner( light0, "Intensity:", 
                      &light0_intensity, LIGHT0_INTENSITY_ID,
                      control_cb );
  light0_spinner->set_float_limits( 0.0, 1.0 );
  GLUI_Scrollbar *sb;
  sb = new GLUI_Scrollbar( light0, "Red",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[0],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light0, "Green",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[1],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light0, "Blue",GLUI_SCROLL_HORIZONTAL,
                           &light0_diffuse[2],LIGHT0_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  new GLUI_Checkbox( light1, "Enabled", &light1_enabled,
                     LIGHT1_ENABLED_ID, control_cb );
  light1_spinner = 
    new GLUI_Spinner( light1, "Intensity:",
                      &light1_intensity, LIGHT1_INTENSITY_ID,
                      control_cb );
  light1_spinner->set_float_limits( 0.0, 1.0 );
  sb = new GLUI_Scrollbar( light1, "Red",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[0],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light1, "Green",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[1],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);
  sb = new GLUI_Scrollbar( light1, "Blue",GLUI_SCROLL_HORIZONTAL,
                           &light1_diffuse[2],LIGHT1_INTENSITY_ID,control_cb);
  sb->set_float_limits(0,1);


  /*** Add another rollout ***/
  GLUI_Rollout *options = new GLUI_Rollout(glui, "Options", true );
  new GLUI_Checkbox( options, "Draw ligand", &show_sphere );
  new GLUI_Checkbox( options, "Draw protease", &show_torus );
  new GLUI_Checkbox( options, "Draw axes", &show_axes );
  new GLUI_Checkbox( options, "Draw text", &show_text );

  /**** Add listbox ****/
  new GLUI_StaticText( glui, "" );
  GLUI_Listbox *list = new GLUI_Listbox( glui, "Text:", &curr_string );
  int i;
  for( i=0; i<4; i++ )
    list->add_item( i, string_list[i] );

  new GLUI_StaticText( glui, "" );


  /*** Disable/Enable buttons ***/
  new GLUI_Button( glui, "Disable movement", DISABLE_ID, control_cb );
  new GLUI_Button( glui, "Enable movement", ENABLE_ID, control_cb );
  new GLUI_Button( glui, "Hide", HIDE_ID, control_cb );
  new GLUI_Button( glui, "Show", SHOW_ID, control_cb );

  new GLUI_StaticText( glui, "" );

  /****** A 'quit' button *****/
  new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );


  /**** Link windows to GLUI, and register idle callback ******/
  
  glui->set_main_gfx_window( main_window );


  /*** Create the bottom subwindow ***/
  glui2 = GLUI_Master.create_glui_subwindow( main_window, 
                                             GLUI_SUBWINDOW_BOTTOM );
  glui2->set_main_gfx_window( main_window );

  GLUI_Rotation *view_rot = new GLUI_Rotation(glui2, "Objects", view_rotate );
  view_rot->set_spin( 1.0 );
  new GLUI_Column( glui2, false );
  GLUI_Rotation *sph_rot = new GLUI_Rotation(glui2, "Ligand", sphere_rotate );
  sph_rot->set_spin( .98 );
  new GLUI_Column( glui2, false );
  GLUI_Rotation *tor_rot = new GLUI_Rotation(glui2, "Protease", torus_rotate );
  tor_rot->set_spin( .98 );
  new GLUI_Column( glui2, false );
  GLUI_Rotation *lights_rot = new GLUI_Rotation(glui2, "Blue Light", lights_rotation );
  lights_rot->set_spin( .82 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_xy = 
    new GLUI_Translation(glui2, "Objects XY", GLUI_TRANSLATION_XY, obj_pos );
  trans_xy->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_x = 
    new GLUI_Translation(glui2, "Objects X", GLUI_TRANSLATION_X, obj_pos );
  trans_x->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_y = 
    new GLUI_Translation( glui2, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
  trans_y->set_speed( .005 );
  new GLUI_Column( glui2, false );
  GLUI_Translation *trans_z = 
    new GLUI_Translation( glui2, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
  trans_z->set_speed( .005 );
  checkbox = 
    new GLUI_Checkbox(obj_panel, "Wireframe", &wireframe, 1, control_cb );

#if 0
  /**** We register the idle callback with GLUI, *not* with GLUT ****/
  GLUI_Master.set_glutIdleFunc( myGlutIdle );
#endif

  /**** Regular GLUT main loop ****/
  
  glutMainLoop();
  destroyWorld();
  return EXIT_SUCCESS;
}


