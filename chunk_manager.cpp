
#include "chunk_manager.h"
#include "chunk.h"
#include <set>

int ChunkManager::CHUNK_SIZE = 16;
int ChunkManager::RENDER_DISTANCE = 5;

ChunkManager::ChunkManager(glm::vec3 position) {
	last_x_chunk = position.x / CHUNK_SIZE;
	last_z_chunk = position.z / CHUNK_SIZE;
	frame_counter = 0;
	update_interval = 5;
	// Start worker thread for background chunk generation
	stop_thread = false;
	worker_thread = std::thread(&ChunkManager::worker_loop, this);
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
					std::lock_guard<std::mutex> lock(chunk_mutex);
					pending_chunks.push({ x, z });				
				}
				chunk_cv.notify_one(); // Wake up worker thread
			}
		}
	}
}


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
}

void ChunkManager::worker_loop() {
	while (!stop_thread) {
		std::pair<int, int> chunk_coords;

		{
			std::unique_lock<std::mutex> lock(chunk_mutex);
			chunk_cv.wait(lock, [this] { return !pending_chunks.empty() || stop_thread; });

			if (stop_thread) break; // Exit if the manager is being destroyed

			chunk_coords = pending_chunks.front();
			pending_chunks.pop();
		}

		// Generate chunk outside the locked section
		Chunk new_chunk(chunk_coords.first, chunk_coords.second);

		{
			std::lock_guard<std::mutex> lock(chunk_mutex);
			chunks.push_back(std::move(new_chunk)); // Move the generated chunk to the active list
		}
	}
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
	
	glEnable(GL_DEPTH_TEST);  // Ensure depth testing is on
	glEnable(GL_CULL_FACE);   // Cull back faces for performance
	glCullFace(GL_BACK);

	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {

		if (!chunks[i].buffers_initialized) {
			std::cerr << "Warning: Chunk at (" << chunks[i].chunk_world_xposition
				<< ", " << chunks[i].chunk_world_zposition
				<< ") has uninitialized buffers!" << std::endl;
			continue;
		}

		if (chunks[i].indices.empty()) {
			std::cerr << "Warning: Chunk at (" << chunks[i].chunk_world_xposition
				<< ", " << chunks[i].chunk_world_zposition
				<< ") has no indices!" << std::endl;
			continue;
		}


		glBindVertexArray(chunks[i].VertexArrayID);
		glDrawElements(GL_TRIANGLES, chunks[i].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle
		
	}

	//glBindVertexArray(chunks[0].VertexArrayID);
	//glDrawElements(GL_TRIANGLES, chunks[0].indices.size(), GL_UNSIGNED_INT, 0); // Starting from vertex 0; 3 vertices total -> 1 triangle

}

ChunkManager::~ChunkManager() {
	stop_thread = true;
	chunk_cv.notify_one(); // Wake up thread to exit
	if (worker_thread.joinable()) {
		worker_thread.join();
	}
}