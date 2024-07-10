#version 450 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D texSampler;
uniform vec3 texColor;

void main()
{
	fragColor = vec4(texColor, 1.0) * vec4(1.0, 1.0, 1.0, texture(texSampler, texCoord).r);
}
