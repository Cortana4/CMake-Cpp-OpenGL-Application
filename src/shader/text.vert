#version 420 core

layout (location = 0) in vec2 posAttrib;
layout (location = 1) in vec2 texCoordAttrib;

out vec2 texCoord;

uniform mat4 projection;

void main()
{
	gl_Position = projection * vec4(posAttrib, 0.0, 1.0);
	texCoord = texCoordAttrib;
}
