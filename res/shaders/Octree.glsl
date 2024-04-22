#ifndef __OCTREE_GLSL__
#define __OCTREE_GLSL__

const uint CHUNK_SIZE = 32;

uint GetByte(uint index)
{
	uint indexInInt = (index % 4);

	return (VoxelData[(index / 4)] & (0xFF << indexInInt * 8)) >> (indexInInt * 8);
}

uint PopCountByte(uint byte, uint offset, uint count)
{
	uint mask = ~(~0 << count) << offset;
	byte &= mask;

	return bitCount(byte);
}

uint PopCountRange(uint begin, uint end)
{
	uint sum = 0;

	while(begin < end)
	{
		sum += bitCount(GetByte(begin));

		++begin;
	}

	return sum;
}

bool GetVoxel(ivec3 coordinate)
{
	uint headIndex = 0;
	uint parentIndex = 0;
	uint childIndexInParent = 8;

	uint nodeHalfSize = CHUNK_SIZE / 2;
	while(nodeHalfSize > 0)
	{
		uint childIndex =
			uint(coordinate.x >= nodeHalfSize) |
			(uint(coordinate.y >= nodeHalfSize) << 1) |
			(uint(coordinate.z >= nodeHalfSize) << 2);

		coordinate.x -= int(coordinate.x >= nodeHalfSize) * int(nodeHalfSize);
		coordinate.y -= int(coordinate.y >= nodeHalfSize) * int(nodeHalfSize);
		coordinate.z -= int(coordinate.z >= nodeHalfSize) * int(nodeHalfSize);

		nodeHalfSize /= 2;

		uint childMask = 1 << childIndex;

		if((GetByte(headIndex) & childMask) == 0)
		{
			return false;
		}

		uint skipCount =
			PopCountByte(GetByte(parentIndex), childIndexInParent + 1, 8) + // The number of children on the same layer after the current.
			PopCountRange(parentIndex + 1, headIndex) + // The number of nodes between the head and parent.
			PopCountByte(GetByte(headIndex), 0, childIndex) + // The number of children of head.
			1; // The head.

		childIndexInParent = childIndex;

		parentIndex = headIndex;
		headIndex += skipCount;
	}

	return true;
}

#endif // __OCTREE_GLSL__
