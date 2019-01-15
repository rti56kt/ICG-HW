#include "glew.h"
#include "glut.h"
#include <Windows.h>
#include "FreeImage.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

using namespace std;

int width = 800, height = 450;
int r = 1;
int degree = 1;
bool pause = false;

int earth_slices = 360;
int earth_stacks = 180;
int moon_slices = 240;
int moon_stacks = 60;
int earth_rotation = 0;
int earth_revolution_cnt = 0;

int moon_cnt = 0;
float moon_rotation = 0.0;
float moon_revolution = 0.0;
float pi = M_PI/180;

unsigned int earth_texture;
unsigned int moon_texture;

void init();
void display();
void lighting();
unsigned int texture(string filename);
void drawSphere(GLfloat radius, GLint slices, GLint stacks, bool show_axis);
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("HW2_0516220");

	init();
	earth_texture = texture("./earth.jpg");
	moon_texture = texture("./moon.jpg");
	lighting();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void init(){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);

	//enable lighting
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	
	//enable GL_TEXTURE_2D
	glEnable(GL_TEXTURE_2D);
}

void lighting(){
	GLfloat Va[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat Vd[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat Vp[] = {0.0f, 10.0f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, Va);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Vd);
	glLightfv(GL_LIGHT0, GL_POSITION, Vp);
	glEnable(GL_LIGHT0);
}

void display(){
	/* ModelView Matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	/* Projection Matrix */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, width / (GLfloat)height, 0.1, 1000);
	/* Viewport Matrix */
	glViewport(0, 0, width, height);
	
	/* Clear */
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	/* Earth */
	glPushMatrix();
	glRotatef(23.5, 0.0f, 0.0f, 1.0f);
    glRotatef(earth_rotation, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, earth_texture);
	drawSphere(r, earth_slices, earth_stacks, true);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	/* Moon */
	glPushMatrix();
	glRotatef(moon_revolution, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0 * r, 0.0, 0.0);
	glRotatef(moon_revolution, 0.0f, -1.0f, 0.0f);
	glRotatef(moon_rotation, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, moon_texture);
	drawSphere(0.5 * r, moon_slices, moon_stacks, false);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glutSwapBuffers();
}

unsigned int texture(string filename){
	unsigned int texture_id;
	FreeImage_Initialise(0);
	FIBITMAP *pImage = FreeImage_Load(FreeImage_GetFileType(filename.c_str(), 0), filename.c_str());
	FIBITMAP *p32bImage = FreeImage_ConvertTo32Bits(pImage);
	int width = FreeImage_GetWidth(p32bImage);
	int height = FreeImage_GetHeight(p32bImage);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(pImage);
	FreeImage_Unload(p32bImage);
	FreeImage_DeInitialise();

	return texture_id;
}

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

			glTexCoord2f(1.0f - phi / 360, 1.0f - theta / 180);
			glVertex3f(sphere_coordinate[0][0], sphere_coordinate[0][1], sphere_coordinate[0][2]);

			glTexCoord2f(1.0f - phi / 360, 1.0f - theta / 180 - 1.0f / stacks);
			glVertex3f(sphere_coordinate[1][0], sphere_coordinate[1][1], sphere_coordinate[1][2]);

			glTexCoord2f(1.0f - phi / 360 - 1.0f / slices, 1.0f - theta / 180 - 1.0f / stacks);
			glVertex3f(sphere_coordinate[2][0], sphere_coordinate[2][1], sphere_coordinate[2][2]);

			glTexCoord2f(1.0f - phi / 360 - 1.0f / slices, 1.0f - theta / 180);
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
		glVertex3f(0, 2 * radius, 0);
		glVertex3f(0, -2 * radius, 0);
		glEnd();
	}
}

void reshape(int _width, int _height){
	width = _width;
	height = _height;
}

void keyboard(unsigned char key, int x, int y){
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

void idle(){
	if(!pause){
		earth_rotation = (earth_rotation + degree) % 360;

		moon_cnt = (moon_cnt + 1) % (28 * 360 / degree);
		moon_revolution = moon_cnt * ((float)degree / 28.0);
		moon_rotation = moon_cnt * ((float)degree / 28.0);

		glutPostRedisplay();
	}
}
