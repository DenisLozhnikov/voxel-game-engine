#include "Chunk.h"
#include "voxel.h"
#include "../light/Lightmap.h"

#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

Chunk::Chunk(int _xpos, int _ypos, int _zpos) : x(_xpos), y(_ypos), z(_zpos)
{
	voxels = new voxel[CHUNK_VOL];
	lightmap = new Lightmap();

	for (int z = 0; z < CHUNK_D; z++) {
		for (int x = 0; x < CHUNK_W; x++) {
			int real_x = x + this->x * CHUNK_W;
			int real_z = z + this->z * CHUNK_D;
			for (int y = 0; y < CHUNK_H; y++) {
				int real_y = y + this->y * CHUNK_H;
				int id = glm::perlin(glm::vec3(real_x * 0.0125f, real_y * 0.0125f, real_z * 0.0125f)) > 0.1f;
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
