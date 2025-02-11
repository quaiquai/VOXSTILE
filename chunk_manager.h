#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include "glad/glad.h"

#include "chunk.h"

class ChunkManager {

public:

	static int CHUNK_SIZE;
	static int RENDER_DISTANCE;
	std::vector<Chunk> chunks;
	ChunkManager();
	void generate_visible_chunks(glm::vec3 position);
	void generate_chunks();
	void render_chunks();
};

#endif // !CHUNK_MANAGER_H
