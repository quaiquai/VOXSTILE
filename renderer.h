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
	void init_framebuffer(T &obj) {
		glGenFramebuffers(1, &obj.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, obj.fbo);

		glGenTextures(1, &obj.texture);
		glBindTexture(GL_TEXTURE_2D, obj.texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, obj.texture, 0);
		// Check for completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer is not complete!" << std::endl;
		}
		// Unbind the FBO
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};