#pragma once
#include "Chunk.h"
#include "voxel.h"
#include <glm/glm.hpp>

class Chunks
{
public:
	float t;
	Chunk** chunks;
	size_t vol;
	unsigned int w, h, d;
	bool modified = true;
	Chunks(int w, int h, int d);
	~Chunks();

	voxel* get(int x, int y, int z);
	Chunk* getChunk(int x, int y, int z);
	void set(int x, int y, int z, int id);
	voxel* rayCast(glm::vec3 a, glm::vec3 dir, float maxDist, glm::vec3& end, glm::vec3& norm, glm::vec3& iend);

	void saveWorld(unsigned char* buff);
	void loadWorld(unsigned char* buff);
};

