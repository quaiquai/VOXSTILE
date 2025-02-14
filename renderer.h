#pragma once
#include "glad/glad.h"
#include "chunk_manager.h"
//#include "chunk.h"

class Renderer {
public:


	Renderer() = default;
	void renderWireframes();
	void enableDepthTesting();
	void initChunkBuffers(ChunkManager &chunks);
	void init_chunk_buffers(Chunk &chunk);
};