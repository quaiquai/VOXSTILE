#version 330 core

in vec2 UV;
in vec2 screenspace;

out vec3 color;

uniform sampler2D renderedTexture;

void main() {
	vec2 uu = vec2(gl_FragCoord.xy / vec2(1920, 1080));
	color = texture(renderedTexture, uu).xyz;
}