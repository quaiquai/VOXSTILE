#version 330 core
out vec4 color;

in vec3 fragmentColor;
in vec3 v_normal;
in vec3 fragPos;
in vec3 TexCoords;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentFragPos;
in vec3 T;
in vec3 B;
in vec3 N;
in vec3 tangentLightDirection;
in vec4 FragPosLightSpace;
in vec3 lightPos;


uniform vec3 lightColor;


uniform sampler2DArray texture3D;
uniform sampler2D shadowMap;


float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	vec3 normal = normalize(v_normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 0.2 / textureSize(shadowMap, 0);
	for (int x = -2; x <= 2; ++x)
	{
		for (int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 50.0;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main(){
	//vec3 normalColor = fragmentColor * 0.5 + 0.5;
	float specularStrength = 0.5;
	vec3 ambient = 0.2 * lightColor;
	//vec3 norm = normalize(v_normal);
	vec3 norm = texture(texture3D, vec3(TexCoords.rg, 1.0)).rgb;
	norm = normalize(norm*2.0 - 1.0);
	//norm = normalize(TBN * norm);
	//vec3 lightDir = normalize(tangentLightPos - tangentFragPos);//for point light
	vec3 lightDir = normalize(tangentLightDirection);//for point light
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	vec3 reflectDir = reflect(-lightDir, norm); //negate for incoming light
	float specularCoef = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * specularCoef * lightColor;
	float diffCoef = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffCoef * lightColor * 1.0;
	//color = vec4((ambient + diffuse + specular) * fragmentColor, 1.0);
	// calculate shadow
	float shadow = ShadowCalculation(FragPosLightSpace);
	
	diffuse *= (1.0 - shadow);
	specular *= (0.5 - shadow);

	color = vec4((ambient + diffuse + specular) * texture(texture3D, TexCoords).rgb, 1.0);


	

	//color = vec4((ambient + (1.0 - shadow) * (diffuse + specular)) * texture(texture3D, TexCoords).rgb, 1.0);
	//color = vec4(texture(shadowMap, TexCoords.xy).rgb, 1.0);
	//color = vec4(FragPosLightSpace.rgb, 1.0);
}