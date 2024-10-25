#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragmentColor;
out vec3 v_normal;

void main(){
	gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1.0);

	fragmentColor = normal;
	v_normal = normal;
}