#include "Chunk.h"
#include "voxel.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

Chunk::Chunk(int _xpos, int _ypos, int _zpos) : x(_xpos), y(_ypos), z(_zpos)
{
	
	voxels = new voxel[CHUNK_VOL];
	int id, real_x, real_y, real_z;

	for (int z = 0; z < CHUNK_D; z++) {
		for (int x = 0; x < CHUNK_W; x++) {
			 real_x = x + this->x * CHUNK_W;
			 real_z = z + this->z * CHUNK_D;
			for (int y = 0; y < CHUNK_H; y++) {
				 real_y = y + this->y * CHUNK_H;
				//int id = glm::perlin(glm::vec3(real_x * 0.0125f, real_y * 0.0125f, real_z * 0.0125f)) > 0.1f;
				//id = glm::perlin(glm::vec3(real_x * 0.055f, real_y * 0.055f, real_z * 0.055f)) > 0.1f;
				id = glm::simplex(glm::vec3(real_x * 0.054f, real_y * 0.064f, real_z * 0.044f)) > 0.1f;
				if (real_y <= 2)
					id = 2;
	
				voxels[(y * CHUNK_D + z) * CHUNK_W + x].id = id;
			}
		}
	}
}

Chunk::~Chunk()
{
	delete[] voxels;
}
