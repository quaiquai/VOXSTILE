#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include <vector>

class Chunk {

public:

	Chunk();
	~Chunk();

	void create_mesh();
	void create_cube(int x, int y, int z);

	static const int CHUNK_SIZE;
	static const int NUMBER_OF_CUBE_VERTS;
	static int CHUNK_COUNT;
	Block ***m_pBlocks;

	int chunk_id;
	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> normals;
	std::vector<float> colors;

};



#endif