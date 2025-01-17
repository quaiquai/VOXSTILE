
#include "chunk_manager.h"
#include "chunk.h"

ChunkManager::ChunkManager() {
	for (int x = 0; x < 5; ++x) {
		for (int z = 0; z < 5; ++z) {
			ChunkManager::chunks.push_back(new Chunk(x, z));
		}
	}
}

void ChunkManager::generate_chunks() {
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		ChunkManager::chunks[i]->remove_heights();
		ChunkManager::chunks[i]->create_mesh();
	}
}

void ChunkManager::render_chunks() {
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		glDrawElements(GL_TRIANGLES, ChunkManager::chunks[i]->indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle
	}
}