#version 450
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 tex;

out vec2 geom_texcoord;
out vec3 geom_nor;
out vec3 geom_pos;
out vec3 geom_ori_pos;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform int Degree;

void main() {
	float pi = 3.14159265359f;
	vec3 new_pos = pos;
	vec3 new_nor = nor;
	float degree = (pi/180)*(Degree%360);

	new_pos.x = cos(-degree)*pos.x - sin(-degree)*pos.z;
	new_pos.z = sin(-degree)*pos.x + cos(-degree)*pos.z;

	geom_ori_pos = new_pos;

	float tmpx = cos((-23.5)*(pi/180))*new_pos.x - sin((-23.5)*(pi/180))*new_pos.y;
	float tmpy = sin((-23.5)*(pi/180))*new_pos.x + cos((-23.5)*(pi/180))*new_pos.y;
	new_pos.x = tmpx;
	new_pos.y = tmpy;

	new_nor.x = cos(-degree)*nor.x - sin(-degree)*nor.z;
	new_nor.z = sin(-degree)*nor.x + cos(-degree)*nor.z;

	// tmpx = cos((-23.5)*(pi/180))*new_nor.x - sin((-23.5)*(pi/180))*new_nor.y;
	// tmpy = sin((-23.5)*(pi/180))*new_nor.x + cos((-23.5)*(pi/180))*new_nor.y;
	// new_nor.x = tmpx;
	// new_nor.y = tmpy;

	gl_Position = Projection * ModelView * vec4(new_pos, 1.0);
	geom_pos = new_pos;
	geom_nor = new_nor;
	geom_texcoord = tex;
}