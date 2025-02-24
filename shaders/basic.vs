#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentColor;
out vec3 v_normal;
out vec3 fragPos;
out vec3 TexCoords;

void main(){
	gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1.0);
	fragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
	fragmentColor = color;
	v_normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = tex_coords;
}