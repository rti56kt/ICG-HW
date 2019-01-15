#version 450
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

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
	
	frag_texcoord = geom_texcoord[0];
	gl_Position = gl_in[0].gl_Position;
	frag_nor = geom_nor[0];
	frag_pos = geom_pos[0];
	frag_ori_pos = geom_ori_pos[0];
	EmitVertex();

	frag_texcoord = geom_texcoord[1];
	gl_Position = gl_in[1].gl_Position;
	frag_nor = geom_nor[1];
	frag_pos = geom_pos[1];
	frag_ori_pos = geom_ori_pos[1];
	EmitVertex();

	frag_texcoord = geom_texcoord[2];
	gl_Position = gl_in[2].gl_Position;
	frag_nor = geom_nor[2];
	frag_pos = geom_pos[2];
	frag_ori_pos = geom_ori_pos[2];
	EmitVertex();
	
	
	EndPrimitive();
}