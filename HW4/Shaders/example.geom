#version 450
layout(triangles) in;
layout(triangle_strip, max_vertices=6) out;

in vec2 geom_texcoord[3];
in vec3 geom_nor[3];
in vec3 geom_pos[3];
in vec3 geom_ori_pos[3];

out vec2 frag_texcoord;
out vec3 frag_nor;
out vec3 frag_pos;
out vec3 frag_ori_pos;
out vec3 tangent;
out vec3 bitangent;

uniform int Degree;
uniform int Objstate;

float random (vec2 st) {
    return fract( ( sin( dot( st.xy, vec2(12.9898, 78.233) ) ) + 0.0314259 ) * 43758.5453123 );
}

void main()
{
	vec2 deltaUV1 = geom_texcoord[1] - geom_texcoord[0];
	vec2 deltaUV2 = geom_texcoord[2] - geom_texcoord[0];
	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	vec3 deltaPos1 = geom_pos[1] - geom_pos[0];
	vec3 deltaPos2 = geom_pos[2] - geom_pos[0];

	tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*f;
	bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*f;
	// tangent = normalize(tangent);
	// bitangent = normalize(bitangent);

	if(Objstate == 0){
		if(Degree >= 140){
			vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
			vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
			vec3 normal = normalize(cross(a, b));
			float magnitude = 0.5*9.8*(Degree*0.005)*(Degree*0.005)*random(vec2(normal));
			vec4 offset = vec4(normal * magnitude, 0.0f);
			for(int i = 0; i < 3; i++){
				gl_Position = gl_in[i].gl_Position + offset;
				frag_texcoord = geom_texcoord[i];
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i];
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}
		}else{
			for(int i = 0; i < 3; i++){
				frag_texcoord = geom_texcoord[i];
				gl_Position = gl_in[i].gl_Position;
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i];
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}
		}
	}

	if(Objstate == 1){
		if(Degree >= 140){
			vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
			vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
			vec3 normal = normalize(cross(a, b));
			float magnitude = 0.5*9.8*(Degree*0.005)*(Degree*0.005)*random(vec2(normal));
			vec4 offset = vec4(normal * magnitude, 0.0f);
			vec4 offset2 = vec4(normal * magnitude, 0.0f);

			for(int i = 0; i < 3; i++){
				frag_texcoord = geom_texcoord[i];
				gl_Position = gl_in[i].gl_Position + vec4(3.0f, 3.0f, 0.0f, 1.0f) * random(geom_pos[0].xy) + offset2;
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i] + vec3(3.0f, 3.0f, 0.0f) * random(geom_pos[0].xy);
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}

			EndPrimitive();

			for(int i = 0; i < 3; i++){
				gl_Position = gl_in[i].gl_Position + offset;
				frag_texcoord = geom_texcoord[i];
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i];
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}
		}else{
			for(int i = 0; i < 3; i++){
				frag_texcoord = geom_texcoord[i];
				gl_Position = gl_in[i].gl_Position + vec4(3.0f, 3.0f, 0.0f, 1.0f) * random(geom_pos[0].xy);
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i] + vec3(3.0f, 3.0f, 0.0f) * random(geom_pos[0].xy);
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}

			EndPrimitive();

			for(int i = 0; i < 3; i++){
				frag_texcoord = geom_texcoord[i];
				gl_Position = gl_in[i].gl_Position;
				frag_nor = geom_nor[i];
				frag_pos = geom_pos[i];
				frag_ori_pos = geom_ori_pos[i];
				EmitVertex();
			}
		}
	}
	
	if(Objstate == 2){
		for(int i = 0; i < 3; i++){
			frag_texcoord = geom_texcoord[i];
			gl_Position = gl_in[i].gl_Position;
			frag_nor = geom_nor[i];
			frag_pos = geom_pos[i];
			frag_ori_pos = geom_ori_pos[i];
			EmitVertex();
		}
	}

	
	EndPrimitive();
}