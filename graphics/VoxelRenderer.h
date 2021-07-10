#pragma once

class Mesh;
class Chunk;

class VoxelRenderer
{
	float* buffer;
	size_t capacity;
public:
	VoxelRenderer(size_t _capacity);
	~VoxelRenderer();

	Mesh* render(Chunk* chunk, const Chunk** chunks, bool ao);
};

