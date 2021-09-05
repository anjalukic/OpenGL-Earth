#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 texDir;
out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightMap;
uniform samplerCube heightCubeMap;
uniform int useTexture;

void main()
{
	fragPos = vec3(model * vec4(aPos, 1.0));
	texDir = aPos;
	texCoord = aTexCoord;
	normal = mat3(transpose(inverse(model)))*aPos;
	
	float earthProportion = 11.0/6371.0;
	float scale = 20.0;
	vec3 position;
	if (useTexture == 0)
		position = (1.0f+texture(heightMap, aTexCoord).r*earthProportion*scale)*aPos;
	else
		position = (1.0f+texture(heightCubeMap, aPos).r*earthProportion*scale)*aPos;
	
    gl_Position = projection*view*model*vec4(position, 1.0);
	
	
}