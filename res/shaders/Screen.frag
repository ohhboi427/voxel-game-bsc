#version 460 core

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D u_renderedTexture;

void main()
{
	vec3 color = texture(u_renderedTexture, UV).rgb;

	Color = vec4(color, 1.0);
}
