
#version 450 core
out vec4 FragColor;
in vec3 texDir;
in vec2 texCoord;
uniform samplerCube textureCubeMap;
uniform sampler2D textureMap; 
uniform int useTexture; 

void main()
{
	if (useTexture == 0)
		FragColor = texture(textureMap, texCoord);
	else 
		FragColor = texture(textureCubeMap, texDir);
}