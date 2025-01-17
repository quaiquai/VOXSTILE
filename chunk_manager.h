#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include "glad/glad.h"

#include "chunk.h"

class ChunkManager {

public:

	std::vector<Chunk*> chunks;
	ChunkManager();
	void generate_chunks();
	void render_chunks();
};

#endif // !CHUNK_MANAGER_H
