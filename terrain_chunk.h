#ifndef TERRAIN_CHUNK
#define	TERRAIN_CHUNK

#include "block.h"
#include <vector>
#include <cmath>

class TerrainChunk {

public:

	TerrainChunk();
	~TerrainChunk();

	void create_mesh();
	void create_cube(float x, float y, float z);

	static const int CHUNK_SIZE;
	static const int NUMBER_OF_CUBE_VERTS;
	static int CHUNK_COUNT;
	Block **m_pBlocks;

	int chunk_id;
	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> normals;
	std::vector<float> colors;

};

#endif // !TERRAIN_CHUNK
