#version 330 core
out vec4 color;

in vec3 fragmentColor;
in vec3 v_normal;
in vec3 fragPos;

uniform vec3 lightPosition;
uniform vec3 lightColor;

void main(){
	//vec3 normalColor = fragmentColor * 0.5 + 0.5;
	vec3 ambient = vec3(0.1, 0.1, 0.1);
	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(lightPosition - fragPos);
	float diffCoef = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffCoef * lightColor;
	color = vec4((ambient + diffuse) * fragmentColor, 1.0);
}