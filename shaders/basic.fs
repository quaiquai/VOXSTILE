#version 330 core
out vec3 color;

in vec3 fragmentColor;
in vec3 v_normal;

void main(){
	//vec3 normalColor = fragmentColor * 0.5 + 0.5;
	color = fragmentColor;
}