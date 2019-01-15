#version 450
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 tex;

out vec2 geom_texcoord;
out vec3 geom_nor;
out vec3 geom_pos;
out vec3 geom_ori_pos;

uniform int Objstate;
uniform mat4 Projection;
uniform mat4 ModelView;
uniform int Degree;

varying float noise;

float random (vec3 st) {
    return fract( ( sin( dot( st.xy, vec2(12.9898, 78.233) ) ) + 0.0314259 ) * 43758.5453123 );
}

float turbulence( vec3 p ) {
    float w = 100.0;
    float t = -.5;
    for (float f = 1.0 ; f <= 10.0 ; f++ ){
        float power = pow( 2.0, f );
        t += abs( random( vec3( power * p ) ) / power );
    }
    return t;
}

void main() {
	float pi = 3.14159265359f;
	vec3 new_pos = pos;
	vec3 new_nor = nor;
	float degree = (pi/180)*(Degree%360);


	if(Objstate == 0){
		if(Degree < 140){
			new_pos.x = cos(-degree)*pos.x - sin(-degree)*pos.z;
			new_pos.z = sin(-degree)*pos.x + cos(-degree)*pos.z;

			geom_ori_pos = new_pos;

			float tmpx = cos((-23.5)*(pi/180))*new_pos.x - sin((-23.5)*(pi/180))*new_pos.y;
			float tmpy = sin((-23.5)*(pi/180))*new_pos.x + cos((-23.5)*(pi/180))*new_pos.y;
			new_pos.x = tmpx;
			new_pos.y = tmpy;

			new_nor.x = cos(-degree)*nor.x - sin(-degree)*nor.z;
			new_nor.z = sin(-degree)*nor.x + cos(-degree)*nor.z;
		}
	}else if(Objstate == 1){
		float magnitude;
		if(Degree < 140) magnitude = 0.5*9.8*(Degree*0.01)*(Degree*0.01);
		else magnitude = 0.5*9.8*(140*0.01)*(140*0.01) + 0.5*0.1*(Degree*0.01)*(Degree*0.01);
		vec3 offset = vec3(-1.0, -1.0, 0.0)*magnitude;

		new_pos = pos + offset;
		geom_ori_pos = new_pos;
		new_nor = nor + offset;
	}else if(Objstate == 2){
		noise = 10.0 *  -.10 * turbulence( .5 * nor + .001 );
		float b = 5.0 * random( 0.05 * pos + vec3( 2.0 * .001 ));
		float displacement = -0.1 * noise + b * .003 * Degree;

		new_pos = pos + nor * displacement;
		geom_ori_pos = new_pos;
		new_nor = nor;
	}

	gl_Position = Projection * ModelView * vec4(new_pos, 1.0);
	geom_pos = new_pos;
	geom_nor = new_nor;
	geom_texcoord = tex;
}