#version 460 core

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D u_renderedImage;

void main()
{
	const float exposure = 1.0;
	const float gamma = 2.2;

	vec3 color = texture(u_renderedImage, UV).rgb;

	Color = vec4(color, 1.0);
}
