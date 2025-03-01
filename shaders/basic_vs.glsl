#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 tex_coords;
layout(location = 4) in vec3 tangents;
layout(location = 5) in vec3 bitangents;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform vec3 lightPosition;
uniform vec3 viewPos;

uniform vec3 lightDirection;

out vec3 fragmentColor;
out vec3 v_normal;
out vec3 fragPos;
out vec3 TexCoords;

out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;
out vec3 tangentLightDirection;
out vec4 FragPosLightSpace;
out vec3 lightPos;

out vec3 T;
out vec3 B;
out vec3 N;

void main(){
	
	fragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
	FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
	fragmentColor = color;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	v_normal = normal;
	TexCoords = tex_coords;
	T = normalize(vec3(model * vec4(tangents, 0.0)));
	B = normalize(vec3(model * vec4(bitangents, 0.0)));
	N = normalize(vec3(model * vec4(normal, 0.0)));
	mat3 TBN = transpose(mat3(T, B, N));
	tangentLightDirection = TBN * normalize(-lightDirection);
	tangentLightPos = TBN * lightPosition;
	tangentViewPos = TBN * viewPos;
	tangentFragPos = TBN * fragPos;

	lightPos = lightPosition;

	gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1.0);
}