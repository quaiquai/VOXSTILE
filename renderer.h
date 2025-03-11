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
	//void render_portal_view(const Portal &portal);
	template<typename T>
	void init_frameBuffer(T &obj);
};