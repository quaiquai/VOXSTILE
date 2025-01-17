#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include <vector>

class Chunk {

public:

	Chunk() = default;
	Chunk(int worldx, int worldz);
	~Chunk();

	void create_mesh();
	void create_cube(int x, int y, int z);
	void remove_heights();

	static const int CHUNK_SIZE;
	static const int NUMBER_OF_CUBE_VERTS;
	static int CHUNK_COUNT;
	static int block_number;
	Block ***m_pBlocks;

	int chunk_id;
	int chunk_world_xposition;
	int chunk_world_zposition;
	float generate_height(int x, int z);
	std::vector<float> vertices;
	std::vector<int> indices;
	std::vector<float> normals;
	std::vector<float> colors;

};



#endif