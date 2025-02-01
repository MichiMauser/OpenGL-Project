#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;
in vec4 lightPosWorld;


out vec4 fColor;


//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

uniform vec3 l_pos[2];
uniform vec3 l_color[2];


//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform sampler2D shadowMap;

uniform bool activ_tr = false;


vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

vec3 ambient1;
vec3 diffuse1;
vec3 specular1;

float cons = 0.2f;
float linear = 0.3f;
float quadric = 0.1f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	

	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	
	diffuse =  max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular =  specularStrength * specCoeff * lightColor;	
}





void computePointLight(){

	for(int i = 0; i < 2; i++){
		float dist;
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//
	dist = length(l_pos[i] - lightPosWorld.xyz);
	float attr = 1.0f / (cons + linear * dist + quadric * (dist * dist));

	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(l_pos[i]);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient += attr *  ambientStrength * l_color[i];
	
	//compute diffuse light
	
	diffuse +=  attr * max(dot(normalEye, lightDirN), 0.0f) * l_color[i];
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular +=  attr * specularStrength * specCoeff * l_color[i];
	
	
	}


}



float computeShadow(){
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords  = normalizedCoords * 0.5 + 0.5;

	if(normalizedCoords.z > 1.0f){
		return 0.0f;
	}

	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;	

	float currentDepth = normalizedCoords.z;

	//float bias = max(0.05f * (1.0 - dot(fNormal, lightDir)), 0.005f);
	float bias = 0.001f;
	float shadow = currentDepth - bias	> closestDepth ? 1.0 : 0.0;

	
	return shadow;
}

float computeFog(){
	float fogDensity = 0.02f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeLightComponents();
	computePointLight();
	float shadow = computeShadow();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	


	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f) ;
    
	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	color = color * 1.0f;
    //fColor = vec4(color, 1.0f);

	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	
	if(colorFromTexture.a < 0.1){
		discard;
	}
	
	float a = 0.2;

	vec4 finalColor = vec4(color, a) * colorFromTexture;

	fColor = fogColor * (1 - fogFactor) + finalColor * fogFactor;
}
