#ifndef BLOCK_H
#define BLOCK_H

#include <array>
#include "block_type.h"


class Block {

public: 

	std::array<float, 108> vertices;
	std::array<float, 108> colors;

	int block_id;

	bool is_active;
	BlockType m_blockType;
	static const float BLOCK_RENDER_SIZE;
	static int block_count;

	Block();
	Block(BlockType type);
	Block(float size);

	void create_verts(float size);


};



#endif // !BLOCK_H
