#pragma once

#include "chunk.h"
#include "block_type.h"


namespace Generators {

	void generate_poolroom(Chunk &chunk);
	void carve_room(Chunk &chunk);
	void generate_stairs(Chunk &chunk, int startX, int startY, int startZ, int direction);
	void generate_bridge(Chunk &chunk, int startX, int startY, int startZ, int length);
	void generate_pool(Chunk &chunk, int startX, int startY, int startZ, int width, int depth);
	void generate_overhang(Chunk &chunk, int startX, int startY, int startZ, int width, int depth);
};