#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
out vec3 texDir;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightMap;

void main()
{
	float earthProportion = 11.0/6371.0;
	float scale = 20.0;
	vec3 position = (1.0f+texture(heightMap, aTexCoord).r*earthProportion*scale)*aPos;
    gl_Position = projection*view*model*vec4(position, 1.0);
	texCoord = aTexCoord;
	texDir = aPos;
}