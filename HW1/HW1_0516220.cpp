#include "glut.h"
#include <Windows.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

void display();
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);
void passiveMouseMotion(int x, int y);
void idle();

int width = 800, height = 450;
int r = 1;
int degree = 120;
bool pause = false;

int earth_slices = 360;
int earth_stacks = 180;
int other_slices = 240;
int other_stacks = 60;
int earth_rotation = 0;
int earth_revolution_cnt = 0;
float earth_revolution = 0.0;

int moon_cnt = 0;
float moon_rotation = 0.0;
float moon_revolution = 0.0;
float pi = M_PI/180;

void drawSphere(GLfloat radius, GLint slices, GLint stacks, bool show_axis){
	float phi, theta;

	for(theta = 0; theta <= 180; theta += (180 / stacks)){
		float sphere_coordinate[4][3] = {0.0f, 0.0f, 0.0f};
		for(phi = 0; phi < 360; phi += (360 / slices)){
			sphere_coordinate[0][0] = radius * sin(theta * pi) * cos(phi * pi);
			sphere_coordinate[0][2] = radius * sin(theta * pi) * sin(phi * pi);
			sphere_coordinate[0][1] = radius * cos(theta * pi);

			sphere_coordinate[1][0] = radius * sin((theta + (180 / stacks)) * pi) * cos(phi * pi);
			sphere_coordinate[1][2] = radius * sin((theta + (180 / stacks)) * pi) * sin(phi * pi);
			sphere_coordinate[1][1] = radius * cos((theta + (180 / stacks)) * pi);

			sphere_coordinate[2][0] = radius * sin((theta + (180 / stacks)) * pi) * cos((phi + (360 / slices)) * pi);
			sphere_coordinate[2][2] = radius * sin((theta + (180 / stacks)) * pi) * sin((phi + (360 / slices)) * pi);
			sphere_coordinate[2][1] = radius * cos((theta + (180 / stacks)) * pi);

			sphere_coordinate[3][0] = radius * sin(theta * pi) * cos((phi + (360 / slices)) * pi);
			sphere_coordinate[3][2] = radius * sin(theta * pi) * sin((phi + (360 / slices)) * pi);
			sphere_coordinate[3][1] = radius * cos(theta * pi);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			glVertex3f(sphere_coordinate[0][0], sphere_coordinate[0][1], sphere_coordinate[0][2]);
			glVertex3f(sphere_coordinate[1][0], sphere_coordinate[1][1], sphere_coordinate[1][2]);
			glVertex3f(sphere_coordinate[2][0], sphere_coordinate[2][1], sphere_coordinate[2][2]);
			glVertex3f(sphere_coordinate[3][0], sphere_coordinate[3][1], sphere_coordinate[3][2]);

			GLfloat v[2][3];
			if(theta + (180 / stacks) == 180){
				v[0][0] = sphere_coordinate[1][0] - sphere_coordinate[0][0];
				v[0][2] = sphere_coordinate[1][2] - sphere_coordinate[0][2];
				v[0][1] = sphere_coordinate[1][1] - sphere_coordinate[0][1];
				
				v[1][0] = sphere_coordinate[1][0] - sphere_coordinate[3][0];
				v[1][2] = sphere_coordinate[1][2] - sphere_coordinate[3][2];
				v[1][1] = sphere_coordinate[1][1] - sphere_coordinate[3][1];
			}else{
				v[0][0] = sphere_coordinate[0][0] - sphere_coordinate[1][0];
				v[0][2] = sphere_coordinate[0][2] - sphere_coordinate[1][2];
				v[0][1] = sphere_coordinate[0][1] - sphere_coordinate[1][1];
				
				v[1][0] = sphere_coordinate[2][0] - sphere_coordinate[1][0];
				v[1][2] = sphere_coordinate[2][2] - sphere_coordinate[1][2];
				v[1][1] = sphere_coordinate[2][1] - sphere_coordinate[1][1];
			}

			GLfloat normal_vector[3] = {v[0][1] * v[1][2] - v[0][2] * v[1][1], v[0][2] * v[1][0] - v[0][0] * v[1][2], v[0][0] * v[1][1] - v[0][1] * v[1][0]};
			float len = sqrt(normal_vector[0] * normal_vector[0] + normal_vector[1] * normal_vector[1] + normal_vector[2] * normal_vector[2]);
			normal_vector[0] = normal_vector[0] / len;
			normal_vector[1] = normal_vector[1] / len;
			normal_vector[2] = normal_vector[2] / len;
			//printf("theta: %f, phi: %f (%f, %f, %f)\n", theta, phi, normal_vector[0], normal_vector[1], normal_vector[2]);

			glNormal3fv(normal_vector);

			glEnd();
		}
	}
	
	
	if(show_axis == true){
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, 4 * radius, 0);
		glVertex3f(0, -4 * radius, 0);
		glEnd();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("WindowName");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(passiveMouseMotion);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void display()
{
	//ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
	//Viewport Matrix
	glViewport(0, 0, width, height);
	
	//
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	GLfloat Va[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat Vd[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat Vp[] = {0.0f, 10.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, Va);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Vd);
	glLightfv(GL_LIGHT0, GL_POSITION, Vp);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);


	//sun
	glPushMatrix();
	glColor3f(1.0f, 0.3f, 0.0f);
	//glRotatef(moon_rotation, 0.0f, 1.0f, 0.0f);
	drawSphere(6 * r, other_slices, other_stacks, false);
	glPopMatrix();
	//system("pause");

	//earth
	glPushMatrix();
	glRotatef(earth_revolution, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	glRotatef(earth_revolution, 0.0f, -1.0f, 0.0f);
	glRotatef(23.5, 0.0f, 0.0f, 1.0f);
    glRotatef(earth_rotation, 0.0f, 1.0f, 0.0f);
	glColor3f(0.0f, 0.5f, 1.0f);
	drawSphere(2 * r, earth_slices, earth_stacks, true);
	glPopMatrix();

	//moon
	glPushMatrix();
	glRotatef(earth_revolution, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	glRotatef(earth_revolution, 0.0f, -1.0f, 0.0f);

	glRotatef(moon_revolution, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0, 0.0, 0.0);
	glRotatef(moon_revolution, 0.0f, -1.0f, 0.0f);
	glRotatef(moon_rotation, 0.0f, 1.0f, 0.0f);
	glColor3f(0.7f, 0.7f, 0.7f);
	drawSphere(1 * r, other_slices, other_stacks, false);
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int _width, int _height) {
	width = _width;
	height = _height;
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
		pause = !pause;
    case 'o':
        if(earth_slices != 4)
            earth_slices = 4;
        else
            earth_slices = 360;

        if(earth_stacks != 2)
            earth_stacks = 2;
        else
            earth_stacks = 180;
	}
}

void mouse(int button, int state, int x, int y) {
}

void mouseMotion(int x, int y) {
}

void passiveMouseMotion(int x, int y) {
}

void idle() {
	// Sleep(50);	//about 50 fps
	if(!pause){
		earth_rotation = (earth_rotation + degree) % 360;
		earth_revolution_cnt = (earth_revolution_cnt + 1) % (365 * 360 / degree);
		earth_revolution = earth_revolution_cnt * ((float)degree / 365.0);

		moon_cnt = (moon_cnt + 1) % (28 * 360 / degree);
		moon_revolution = moon_cnt * ((float)degree / 28.0);
		moon_rotation = moon_cnt * ((float)degree / 28.0);

		glutPostRedisplay();
	}
}
