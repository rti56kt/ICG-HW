#version 450
layout(binding = 0) uniform sampler2D Texture;
layout(binding = 1) uniform sampler2D Texture1;
layout(binding = 2) uniform sampler2D Texture2;
layout(binding = 3) uniform sampler2D ex_tex;
layout(binding = 4) uniform sampler2D co_tex;

uniform bool texture_switch;
uniform bool normal_switch;
uniform bool specular_switch;

in vec2 frag_texcoord;
in vec3 frag_nor;
in vec3 frag_pos;
in vec3 frag_ori_pos;
in vec3 tangent;
in vec3 bitangent;

out vec4 outColor;
out vec3 outNor;
uniform int Objstate;
uniform int Degree;

void main(){
	float pi = 3.14159265359f;
	vec4 resultColor;

	vec3 tex2 = texture2D(Texture1, frag_texcoord).rgb;
	vec3 normal = tex2;
	if(normal_switch){
		// normal.x = 1-normal.x;
		normal.y = 1-normal.y;
		normal = (normal * 2.0 - 1.0);
		normal = mat3(tangent, bitangent, frag_nor) * normal;
		normal = normalize(normal);
	}else
		normal = normalize(frag_nor);

	if(Objstate == 1) normal = normalize(frag_nor);

	float degree = (pi/180)*(Degree);
	degree = degree / 10;

	vec3 lightPos = vec3(0.0f, 0.0f, 10.0f);
	float lightx = lightPos.y;
	// lightPos.x = cos(-degree)*lightx - sin(-degree)*lightPos.z;
	// lightPos.z = sin(-degree)*lightx + cos(-degree)*lightPos.z;
	// lightx = lightPos.x;
	// lightPos.y = cos((23.5)*(pi/180))*lightx - sin((23.5)*(pi/180))*lightPos.z;
	// lightPos.z = sin((23.5)*(pi/180))*lightx + cos((23.5)*(pi/180))*lightPos.z;
	vec3 lightDir = normalize(lightPos - frag_ori_pos);

	vec3 viewPos =  vec3(0.0f, 0.0f, 3.0f);
	vec3 viewDir = normalize(viewPos - frag_ori_pos);
	vec3 reflectDir = normalize(reflect(-lightDir, normal));

	vec4 tex1 = texture2D(Texture, frag_texcoord);
	vec4 tex4 = texture2D(ex_tex, frag_texcoord);
	vec4 tex5 = texture2D(co_tex, frag_texcoord);
	vec4 tex3 = texture2D(Texture2, frag_texcoord);

	float ambientStrength = 0.8;
	vec4 ambient = ambientStrength * vec4(0.7, 0.7, 0.7, 1.0);

	vec4 diffuse;
	float diffuseStrength = 1.0;
	float diff = max(dot(normal, lightDir), 0.0);
	if(texture_switch) diffuse = diffuseStrength * diff * vec4(1.0, 1.0, 1.0, 1.0);
	else diffuse = diffuseStrength * diff * vec4(0.9, 0.1, 0.1, 1.0);

	float specularStrength = 0.3;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4);
	vec4 specular = specularStrength * spec * vec4(1.0, 1.0, 1.0, 1.0);
	if(Objstate == 0)
		if(specular_switch) specular = tex3 * specular;
	
	if(Objstate == 0) resultColor = tex1*(ambient + diffuse + specular);
	if(Objstate == 1) resultColor = tex5*(ambient + diffuse + specular);
	if(Objstate == 2) resultColor = tex4*(ambient + diffuse + specular);
	outColor = resultColor;
} 
