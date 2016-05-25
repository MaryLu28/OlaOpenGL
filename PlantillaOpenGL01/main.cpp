// Cubica

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <iostream>
#include <map>            // Librería que me permite utilizar hash.
#include <cmath>

using namespace std;

#define DEF_floorGridScale  1.0
#define DEF_floorGridXSteps 10.0
#define DEF_floorGridZSteps 10.0

struct direccionOla 
{
	GLfloat x;
	GLfloat z;
};

GLfloat ctlpointsNurbsSurf[21][21][3];

// Knots para la creacion de la superficie
GLfloat knotsSurf[25] = {
    0.0,0.0,0.0,0.0,0.10,0.15,0.20,0.25,0.30,0.35,0.40,0.45,0.50,0.55,0.60,0.65,0.70,0.75,0.80,0.85,0.90,1.0,1.0,1.0,1.0
};

// Variables de la formula
GLfloat L[2] = {8.0,4.0};
GLfloat A[2] = {0.4,0.0};
GLfloat S[2] = {2.0,0.0};
direccionOla D[2];
direccionOla Dnormal[2];

GLfloat W[2] = {0.0, 0.0};
GLfloat phi[2] = {0.0, 0.0};

float pi = 3.141592;

GLUnurbsObj *theNurb;

float t;

int idOla = 0;

bool activado = false;

/*
	Descripción:
		Permite capturar que tecla es presionada y de esta forma
		se podrá encadenar la acción correspondiente.
*/
void teclaPresionada(unsigned char tecla, int x, int y)
{
	switch (tecla)
	{
		// Teclas para cambiar los valores de la ola.

		case 'a':
		case 'A':
			if (idOla != 0)
			{
				L[idOla - 1] -= 0.1;
				W[idOla - 1] = 2*pi / L[idOla - 1];
				phi[idOla - 1] = S[idOla - 1] * (2*pi / L[idOla - 1]);
			}
			break;
		case 'z':
		case 'Z':
			if (idOla != 0) 
			{
				L[idOla - 1] += 0.1;
				W[idOla - 1] = 2*pi / L[idOla - 1];
				phi[idOla - 1] = S[idOla - 1] * (2*pi / L[idOla - 1]);
			}
			break;
		case 's':
		case 'S':
			if (idOla != 0) A[idOla - 1] -= 0.1;
			break;
		case 'x':
		case 'X':
			if (idOla != 0) A[idOla - 1] += 0.1;
			break;
		case 'd':
		case 'D':
			if (idOla != 0){
				S[idOla - 1] -= 0.1;
				phi[idOla - 1] = S[idOla - 1] * (2*pi / L[idOla - 1]);
			}
			break;
		case 'c':
		case 'C':
			if (idOla != 0){
				S[idOla - 1] += 0.1;
				phi[idOla - 1] = S[idOla - 1] * (2*pi / L[idOla - 1]);
			};
			break;
		case 'f':
		case 'F':
			if (idOla != 0) D[idOla - 1].x -= 0.1;
			break;
		case 'v':
		case 'V':
			if (idOla != 0) D[idOla - 1].x += 0.1;
			break;
		case 'g':
		case 'G':
			if (idOla != 0) D[idOla - 1].z -= 0.1;
			break;
		case 'b':
		case 'B':
			if (idOla != 0) D[idOla - 1].z += 0.1;
			break;

		// Tecla para  comenzar la animación.
		case 'r':
		case 'R':
            activado = true;
			break;
		
		// Tecla para detener la animación.
		case 'p':
		case 'P':
            activado = false;
			break;

		// Teclas para seleccionar la ola.
		case '1':
			idOla = 1;
			break;
		case '2': 
			idOla = 2;
			break;
	}
}


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
    
    GLfloat z = 10.0; // primer valor de Z para los puntos de control
    // Creacion de los puntos de control
    for (int i=0; i < 21; i++)
    {   
        GLfloat x = 10.0; // primer valor de X para los puntos de control
        for (int j=0; j < 21; j++)
        {
            ctlpointsNurbsSurf[i][j][0] = x;    // coordenada X del punto de control actual
            ctlpointsNurbsSurf[i][j][1] = 0.0;  // coordenada Y del punto de control actual
            ctlpointsNurbsSurf[i][j][2] = z;    // coordenada Z del punto de control actual
            x -= 1.0;   // despazamiendo en el eje X de los puntos de control
        }
        z -= 1.0;   // desplazamiento en el eje Z de los puntos de control
    }
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

	D[0].x = 0.0;
	D[0].z = -1.0;
	D[1].x = 1.0;
	D[1].z = 1.0;

    GLfloat zExtent, xExtent, xLocal, zLocal;
    int loopX, loopZ;

    glLoadIdentity ();                       
    gluLookAt (25.0, 12.0, 4.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glutKeyboardFunc(teclaPresionada);

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
    /*glDisable(GL_LIGHTING);
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
    glEnable(GL_LIGHTING);*/
    // Fin Grid
    
    //Suaviza las lineas
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_LINE_SMOOTH ); 

    glPushMatrix();

    gluBeginSurface(theNurb);
    
    // creacion de la superficie
    gluNurbsSurface(theNurb, 
                   25, knotsSurf, 25, knotsSurf,
                   21 * 3, 3, &ctlpointsNurbsSurf[0][0][0], 
                   4, 4, GL_MAP2_VERTEX_3);

    gluEndSurface(theNurb);
    
    glPopMatrix();
    
    
    /* Muestra los puntos de control */
    
    /*    int i,j;
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
        glEnable(GL_LIGHTING);*/
    
        

    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    glutSwapBuffers();
}

void animacion(int value) {

	GLfloat normal0 = 1 / sqrt(pow(D[0].x,2)+pow(D[0].z,2));
	GLfloat normal1 = 1 / sqrt(pow(D[1].x,2)+pow(D[1].z,2));
	Dnormal[0].x = D[0].x * normal0;
	Dnormal[0].z = D[0].z * normal0;
	Dnormal[1].x = D[1].x * normal1;
	Dnormal[1].z = D[1].z * normal1;

	float sum0 = 0.0;
	float sum1 = 0.0;

	for (int i=0; i < 21; i++)
    {
        for (int j=0; j < 21; j++)
        {
			float wDnormal0x = ctlpointsNurbsSurf[i][j][0] * W[0];
			float wDnormal0z = ctlpointsNurbsSurf[i][j][2] * W[0];

			float wDnormal1x = ctlpointsNurbsSurf[i][j][0] * W[1];
			float wDnormal1z = ctlpointsNurbsSurf[i][j][2] * W[1];

			sum0 = A[0] * sin((Dnormal[0].x * wDnormal0x + Dnormal[0].z * wDnormal0z) + (t * phi[0]));
			sum1 = A[1] * sin((Dnormal[1].x * wDnormal1x + Dnormal[1].z * wDnormal1z) + (t * phi[1]));
			ctlpointsNurbsSurf[i][j][1] = sum0 + sum1;
		}
    }

    t+=0.1;

    glutTimerFunc(10,animacion,1);
    glutPostRedisplay();
    
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	glutTimerFunc(10,animacion,1);
    t = 0.0;
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
