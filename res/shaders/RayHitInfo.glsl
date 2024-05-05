#ifndef __RAY_HIT_INFO__
#define __RAY_HIT_INFO__

struct RayHitInfo
{
	vec3 Point;
	float Distance;
	vec2 UV;
	uint Normal;
};

// Normal indices for each plane
const uvec3 NormalIndex = uvec3(0, 1, 2);

#endif // __RAY_HIT_INFO__
