/*
 * ---------------- www.spacesimulator.net --------------
 *   ---- Space simulators and 3d engine tutorials ----
 *
 * Author: Damiano Vitulli
 *
 * This program is released under the BSD licence
 * By using this program you agree to licence terms on spacesimulator.net copyright page
 *
 *
 * Tutorial 5: Vectors and OpenGL lighting
 * 
 * 
 *
 * To compile this project you must include the following libraries:
 * opengl32.lib,glu32.lib,glut32.lib
 * You need also the header file glut.h in your compiler directory.
 *  
 */

/*
 * Spaceships credits:
 * fighter1.3ds - created by: Dario Vitulli 
 */


#include <windows.h>
#include <GL/glut.h>
#include "load_bmp.h"
#include "load_3ds.h"
#include "object.h"
#include <time.h>
#include <stdio.h>
#define MAX_CHAR       128

GLfloat angle;
bool time_up = false;

/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

// The width and height of your window, change them as you like
int screen_width=640;
int screen_height=480;

// Absolute rotation values (0-359 degrees) and rotation increments for each frame
double rotation_x = 0, rotation_x_increment=0;
double rotation_y = 0, rotation_y_increment = 0;
double rotation_z = 0, rotation_z_increment = 0;

double x = 0, x2 = 0;
double y = 0, y2 = 0;
double z = 0, z2 = -0.005;
double zc = 0, zc2 = 0.001, yc = 0, yc2 = 0.001;
int score = 0;
 
// Flag for rendering as lines or filled polygons
int filling=1; //0=OFF 1=ON

//Lights settings
GLfloat light_ambient[]= { 0.1f, 0.1f, 0.1f, 0.1f };
GLfloat light_diffuse[]= { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat light_specular[]= { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat light_position[]= { 100.0f, 0.0f, -10.0f, 1.0f };

//Materials settings
GLfloat mat_ambient[]= { 0.1f, 0.1f, 0.1f, 0.0f };
GLfloat mat_diffuse[]= { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat mat_specular[]= { 0.2f, 0.2f, 0.2f, 0.0f };
GLfloat mat_shininess[]= { 1.0f };

void drawString(const char* str) {

    static int isFirstCall = 1;
    static GLuint lists;

    if (isFirstCall) { // 如果是第一次調用，執行初始化

        // 爲每一個ASCII字符產生一個顯示列表

        isFirstCall = 0;

        // 申請MAX_CHAR個連續的顯示列表編號

        lists = glGenLists(MAX_CHAR);

        // 把每個字符的繪製命令都裝到對應的顯示列表中

        wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);

    }

    // 調用每個字符對應的顯示列表，繪製每個字符

    for (; *str != '\0'; ++str)
        glCallList(lists + *str);

}

void selectFont(int size, int charset, const char* face) {

    HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,

        charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,

        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);

    HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);

    DeleteObject(hOldFont);

}

/**********************************************************
 *
 * SUBROUTINE init(void)
 *
 * Used to initialize OpenGL and to setup our world
 *
 *********************************************************/

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0); // Clear background color to black

    // Viewport transformation
    glViewport(0,0,screen_width,screen_height);  

    // Projection transformation
    glMatrixMode(GL_PROJECTION); // Specifies which matrix stack is the target for matrix operations 
    glLoadIdentity(); // We initialize the projection matrix as identity
    gluPerspective(45.0f,(GLfloat)screen_width/(GLfloat)screen_height,5.0f,10000.0f); // We define the "viewing volume"

	//Lights initialization and activation
    glLightfv (GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv (GL_LIGHT1, GL_SPECULAR, light_specular);
    glLightfv (GL_LIGHT1, GL_POSITION, light_position);    
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);

    //Materials initialization and activation
	glMaterialfv (GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv (GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv (GL_FRONT, GL_SHININESS, mat_shininess);    

	//Other initializations
    glShadeModel(GL_SMOOTH); // Type of shading for the polygons
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Texture mapping perspective correction (OpenGL... thank you so much!)
    glEnable(GL_TEXTURE_2D); // Texture mapping ON
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); // Polygon rasterization mode (polygon filled)
	glEnable(GL_CULL_FACE); // Enable the back face culling
    glEnable(GL_DEPTH_TEST); // Enable the depth test (also called z buffer)

	//Objects loading
	ObjLoad ("fighter1.3ds","skull.bmp");
}

/**********************************************************
 *
 * SUBROUTINE resize(int p_width, int p_height)
 *
 * This routine must be called everytime we resize our window.
 *
 * Input parameters: p_width = width in pixels of our viewport
 *					 p_height = height in pixels of our viewport
 * 
 *********************************************************/

void resize (int p_width, int p_height)
{
	if (screen_width==0 && screen_height==0) exit(0);
    screen_width=p_width; // We obtain the new screen width values and store it
    screen_height=p_height; // Height value

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // We clear both the color and the depth buffer so to draw the next frame
    glViewport(0,0,screen_width,screen_height); // Viewport transformation

    glMatrixMode(GL_PROJECTION); // Projection transformation
    glLoadIdentity(); // We initialize the projection matrix as identity
    gluPerspective(45.0f,(GLfloat)screen_width/(GLfloat)screen_height,5.0f,10000.0f);

    glutPostRedisplay (); // This command redraw the scene (it calls the same routine of glutDisplayFunc)
}

/**********************************************************
 *
 * SUBROUTINE keyboard(unsigned char p_key, int p_x, int p_y)
 *
 * Used to handle the keyboard input (ASCII Characters)
 *
 * Input parameters: p_key = ascii code of the key pressed
 *					 p_x = not used, keeped to maintain compatibility with glut function
 *                   p_y = not used, keeped to maintain compatibility with glut function
 * 
 *********************************************************/

void keyboard(unsigned char p_key, int p_x, int p_y)
{  
    switch (p_key)
    {
        case ' ':
            //rotation_x_increment=0;
            //rotation_y_increment=0;
            //rotation_z_increment=0;
            //x2 = 0;
            //y2 = 0;
            //z2 = 0;
        break;
		case 'r': case 'R':
            if (filling==0)
            {
                glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); // Polygon rasterization mode (polygon filled)
                filling=1;
            }   
            else 
            {
                glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); // Polygon rasterization mode (polygon outlined)
                filling=0;
            }
        break;
        case 'w': case 'W':
            y2 = 0.005;
            break;
        case 's': case 'S':
            y2 = -0.005;
            break;
        case 'a': case 'A':
            x2 = -0.005;
            break;
        case 'd': case 'D':
            x2 = 0.005;
            break;
        case 'p': case 'P':
			exit(0);
		break;
    }
}

/**********************************************************
 *
 * SUBROUTINE keyboard(int p_key, int p_x, int py)
 *
 * Used to handle the keyboard input (not ASCII Characters)
 *
 * Input parameters: p_key = code of the key pressed
 *					 p_x = not used, keeped to maintain compatibility with glut function
 *                   p_y = not used, keeped to maintain compatibility with glut function
 * 
 *********************************************************/

void keyboard_s(int p_key, int p_x, int py)
{
    switch (p_key)
    {
        case GLUT_KEY_UP:
            //rotation_x_increment = rotation_x_increment +0.005;
            rotation_x_increment = 0.005;
        break;
        case GLUT_KEY_DOWN:
            //rotation_x_increment = rotation_x_increment -0.005;
            rotation_x_increment = - 0.005;
        break;
        case GLUT_KEY_LEFT:
            //rotation_y_increment = rotation_y_increment +0.005;
            rotation_y_increment = 0.005;
        break;
        case GLUT_KEY_RIGHT:
            //rotation_y_increment = rotation_y_increment -0.005;
            rotation_y_increment = - 0.005;
        break;
    }
}

void ball(double a, double b, double c, double d)
{
    GLUquadricObj* quadricPtr;
    quadricPtr = gluNewQuadric();
    gluQuadricDrawStyle(quadricPtr, GL_FILL);
    gluQuadricNormals(quadricPtr, GLU_NONE);
    gluQuadricTexture(quadricPtr, GL_FALSE);
    /*if (z <= c && z >= c - 0.1) {
        score += 1;
    }*/
    if ((b + d + 1.2 > y && b - d - 2 < y) && (z <= c && z >= c - 0.1) && (a + d + 6.2 > x && a + d - 6.2 < x)) {
        score += 1;
    }
    glPushMatrix();
    glColor3f(0, 1, 0);
    glTranslatef(a, b, c);
    gluSphere(quadricPtr, d, 15, 10);
    glPopMatrix();
}

int timerDuration = 30;
void update(int value) 
{
    glutTimerFunc(1000, update, 0);
    timerDuration--;
    if (timerDuration <= 0) {
        time_up = true;
        glutIdleFunc(NULL);
    }
    glutPostRedisplay();
}

/**********************************************************
 *
 * SUBROUTINE display(void)
 *
 * This is our main rendering subroutine, called each frame
 * 
 *********************************************************/

void display(void)
{
    int j;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This clear the background color to dark blue
    glMatrixMode(GL_MODELVIEW); // Modeling transformation
    glLoadIdentity(); // Initialize the model matrix as identity

    glTranslatef(0.0,0.0,-35); // We move the object forward (the model matrix is multiplied by the translation matrix)
    
    selectFont(30, ANSI_CHARSET, "Comic Sans MS");
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(-18, 12, 0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glRasterPos2f(0.0f, 0.0f);
    drawString("SCORE:");
    glPopMatrix();
    char SCORE[10];
    sprintf(SCORE, "%d", score);
    for (int i = 0; i < strlen(SCORE); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, SCORE[i]);
    }

    glPopMatrix();
    glPushMatrix();
    glTranslatef(9, 12, 0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glRasterPos2f(0.0f, 0.0f);
    drawString("TIME:");
    glPopMatrix();
    char numberStr[10];
    if (timerDuration <= 0) {
        timerDuration = 0;
    }
    sprintf(numberStr, "%d", timerDuration);
    for (int i = 0; i < strlen(numberStr); i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, numberStr[i]);
    }
    

    if (time_up) {
        glPushMatrix();
        glTranslatef(-3, 5, 0);
        glColor3f(0.0f, 0.0f, 1.0f);
        glRasterPos2f(0.0f, 0.0f);
        drawString("TIME UP");
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-5, -8, 0);
        glColor3f(0.0f, 0.0f, 1.0f);
        glRasterPos2f(0.0f, 0.0f);
        drawString("FINAL SCORE:");
        glPopMatrix();
        char SCORE[10];
        sprintf(SCORE, "%d", score);
        for (int i = 0; i < strlen(SCORE); i++) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, SCORE[i]);
        }
    }
    yc = yc + yc2;
    zc = zc + zc2;
    z = z + z2;
    glTranslatef(0, 0, -z);

    ball(0, 2, -1, 1);
    ball(-3, -2, -3, 1);
    ball(3, 2, -10, 1.5);
    ball(-12, 2, -20, 1);
    ball(-7, 7, -30, 2);
    ball(-7, 7, -130, 2);
    ball(-2, -12, -40, 1);
    ball(12, -15, -45, 2);
    ball(-2, 2, -50, 1);
    ball(1, 15, -55, 2);
    ball(-3, -5, -90, 1);
    ball(3, 6, -165, 1.5);
    ball(-3, 6, -300, 1.5);
    ball(-2, 3, -120, 3);
    ball(2, 2, -155, 2);
    ball(-12, 2, -170, 1);
    ball(-3, -3, -180, 1);
    ball(-7, -7, -205, 2);
    ball(-1, -15, -55, 2);
    ball(-6, 5, -90, 1);
    ball(3, -6, -165, 1.5);
    ball(-2, 3, -120, 1);
    ball(5, -2, -155, 2.5);
    ball(-6, 3, -180, 1);
    ball(-6, 3, -370, 1);
    ball(-7, -2, -225, 2);
    ball(-2, 2, -250, 1);
    ball(10, -3, -325, 2);
    ball(7, -5, -355, 2);
    ball(-9, 2, -380, 1);
    ball(-3, -5, -400, 1);
    ball(2, 2, -450, 1);
    ball(8, 12, -340, 1);
    ball(-7, 2, -460, 1);
    ball(1, -15, -490, 2);
    ball(-6, 8, -460, 1);
    ball(-6, -8, -460, 1);

    x = x + x2;
    y = y + y2;
    z = z + z2;
    glTranslatef(x, y, z);

    rotation_x = rotation_x + rotation_x_increment;
    rotation_y = rotation_y + rotation_y_increment;
    rotation_z = rotation_z + rotation_z_increment;

    if (rotation_x > 359) rotation_x = 0;
    if (rotation_y > 359) rotation_y = 0;
    if (rotation_z > 359) rotation_z = 0;

    glRotatef(rotation_x,1.0,0.0,0.0); // Rotations of the object (the model matrix is multiplied by the rotation matrices)
    glRotatef(rotation_y,0.0,1.0,0.0);
    glRotatef(rotation_z,0.0,0.0,1.0);

	if (object.id_texture!=-1) 
	{
		glBindTexture(GL_TEXTURE_2D, object.id_texture); // We set the active texture 
	    glEnable(GL_TEXTURE_2D); // Texture mapping ON
	}
	else
	    glDisable(GL_TEXTURE_2D); // Texture mapping OFF

	glBegin(GL_TRIANGLES); // glBegin and glEnd delimit the vertices that define a primitive (in our case triangles)
	for (j=0;j<object.polygons_qty;j++)
	{
		//----------------- FIRST VERTEX -----------------
		//Normal coordinates of the first vertex
		glNormal3f( object.normal[ object.polygon[j].a ].x,
					object.normal[ object.polygon[j].a ].y,
					object.normal[ object.polygon[j].a ].z);
		// Texture coordinates of the first vertex
		glTexCoord2f( object.mapcoord[ object.polygon[j].a ].u,
					  object.mapcoord[ object.polygon[j].a ].v);
		// Coordinates of the first vertex
		glVertex3f( object.vertex[ object.polygon[j].a ].x,
					object.vertex[ object.polygon[j].a ].y,
					object.vertex[ object.polygon[j].a ].z);

		//----------------- SECOND VERTEX -----------------
		//Normal coordinates of the second vertex
		glNormal3f( object.normal[ object.polygon[j].b ].x,
					object.normal[ object.polygon[j].b ].y,
					object.normal[ object.polygon[j].b ].z);
		// Texture coordinates of the second vertex
		glTexCoord2f( object.mapcoord[ object.polygon[j].b ].u,
					  object.mapcoord[ object.polygon[j].b ].v);
		// Coordinates of the second vertex
		glVertex3f( object.vertex[ object.polygon[j].b ].x,
					object.vertex[ object.polygon[j].b ].y,
					object.vertex[ object.polygon[j].b ].z);
        
		//----------------- THIRD VERTEX -----------------
		//Normal coordinates of the third vertex
		glNormal3f( object.normal[ object.polygon[j].c ].x,
					object.normal[ object.polygon[j].c ].y,
					object.normal[ object.polygon[j].c ].z);
		// Texture coordinates of the third vertex
		glTexCoord2f( object.mapcoord[ object.polygon[j].c ].u,
					  object.mapcoord[ object.polygon[j].c ].v);
		// Coordinates of the Third vertex
		glVertex3f( object.vertex[ object.polygon[j].c ].x,
					object.vertex[ object.polygon[j].c ].y,
					object.vertex[ object.polygon[j].c ].z);
	}
	glEnd();
    glFlush(); // This force the execution of OpenGL commands
    
    glutSwapBuffers(); // In double buffered mode we invert the positions of the visible buffer and the writing buffer
}



/**********************************************************
 *
 * The main routine
 * 
 *********************************************************/

int main(int argc, char **argv)
{
    // We use the GLUT utility to initialize the window, to handle the input and to interact with the windows system
    time_t startTime = time(NULL);
    glutInit(&argc, argv);    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screen_width,screen_height);
    glutInitWindowPosition(0,0);
    glutCreateWindow("www.spacesimulator.net - 3d engine tutorials - To exit press ESC");  
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc (resize);
    glutKeyboardFunc (keyboard);
    glutSpecialFunc (keyboard_s);
    init();
    glutTimerFunc(1000, update, 0);
    glutMainLoop();
    return(0);    
}
