
#include "renderer.h"


void Renderer::renderWireframes() {
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
void Renderer::enableDepthTesting() {
	glEnable(GL_DEPTH_TEST);
}

void Renderer::init_chunk_buffers(Chunk &chunk) {
	glBindVertexArray(chunk.VertexArrayID);
	glBindBuffer(GL_ARRAY_BUFFER, chunk.vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER, chunk.vertices.size() * sizeof(float), &chunk.vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                 // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, chunk.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, chunk.normals.size() * sizeof(float), &chunk.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, chunk.normalBuffer);
	glVertexAttribPointer(
		1,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, chunk.colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, chunk.colors.size() * sizeof(float), &chunk.colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, chunk.texture_buffer);
	glBufferData(GL_ARRAY_BUFFER, chunk.tex_coords.size() * sizeof(float), &chunk.tex_coords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk.IndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk.indices.size() * sizeof(int), &chunk.indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}


void Renderer::initChunkBuffers(ChunkManager &chunks) {
	std::lock_guard<std::mutex> lock(chunks.chunk_mutex); // Lock for thread safety
	for (int i = 0; i < chunks.chunks.size(); ++i) {
		if (chunks.chunks[i].buffers_initialized == false) {
			
			glBindVertexArray(chunks.chunks[i].VertexArrayID);
			glBindBuffer(GL_ARRAY_BUFFER, chunks.chunks[i].vertex_buffer);

			glBufferData(GL_ARRAY_BUFFER, chunks.chunks[i].vertices.size() * sizeof(float), &chunks.chunks[i].vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                 // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glBindBuffer(GL_ARRAY_BUFFER, chunks.chunks[i].normalBuffer);
			glBufferData(GL_ARRAY_BUFFER, chunks.chunks[i].normals.size() * sizeof(float), &chunks.chunks[i].normals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, chunks.chunks[i].normalBuffer);
			glVertexAttribPointer(
				1,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			glBindBuffer(GL_ARRAY_BUFFER, chunks.chunks[i].colorBuffer);
			glBufferData(GL_ARRAY_BUFFER, chunks.chunks[i].colors.size() * sizeof(float), &chunks.chunks[i].colors[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			glBindBuffer(GL_ARRAY_BUFFER, chunks.chunks[i].texture_buffer);
			glBufferData(GL_ARRAY_BUFFER, chunks.chunks[i].tex_coords.size() * sizeof(float), &chunks.chunks[i].tex_coords[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(
				3,                                // attribute
				3,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks.chunks[i].IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunks.chunks[i].indices.size() * sizeof(int), &chunks.chunks[i].indices[0], GL_STATIC_DRAW);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			chunks.chunks[i].buffers_initialized = true;
		}
	}
}