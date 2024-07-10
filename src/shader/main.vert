#version 450 core

layout (location = 0) in vec3 posAttrib;
layout (location = 1) in vec3 normalAttrib;
layout (location = 2) in vec2 texCoordsAttrib;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragPos = vec3(model * vec4(posAttrib, 1.0f));
	normal = mat3(transpose(inverse(model))) * normalAttrib;
	texCoords = texCoordsAttrib;

	gl_Position = projection * view * model * vec4(posAttrib, 1.0f);
}
