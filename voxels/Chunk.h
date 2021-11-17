#pragma once

#define CHUNK_W 16
#define CHUNK_H 16
#define CHUNK_D 16
#define CHUNK_VOL (CHUNK_W * CHUNK_H * CHUNK_D)

class voxel;
class Lightmap;

class Chunk
{
public:
	int x, y, z;
	bool modified = true;
	voxel* voxels;
	Lightmap* lightmap;
	Chunk(int x, int y, int z);
	~Chunk();
};

