#pragma once
#include "glad/glad.h"
#include "chunk_manager.h"

class Renderer {
public:


	Renderer() = default;
	void renderWireframes();
	void enableDepthTesting();
	void initChunkBuffers(ChunkManager &chunks);
};