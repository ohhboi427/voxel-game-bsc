#ifndef __SHADING_GLSL__
#define __SHADING_GLSL__

#include "RayHitInfo.glsl"

layout(binding = 1) uniform sampler2D u_terrain;

// xyz -> rgb, w -> intensity
const vec4 SunLight = vec4(1.00, 0.95, 0.90, 1.00);
const vec4 SkyLight = vec4(0.60, 0.80, 1.00, 0.10);

const float NormalLightStrength[3] = {
	0.8,
	1.0,
	0.6,
};

float CalculateLightStrength(uint normal)
{
	return NormalLightStrength[normal];
}

vec3 Shade(RayHitInfo hitInfo)
{
	float lightStrength = CalculateLightStrength(hitInfo.Normal);

	vec3 light = (SunLight.xyz * SunLight.w + SkyLight.xyz * SkyLight.w) * lightStrength;
	vec3 textureColor = texture(u_terrain, hitInfo.UV).rgb;

	return textureColor * light;
}

#endif // __SHADING_GLSL__
