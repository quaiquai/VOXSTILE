
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
	chunks.reserve(10);
}

//from the players position, render all the chunks around based on the render distance
//this can be calculated by rendering over the range of position - renderdistance -> position + renderdistance in the x and z axis
void ChunkManager::generate_visible_chunks(glm::vec3 position){
	
	int chunk_positionX = position.x / CHUNK_SIZE;
	int chunk_positionZ = position.z / CHUNK_SIZE;
	for (int x = chunk_positionX - RENDER_DISTANCE; x < chunk_positionX + RENDER_DISTANCE; ++x) {
		for (int z = chunk_positionZ - RENDER_DISTANCE; z < chunk_positionZ +RENDER_DISTANCE; ++z) {
			ChunkManager::chunks.push_back(Chunk(x, z));
		}
	}
}

/*
void ChunkManager::remove_objects() {
	for (int i = 0; i < unload_list.size(); i++) {
		chunks.erase(std::remove_if(chunks.begin(), chunks.end(),
			[unload_list[i]](const std::unique_ptr<Chunk>& obj) { return obj->id == unload_list[i]; }),
			chunks.end());
	}
	
}
*/

void ChunkManager::update_visible_chunks(glm::vec3 position) {


	for (size_t i = chunks.size(); i-- > 0;) {  // Reverse iteration to avoid invalidating indices
		int distance_from_cameraX = glm::abs(chunks[i].chunk_world_xposition - (position.x / CHUNK_SIZE));
		int distance_from_cameraZ = glm::abs(chunks[i].chunk_world_zposition - (position.z / CHUNK_SIZE));

		if (distance_from_cameraX > RENDER_DISTANCE || distance_from_cameraZ > RENDER_DISTANCE) {
			unload_list.push_back(chunks[i]);  // Move chunk to unload list

											   // Erase the chunk from the main chunks list
			chunks.erase(chunks.begin() + i);
		}
	}

	/*
	chunks.erase(std::remove_if(chunks.begin(), chunks.end(),
		[this, position](const Chunk& chunk) {
		int distance_from_cameraX = glm::abs(chunk.chunk_world_xposition - (position.x / CHUNK_SIZE));
		int distance_from_cameraZ = glm::abs(chunk.chunk_world_zposition - (position.z / CHUNK_SIZE));

		if (distance_from_cameraX > RENDER_DISTANCE || distance_from_cameraZ > RENDER_DISTANCE) {
			unload_list.push_back(chunk);
			return true;  // Mark for removal
		}
		return false;
	}),
		chunks.end());
	*/
	/*
	for (size_t i = chunks.size(); i-- > 0; ) {
		int distance_from_cameraX = glm::abs(chunks[i].chunk_world_xposition - (position.x / CHUNK_SIZE));
		int distance_from_cameraZ = glm::abs(chunks[i].chunk_world_zposition - (position.z / CHUNK_SIZE));
		if (distance_from_cameraX > RENDER_DISTANCE || distance_from_cameraZ > RENDER_DISTANCE) {
			unload_list.push_back(chunks[i]);
			std::cout << distance_from_cameraX << std::endl;
			std::cout << distance_from_cameraZ << std::endl;
			return;
		}
	}
	*/
	
}

void ChunkManager::clear_unload_list() {
	unload_list.clear();
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