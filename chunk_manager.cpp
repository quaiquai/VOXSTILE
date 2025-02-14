
#include "chunk_manager.h"
#include "chunk.h"
#include <set>

int ChunkManager::CHUNK_SIZE = 16;
int ChunkManager::RENDER_DISTANCE = 1;

ChunkManager::ChunkManager(glm::vec3 position) {
	last_x_chunk = position.x / CHUNK_SIZE;
	last_z_chunk = position.z / CHUNK_SIZE;
	frame_counter = 0;
	update_interval = 5;
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

void ChunkManager::generate_new_visible_chunks(glm::vec3 position) {
	int chunk_positionX = position.x / CHUNK_SIZE;
	int chunk_positionZ = position.z / CHUNK_SIZE;

	// Check if the player moved to a new chunk
	if (chunk_positionX != last_x_chunk || chunk_positionZ != last_z_chunk) {
		last_x_chunk = chunk_positionX;
		last_z_chunk = chunk_positionZ;

		// Create a set of existing chunk positions for fast lookup
		std::set<std::pair<int, int>> existing_chunks;
		for (const auto& chunk : chunks) {
			existing_chunks.insert({ chunk.chunk_world_xposition, chunk.chunk_world_zposition });
		}

		// Generate new chunks around the player
		int number_of_generated_chunks = 0;
		for (int x = chunk_positionX - RENDER_DISTANCE; x <= chunk_positionX + RENDER_DISTANCE; ++x) {
			for (int z = chunk_positionZ - RENDER_DISTANCE; z <= chunk_positionZ + RENDER_DISTANCE; ++z) {
				if (existing_chunks.find({ x, z }) == existing_chunks.end()) {
					pending_chunks.push({ x, z });
					/*
					chunks.emplace_back(Chunk(x, z));  // Add only missing chunks
					generate_new_chunk(chunks[chunks.size() - 1]);
					load_list_index.push_back(chunks.size() - 1);
					//renderer.init_chunk_buffers(chunks[chunks.size() - 1]);
					
					glBindVertexArray(chunks[chunks.size() - 1].VertexArrayID);
					glBindBuffer(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].vertex_buffer);

					glBufferData(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].vertices.size() * sizeof(float), &chunks[chunks.size() - 1].vertices[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(
						0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
						3,                 // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						0,                  // stride
						(void*)0            // array buffer offset
					);

					glBindBuffer(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].normalBuffer);
					glBufferData(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].normals.size() * sizeof(float), &chunks[chunks.size() - 1].normals[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(1);
					glBindBuffer(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].normalBuffer);
					glVertexAttribPointer(
						1,                                // attribute
						3,                                // size
						GL_FLOAT,                         // type
						GL_FALSE,                         // normalized?
						0,                                // stride
						(void*)0                          // array buffer offset
					);

					glBindBuffer(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].colorBuffer);
					glBufferData(GL_ARRAY_BUFFER, chunks[chunks.size() - 1].colors.size() * sizeof(float), &chunks[chunks.size() - 1].colors[0], GL_STATIC_DRAW);
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(
						2,                                // attribute
						3,                                // size
						GL_FLOAT,                         // type
						GL_FALSE,                         // normalized?
						0,                                // stride
						(void*)0                          // array buffer offset
					);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks[chunks.size() - 1].IndexBuffer);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunks[chunks.size() - 1].indices.size() * sizeof(int), &chunks[chunks.size() - 1].indices[0], GL_STATIC_DRAW);

					glBindVertexArray(0);

					++number_of_generated_chunks;
					if (number_of_generated_chunks >= 2) {
						return;
					}
					*/
				}
			}
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
	generate_new_visible_chunks(position);
	if (frame_counter % update_interval != 0) {
		return; // Skip this frame
	}

	int chunks_to_process = 2; // Limit chunks per frame
	while (!pending_chunks.empty() && chunks_to_process > 0) {
		std::pair<int, int> chunk_coords = pending_chunks.front();
		int x = chunk_coords.first;
		int z = chunk_coords.second;
		pending_chunks.pop();

		chunks.emplace_back(Chunk(x, z));
		generate_new_chunk(chunks.back());

		glBindVertexArray(chunks.back().VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, chunks.back().vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, chunks.back().vertices.size() * sizeof(float), chunks.back().vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, chunks.back().normalBuffer);
		glBufferData(GL_ARRAY_BUFFER, chunks.back().normals.size() * sizeof(float), chunks.back().normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, chunks.back().colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, chunks.back().colors.size() * sizeof(float), chunks.back().colors.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunks.back().IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunks.back().indices.size() * sizeof(int), chunks.back().indices.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);

		--chunks_to_process; // Process only a limited number per frame
	}

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

void ChunkManager::generate_new_chunk(Chunk &chunk) {
	chunk.remove_heights();
	//chunk.create_mesh();
}

void ChunkManager::clear_unload_list() {
	unload_list.clear();
}

void ChunkManager::generate_chunks() {
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		ChunkManager::chunks[i].remove_heights();
		//ChunkManager::chunks[i].create_mesh();
	}
}

void ChunkManager::render_chunks() {
	
	
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		glBindVertexArray(chunks[i].VertexArrayID);
		glDrawElements(GL_TRIANGLES, chunks[i].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle
	}
	
	
	//glBindVertexArray(chunks[chunks.size()-1].VertexArrayID);
	//glDrawElements(GL_TRIANGLES, chunks[9].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle

}