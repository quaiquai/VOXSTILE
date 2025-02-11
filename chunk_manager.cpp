
#include "chunk_manager.h"
#include "chunk.h"

int ChunkManager::CHUNK_SIZE = 16;
int ChunkManager::RENDER_DISTANCE = 5;

ChunkManager::ChunkManager() {
	/*
	for (int x = 0; x < 5; ++x) {
		for (int z = 0; z < 5; ++z) {
			ChunkManager::chunks.push_back(Chunk(x, z));
		}
	}
	*/
}

//from the players position, render all the chunks around based on the render distance
//this can be calculated by rendering over the range of position - renderdistance -> position + renderdistance in the x and z axis
void ChunkManager::generate_visible_chunks(glm::vec3 position){
	int chunk_positionX = position.x / CHUNK_SIZE;
	int chunk_positionZ = position.z / CHUNK_SIZE;
	for (int x = chunk_positionX - RENDER_DISTANCE; x < chunk_positionX + RENDER_DISTANCE; ++x) {
		for (int z = chunk_positionZ - RENDER_DISTANCE; z < +RENDER_DISTANCE; ++z) {
			ChunkManager::chunks.push_back(Chunk(x, z));
		}
	}
}

void ChunkManager::generate_chunks() {
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		ChunkManager::chunks[i].remove_heights();
		ChunkManager::chunks[i].create_mesh();
	}
}

void ChunkManager::render_chunks() {
	
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		glBindVertexArray(chunks[i].VertexArrayID);
		glDrawElements(GL_TRIANGLES, chunks[i].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle
	}
	
	//glBindVertexArray(chunks[9].VertexArrayID);
	//glDrawElements(GL_TRIANGLES, chunks[9].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle

}