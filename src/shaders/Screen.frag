#version 460 core

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D u_renderedImage;

void main()
{
	Color = texture(u_renderedImage, UV);
	Color.a = 1.0;
}
