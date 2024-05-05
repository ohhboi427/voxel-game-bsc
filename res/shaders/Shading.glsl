#ifndef __SHADING_GLSL__
#define __SHADING_GLSL__

#include "RayHitInfo.glsl"

// xyz -> rgb, w -> intensity
const vec4 SunLight = vec4(1.00, 0.95, 0.90, 1.00);
const vec4 SkyLight = vec4(0.60, 0.80, 1.00, 0.10);

vec3 Shade(RayHitInfo hitInfo)
{
	float lightStrength;
	switch(hitInfo.Normal)
	{
	case 0:
		lightStrength = 0.8;
		break;
	case 1:
		lightStrength = 1.0;
		break;
	case 2:
		lightStrength = 0.6;
		break;
	}

	return (SunLight.xyz * SunLight.w + SkyLight.xyz * SkyLight.w) * lightStrength;
}

#endif // __SHADING_GLSL__
