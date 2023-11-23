#version 460 core

layout(location = 0) in vec2 UV;
layout(location = 0) out vec4 Color;

layout(binding = 0) uniform sampler2D u_renderedImage;

void main()
{
	const float exposure = 1.0;
	const float gamma = 2.2;

	vec3 unmappedColor = texture(u_renderedImage, UV).rgb;

	vec3 mappedColor = pow(vec3(1.0) - exp(-unmappedColor * exposure), vec3(1.0 / gamma));

	Color = vec4(mappedColor, 1.0);
}
