#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <cassert>
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;


// You will need more global variables to implement color and position changes


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }


int col_idx = 0;
int theta = 0;
bool start = false;
bool gMousePressed = false;
int interval = 100;
int list_id;
int gButton;

#define PI 3.14159265
void timeFunc(int t)
{
    if(start) {
        theta += 5;
        theta %= 360;	
    }
    glutPostRedisplay();
    glutTimerFunc(interval, timeFunc, 0);
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        col_idx = (++col_idx)%4;
        break;
    case 'r':
	    if(start) start = false;
	    else  start = true;
	    break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

GLfloat Lt0pos[] = {1.0f, 1.0f, 5.0f, 1.0f};
// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        Lt0pos[1] += 0.5;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        Lt0pos[1] -= 0.5;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        Lt0pos[0] -= 0.5;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        Lt0pos[0] += 0.5;
		break;
    }
	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}


int     width, height;
int     startClickX, startClickY;
Matrix4f startRot;
Matrix4f currentRot;

void mouseFunc(int button, int state, int x, int y)
{
	startClickX = x;
	startClickY = y;
	gButton = button;

	if(state == GLUT_DOWN) {
		gMousePressed = true;	
		switch(button) {
		case GLUT_LEFT_BUTTON:
			currentRot = startRot;
			break;
		case GLUT_MIDDLE_BUTTON:
		case GLUT_RIGHT_BUTTON:
		default:
			break;
		}
	}
	else {
		startRot = currentRot;
		gMousePressed = false;
	}
//	glutPostRedisplay();
}

void arcBallRotation(int x, int y)
{
	float sx, sy, sz;
	float ex, ey, ez;
	float sl, el;
	float scale, dotp;

	sx = startClickX - width / 2.0;	// the center of the windows is origin 
	sy = startClickY - height / 2.0;	
	ex = x - width / 2.0;
	ey = y - height / 2.0;
	
	sy = -sy; // Y is up
	ey = -ey;

	scale = (float)(width > height ? height: width);
	scale = 1.0 / scale;
	
	sx *= scale;
	sy *= scale;
	ex *= scale;
	ey *= scale;
	
	sl = hypot(sx, sy);
	el = hypot(ex, ey);
	if(sl > 1.0) {
		sx /= sl;
		sy /= sl;
		sl = 1.0;
	}
	if(el > 1.0) {
		ex /= el;
		sy /= el;
		el = 1.0;
	}
	// project to a unit sphere;
	sz = sqrt(1.0 - sl * sl);
	ez = sqrt(1.0 - el * el);
	
	dotp = sx * ex + sy * ey + sz * ez;
	if(dotp !=  1) { // if parrallel
		Vector3f axis(sy * ez - ey * sz, sz * ex - ez * sx, sx * ey - ex * sy);
		axis.normalize();
		float angle =  acos(dotp);// why 2 ??
		currentRot = Matrix4f::rotation(axis, angle);
		currentRot = currentRot * startRot;
	}
	else {
		currentRot = startRot;
	}
}

void motionFunc(int x, int y)
{
	switch(gButton){
	case GLUT_LEFT_BUTTON:
		arcBallRotation(x, y);
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	default:
		break;	
	}
	glutPostRedisplay();
}


// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
#if 0
    gluLookAt(5.0 * sin(theta * PI/180), 0.0, 5.0 * cos(theta * PI/180),
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);// we may rotate the camera or the object
#endif
#if 1
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
#endif
    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[col_idx]);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	glPushMatrix(); // push matrix here so will not disturbe the light position

	glMultMatrixf(currentRot); // here mult and rotate can not exchange
								// please check why ??
    glRotated(theta, 0, 1, 0); // we may rotate the object or the camera

    glCallList(list_id);
	glPopMatrix();
	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
//	glutSolidTeapot(1.0);
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.

    list_id = glGenLists(1);
    assert(list_id != 0);

    glNewList(list_id, GL_COMPILE);
// better to use vertex array
    vector<vector<unsigned> >::iterator itr0;
        glBegin(GL_TRIANGLES);
            for(itr0=vecf.begin(); itr0 != vecf.end(); ++itr0){
                glNormal3f(vecn[(*itr0)[3]-1][0], vecn[(*itr0)[3]-1][1],
                           vecn[(*itr0)[3]-1][2]);
                glVertex3f(vecv[(*itr0)[0]-1][0], vecv[(*itr0)[0]-1][1], 
                           vecv[(*itr0)[0]-1][2]);
                glNormal3f(vecn[(*itr0)[4]-1][0], vecn[(*itr0)[4]-1][1],
                           vecn[(*itr0)[4]-1][2]);
                glVertex3f(vecv[(*itr0)[1]-1][0], vecv[(*itr0)[1]-1][1], 
                           vecv[(*itr0)[1]-1][2]);
                glNormal3f(vecn[(*itr0)[5]-1][0], vecn[(*itr0)[5]-1][1],
                           vecn[(*itr0)[5]-1][2]);
                glVertex3f(vecv[(*itr0)[2]-1][0], vecv[(*itr0)[2]-1][1], 
                           vecv[(*itr0)[2]-1][2]);
            }
        glEnd();
    glEndList();
	startRot = Matrix4f::identity();
	currentRot = Matrix4f::identity();
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

	width = w;
	height = h;
    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}


#define MAX_SIZE 256
void loadInput()
{
	// load the OBJ file here
	char buffer[MAX_SIZE];
	while(cin.getline(buffer, MAX_SIZE)) {
		stringstream ss(buffer);
		Vector3f v;
		string s;

		ss >> s;
		if(s == "v"){
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
		}
		else if(s == "vn") {
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
		}
		else if(s == "f"){
			int a, b, c, d, e, f, g, h, i;
			char ch;
			vector<unsigned> vec;
			ss  >> a >> ch >> b >> ch >> c;
			ss	>> d >> ch >> e >> ch >> f;
			ss	>> g >> ch >> h >> ch >> i;
			// i changed the sequence maybe better for vertex array
			// the vertex index
			vec.push_back(a); vec.push_back(d); vec.push_back(g);
			// not used now
			vec.push_back(c); vec.push_back(f); vec.push_back(i);
			// the normal index 
			vec.push_back(b); vec.push_back(e); vec.push_back(h);
			vecf.push_back(vec);
	
		}
		else {
			continue;	
		}
	}
	return;
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );
    // Start the main loop.  glutMainLoop never returns.
//    glutIdleFunc(timeFunc);
    glutTimerFunc(interval, timeFunc, 0);
	
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);

    glutMainLoop( );

    return 0;	// This line is never reached.
}
