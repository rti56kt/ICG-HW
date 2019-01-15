#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <Windows.h>
#include "FreeImage.h"
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

using namespace std;

struct VertexAttribute
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

int width = 650, height = 650;
int r = 1;
int degree = 1;
bool texture_switch = true;
bool normal_switch = true;
bool specular_switch = true;
bool pause = false;

int objstate = 0;

int earth_slices = 360;
int earth_stacks = 180;
int earth_rotation = 0;
float pi = M_PI/180;

double cx = 0.0;
double cy = 0.0;

GLuint program;
GLuint vboName;
unsigned int earth_texture;
unsigned int normal_map;
unsigned int specular_map;
unsigned int explode_texture;
unsigned int comet_texture;

void init();
void display();
unsigned int texture(string filename);
VertexAttribute *drawSphere(GLfloat radius, GLint slices, GLint stacks);
void reshape(int _width, int _height);
void keyboard(unsigned char key, int x, int y);
void idle();

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("HW3_0516220");
	glewInit();
	init();
	earth_texture = texture("./earth_texture_map.jpg");
	normal_map = texture("./earth_normal_map.tif");
	specular_map = texture("./earth_specular_map.tif");
	explode_texture = texture("./2k_sun.jpg");
	comet_texture = texture("./2k_eris_fictional.jpg");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void init(){
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);
	
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = drawSphere(r, earth_slices, earth_stacks);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * earth_slices*(earth_stacks+1)*2*3, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

void display(){
	/* ModelView Matrix */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	if(earth_rotation < 300 && earth_rotation > 170){
		if (earth_rotation % 10 == 1) {
			cx = (rand() % 1000 - 500.0) / 1000.0;
			cy = (rand() % 1000 - 500.0) / 1000.0;
		}
		gluLookAt(cx, cy, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	}
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

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);

	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "Texture");
	GLint tex1Loc = glGetUniformLocation(program, "Texture1");
	GLint tex2Loc = glGetUniformLocation(program, "Texture2");
	GLint tex3Loc = glGetUniformLocation(program, "ex_tex");
	GLint tex4Loc = glGetUniformLocation(program, "co_tex");
	GLint degreeLoc = glGetUniformLocation(program, "Degree");
	GLint switch1Loc = glGetUniformLocation(program, "texture_switch");
	GLint switch2Loc = glGetUniformLocation(program, "normal_switch");
	GLint switch3Loc = glGetUniformLocation(program, "specular_switch");
	GLint objstateLoc = glGetUniformLocation(program, "Objstate");

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earth_texture);
	glUniform1i(texLoc, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal_map);
	glUniform1i(tex1Loc, 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, specular_map);
	glUniform1i(tex2Loc, 2);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, explode_texture);
	glUniform1i(tex3Loc, 3);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, comet_texture);
	glUniform1i(tex4Loc, 4);

	glUniform1i(degreeLoc, earth_rotation);
	glUniform1i(switch1Loc, texture_switch);
	glUniform1i(switch2Loc, normal_switch);
	glUniform1i(switch3Loc, specular_switch);

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	int spheresize = earth_slices*(earth_stacks+1)*2;

	objstate = 1;
	glUniform1i(objstateLoc, objstate);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, spheresize);

	objstate = 0;
	glUniform1i(objstateLoc, objstate);
	glDrawArrays(GL_TRIANGLE_STRIP, spheresize+1, spheresize*2);
	
	objstate = 2;
	glUniform1i(objstateLoc, objstate);
	glDrawArrays(GL_TRIANGLE_STRIP, spheresize*2+1, spheresize*3);

	glBindTexture(GL_TEXTURE_2D, NULL);

	glUseProgram(0);
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

VertexAttribute *drawSphere(GLfloat radius, GLint slices, GLint stacks){
	VertexAttribute *vertices;
	vertices = new VertexAttribute[slices*(stacks+1)*2*3];
	float phi, theta;
	int cnt=0;
	
	for(phi = 0; phi < 360; phi += (360 / slices)){
		float x = 0.0, y = 0.0, z = 0.0;
		for(theta = 0; theta <= 180; theta += (180 / stacks)){
			x = radius/2.0 * sin(theta * pi) * cos(phi * pi) + 10.0;
			z = radius/2.0 * sin(theta * pi) * sin(phi * pi);
			y = radius/2.0 * cos(theta * pi) + 10.0;

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;

			x = radius/2.0 * sin(theta * pi) * cos((phi + (360 / slices)) * pi) + 10.0;
			z = radius/2.0 * sin(theta * pi) * sin((phi + (360 / slices)) * pi);
			y = radius/2.0 * cos(theta * pi) + 10.0;

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360 - 1.0f / slices;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;
		}
	}

	for(phi = 0; phi < 360; phi += (360 / slices)){
		float x = 0.0, y = 0.0, z = 0.0;
		for(theta = 0; theta <= 180; theta += (180 / stacks)){
			x = radius * sin(theta * pi) * cos(phi * pi);
			z = radius * sin(theta * pi) * sin(phi * pi);
			y = radius * cos(theta * pi);

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;

			x = radius * sin(theta * pi) * cos((phi + (360 / slices)) * pi);
			z = radius * sin(theta * pi) * sin((phi + (360 / slices)) * pi);
			y = radius * cos(theta * pi);

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360 - 1.0f / slices;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;
		}
	}

	for(phi = 0; phi < 360; phi += (360 / slices)){
		float x = 0.0, y = 0.0, z = 0.0;
		for(theta = 0; theta <= 180; theta += (180 / stacks)){
			x = radius/3.0 * sin(theta * pi) * cos(phi * pi);
			z = radius/3.0 * sin(theta * pi) * sin(phi * pi);
			y = radius/3.0 * cos(theta * pi);

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;

			x = radius/3.0 * sin(theta * pi) * cos((phi + (360 / slices)) * pi);
			z = radius/3.0 * sin(theta * pi) * sin((phi + (360 / slices)) * pi);
			y = radius/3.0 * cos(theta * pi);

			vertices[cnt].position[0] = x;
			vertices[cnt].position[1] = y;
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - phi / 360 - 1.0f / slices;
			vertices[cnt].texcoord[1] = 1.0f - theta / 180;
			cnt++;
		}
	}

	
	return vertices;
}

void reshape(int _width, int _height){
	width = _width;
	height = _height;
}

void keyboard(unsigned char key, int x, int y){
	switch (key) {
	case '1':
		texture_switch = !texture_switch;
		break;
	case '2':
		normal_switch = !normal_switch;
		break;
	case '3':
		specular_switch = !specular_switch;
		break;
	case 'p':
		pause = !pause;
		break;
	}
}

void idle(){
	if(!pause){
		Sleep(10);
		earth_rotation = (earth_rotation + degree) % 3600;
	}
		glutPostRedisplay();
}
