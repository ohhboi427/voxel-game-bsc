#ifndef __RAY_HIT_INFO__
#define __RAY_HIT_INFO__

struct RayHitInfo
{
	// xyz -> xyz, w -> distance
	vec4 Point;
	vec2 UV;
	uint Normal;
};

const uint NormalXY = 2;
const uint NormalXZ = 1;
const uint NormalYZ = 0;

#endif // __RAY_HIT_INFO__
