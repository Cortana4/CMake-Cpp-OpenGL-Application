#version 420 core

layout (location = 0) in vec3 posAttrib;
//layout (location = 1) in vec3 normalAttrib;
//layout (location = 2) in vec2 texCoordAttrib;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(posAttrib, 1.0f);
}
