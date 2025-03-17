#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 screenspace;
out vec2 UV;

void main()
{
	UV = texCoords;
	vec4 clip_space = projection * view * model * vec4(aPos, 1.0);
	vec3 ndc = clip_space.xyz / clip_space.w;
	screenspace = (ndc.xy + 1.0) * 0.5;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}