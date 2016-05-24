// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>

using namespace std;

#define DEF_floorGridScale  1.0
#define DEF_floorGridXSteps 10.0
#define DEF_floorGridZSteps 10.0

GLfloat ctlpointsNurbsSurf[21][21][3];

GLfloat knotsSurf[25] = {
    0.0,0.0,0.0,0.0,0.10,0.15,0.20,0.25,0.30,0.35,0.40,0.45,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,1.0,1.0,1.0,1.0
};

GLfloat L[2] = {0.0,0.0};
GLfloat A[2] = {0.0,0.0};
GLfloat S[2] = {0.0,0.0};
GLfloat D[2] = {0.0,0.0};

GLfloat W[2];
GLfloat phi[2];

float pi = 3.141592;

GLUnurbsObj *theNurb;

float t;

void ejesCoordenada() {
    
    glLineWidth(2.5);
    glBegin(GL_LINES);
        glColor3f(1.0,0.0,0.0);
        glVertex2f(0,10);
        glVertex2f(0,-10);
        glColor3f(0.0,0.0,1.0);
        glVertex2f(10,0);
        glVertex2f(-10,0);
    glEnd();

    glLineWidth(1.5);
    int i;
    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
        for(i = -10; i <=10; i++){
            if (i!=0) {     
                if ((i%2)==0){  
                    glVertex2f(i,0.4);
                    glVertex2f(i,-0.4);

                    glVertex2f(0.4,i);
                    glVertex2f(-0.4,i);
                }else{
                    glVertex2f(i,0.2);
                    glVertex2f(i,-0.2);

                    glVertex2f(0.2,i);
                    glVertex2f(-0.2,i);

                }
            }
        }
        
    glEnd();

    glLineWidth(1.0);
}

void changeViewport(int w, int h) {
    
    float aspectratio;

    if (h==0)
        h=1;

    
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(30, (GLfloat) w/(GLfloat) h, 1.0, 200.0);
   glMatrixMode (GL_MODELVIEW);

}

void init_surface() {
    
    GLfloat z = 10.0;
    for (int i=0; i < 21; i++)
    {
        GLfloat x = 10.0;
        for (int j=0; j < 21; j++)
        {
            ctlpointsNurbsSurf[i][j][0] = x;
            ctlpointsNurbsSurf[i][j][1] = 0.0;
            ctlpointsNurbsSurf[i][j][2] = z;
            x -= 1.0;
        }
        z -= 1.0;
    }
}

void init(){

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    init_surface();

    theNurb = gluNewNurbsRenderer();
    gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 15.0);
    gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);
    init_surface();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    // glutTimerFunc(10,changePoints,1);
    // t = 0.0;
}

void Keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:             
        exit (0);
        break;
    
  }
}

void render(){
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat zExtent, xExtent, xLocal, zLocal;
    int loopX, loopZ;

    glLoadIdentity ();                       
    gluLookAt (25.0, 12.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Luz y material

    GLfloat mat_diffuse[] = { 0.6, 0.6, 0.9, 1.0 };
    GLfloat mat_specular[] = { 0.8, 0.8, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 60.0 };
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    

    GLfloat light_ambient[] = { 0.0, 0.0, 0.2, 1.0 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_specular[] = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat light_position[] = { -10.0, 5.0, 0.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);   

    // Render Grid 
    glDisable(GL_LIGHTING);
    glLineWidth(1.0);
    glPushMatrix();
    glRotatef(90,1.0,0.0,0.0);
    glColor3f( 0.0, 0.7, 0.7 );
    glBegin( GL_LINES );
    zExtent = DEF_floorGridScale * DEF_floorGridZSteps;
    for(loopX = -DEF_floorGridXSteps; loopX <= DEF_floorGridXSteps; loopX++ )
    {
    xLocal = DEF_floorGridScale * loopX;
    glVertex3f( xLocal, -zExtent, 0.0 );
    glVertex3f( xLocal, zExtent,  0.0 );
    }
    xExtent = DEF_floorGridScale * DEF_floorGridXSteps;
    for(loopZ = -DEF_floorGridZSteps; loopZ <= DEF_floorGridZSteps; loopZ++ )
    {
    zLocal = DEF_floorGridScale * loopZ;
    glVertex3f( -xExtent, zLocal, 0.0 );
    glVertex3f(  xExtent, zLocal, 0.0 );
    }
    glEnd();
    ejesCoordenada();
    glPopMatrix();
    glEnable(GL_LIGHTING);
    // Fin Grid
    
    //Suaviza las lineas
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_LINE_SMOOTH ); 

    glPushMatrix();

    gluBeginSurface(theNurb);
    
    gluNurbsSurface(theNurb, 
                   25, knotsSurf, 25, knotsSurf,
                   21 * 3, 3, &ctlpointsNurbsSurf[0][0][0], 
                   4, 4, GL_MAP2_VERTEX_3);

    gluEndSurface(theNurb);
    
    glPopMatrix();
    
    
    /* Muestra los puntos de control */
    
        int i,j;
        glPointSize(5.0);
        glDisable(GL_LIGHTING);
        glColor3f(1.0, 1.0, 0.0);
        glBegin(GL_POINTS);
        for (i = 0; i <21; i++) {
            for (j = 0; j < 21; j++) {
                glVertex3f(ctlpointsNurbsSurf[i][j][0],  ctlpointsNurbsSurf[i][j][1], ctlpointsNurbsSurf[i][j][2]);
            }
        }
        glEnd();
        glEnable(GL_LIGHTING);
    
        

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    glutSwapBuffers();
}

void animacion(int value) {

    t+=0.01;

    // Formula
    
    glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
    
}

int main (int argc, char** argv) {

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutInitWindowSize(960,540);

    glutCreateWindow("Nurbs Proyecto - Ola");

    init ();

    glutReshapeFunc(changeViewport);
    glutDisplayFunc(render);
    glutKeyboardFunc (Keyboard);
        
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "GLEW error");
        return 1;
    }
    
    glutMainLoop();
    return 0;

}
