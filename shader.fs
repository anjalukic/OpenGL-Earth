
#version 450 core
out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
    float linear;
    float quadratic;
};

in vec3 texDir;
in vec2 texCoord; 
in vec3 normal;
in vec3 fragPos;

uniform samplerCube textureCubeMap;
uniform samplerCube specularCubeMap; 
uniform sampler2D textureMap; 
uniform sampler2D specularMap; 
uniform int useTexture; 
uniform Light light;
uniform vec3 viewPos;

void main()
{
	vec3 diffuseColor;
	vec3 specularColor;
	
	//use cubeMap or 2D texture
	if (useTexture == 0){
		diffuseColor = vec3(texture(textureMap, texCoord).rgb);
		specularColor = vec3(texture(specularMap, texCoord).rgb);
	}
	else {
		diffuseColor = vec3(texture(textureCubeMap, texDir).rgb);
		specularColor = vec3(texture(specularCubeMap, texDir).rgb);
	}
		
	// ambient
    vec3 ambient = light.ambient * diffuseColor.rgb;
	
	vec3 lightDir = normalize(light.position - fragPos);
	
    // diffuse 
    float diff = max(dot(normalize(normal), lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * diffuseColor.rgb;  
	
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normalize(normal));  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * spec * specularColor.rgb;  
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation;
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
	
	
}