#ifndef __SHADING_GLSL__
#define __SHADING_GLSL__

#include "RayHitInfo.glsl"

// xyz -> rgb, w -> intensity
const vec4 SunLight = vec4(1.00, 0.95, 0.90, 1.00);
const vec4 SkyLight = vec4(0.60, 0.80, 1.00, 0.10);

float CalculateLightStrength(uint normal)
{
	switch(normal)
	{
	case NormalYZ:
		return 0.8;
	case NormalXZ:
		return 1.0;
	case NormalXY:
		return 0.6;
	}

	return 0.0;
}

vec3 Shade(RayHitInfo hitInfo)
{
	float lightStrength = CalculateLightStrength(hitInfo.Normal);

	return (SunLight.xyz * SunLight.w + SkyLight.xyz * SkyLight.w) * lightStrength;
}

#endif // __SHADING_GLSL__
