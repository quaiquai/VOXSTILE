
#include "chunk_manager.h"
#include "chunk.h"
#include <set>



int ChunkManager::CHUNK_SIZE = 32;				//LxWxH of chunk
int ChunkManager::RENDER_DISTANCE = 1;			//X-Z area of chunks to render around player position

ChunkManager::ChunkManager(glm::vec3 position) {
	//for logging
	total_verts = 0;
	//used for determining if moved of chunk boundaries
	last_x_chunk = 1000;
	last_z_chunk = 1000;
	//can use for limiting updates dependent on frames
	frame_counter = 0;
	update_interval = 5;
	// Start worker thread for background chunk generation
	stop_thread = false;
	worker_thread = std::thread(&ChunkManager::worker_loop, this);
}

//Loops through all the chunks and generate VAO and VBOs needed if haven't been done
void ChunkManager::generate_chunk_buffers() {
	std::lock_guard<std::mutex> lock(chunk_mutex);
	for (int i = 0; i < chunks.size(); i++) {
		if (!chunks[i].buffers_generated) {
			
			chunks[i].generate_buffers();
		}
	}
}

//from the players position, render all the chunks around based on the render distance
//this can be calculated by rendering over the range of position - renderdistance -> position + renderdistance in the x and z axis
void ChunkManager::spawn_initial_chunks(glm::vec3 position){
	
	int chunk_positionX = position.x / CHUNK_SIZE;
	int chunk_positionZ = position.z / CHUNK_SIZE;

	for (int x = chunk_positionX - RENDER_DISTANCE; x < chunk_positionX + RENDER_DISTANCE; ++x) {
		for (int z = chunk_positionZ - RENDER_DISTANCE; z < chunk_positionZ +RENDER_DISTANCE; ++z) {
			ChunkManager::chunks.push_back(std::move(Chunk(x, z)));
		}
	}
}

void ChunkManager::fill_chunks() {
	for (Chunk &c : chunks) {
		if (!c.blocks_generated) {
			//c.generate_blocks();
		}	
	}
}

void ChunkManager::generate_new_visible_chunks(glm::vec3 position) {
	int chunk_positionX = position.x / CHUNK_SIZE;
	int chunk_positionZ = position.z / CHUNK_SIZE;

	// Create a set of existing chunk positions for fast lookup
	// Use unordered_set for faster lookups
	std::unordered_set<std::pair<int, int>, PairHash> existing_chunks;

	for (size_t i = chunks.size(); i-- > 0;) {  // Reverse iteration to avoid invalidating indices
		int distance_from_cameraX = glm::abs(chunks[i].chunk_world_xposition - last_x_chunk);
		int distance_from_cameraZ = glm::abs(chunks[i].chunk_world_zposition - last_z_chunk);

		if (distance_from_cameraX > RENDER_DISTANCE || distance_from_cameraZ > RENDER_DISTANCE) {

			{
				//std::lock_guard<std::mutex> lock(chunk_mutex);
				unload_list.push(i);
			}					   // Erase the chunk from the main chunks list

		}
		else {
			existing_chunks.insert({ chunks[i].chunk_world_xposition, chunks[i].chunk_world_zposition });
		}
	}

	// Generate new chunks around the player
	int number_of_generated_chunks = 0;
	for (int x = chunk_positionX - RENDER_DISTANCE; x <= chunk_positionX + RENDER_DISTANCE; ++x) {
		for (int z = chunk_positionZ - RENDER_DISTANCE; z <= chunk_positionZ + RENDER_DISTANCE; ++z) {
			if (existing_chunks.find({ x, z }) == existing_chunks.end() && chunks_to_load_list.find({x,z}) == chunks_to_load_list.end()) {
				//std::lock_guard<std::mutex> lock(chunk_mutex);

				chunks_to_load.push({ x, z });
				chunks_to_load_list.insert({ x, z });
			}

		}
	}
	
	last_x_chunk = chunk_positionX;
	last_z_chunk = chunk_positionZ;

	
	 // Wake up worker thread
	
}

void ChunkManager::add_pending_chunks() {

	int num_to_process = 2;
	//move all chunks that are ready to the main chunk list
	
	{
		while (num_to_process > 0 && !chunks_to_load.empty()) {
			
			if (pending_chunks.size() >= 2) {
				chunk_cv.notify_one();
				return;
			}
			//std::lock_guard<std::mutex> lock(chunk_mutex);
			pending_chunks.push(chunks_to_load.front());
			chunks_to_load.pop();
			num_to_process--;
		}
		//chunks_to_load_list.clear();
		//chunk_cv.notify_one();
	}
}

void ChunkManager::remove_unload_chunks() {
	int num_to_process = 2;
	//remove all the chunks that were detected out of render distance
	{
		while (num_to_process > 0) {
			std::lock_guard<std::mutex> lock(chunk_mutex);
			if (!unload_list.empty()) {
				chunks.erase(chunks.begin() + unload_list.front());
				unload_list.pop();
			}
			num_to_process--;
		}
		
	}
}

void ChunkManager::update_visible_chunks(glm::vec3 position) {

	generate_new_visible_chunks(position);
	
}



void ChunkManager::worker_loop() {
	while (!stop_thread) {
		std::pair<int, int> chunk_coords;

		// Generate chunk outside the locked section
		

		{
			std::unique_lock<std::mutex> lock(chunk_mutex);
			chunk_cv.wait(lock, [this] { return !pending_chunks.empty() || stop_thread; });

			if (stop_thread) break; // Exit if the manager is being destroyed

			
		}
		chunk_coords = pending_chunks.front();
		pending_chunks.pop();
		
		
		Chunk new_chunk(chunk_coords.first, chunk_coords.second);
		
		Generators::generate_poolroom(new_chunk);
		Generators::carve_room(new_chunk);
		new_chunk.generate_mesh();

		rooms.push_back(new_chunk.room);
		
		{
			std::lock_guard<std::mutex> lock(chunk_mutex);
			total_verts += new_chunk.vertices.size();
			//if chunk list isnt empty set the last chunk's next room to the newly created chunks room
			if (!chunks.empty()) {
				chunks.back().prev_room = rooms.back(); //this needs to be changed to the variable next_room
			}
			chunks.emplace_back(std::move(new_chunk));
			
		}
	}
}

void ChunkManager::generate_new_chunk(Chunk &chunk) {
	//chunk.remove_heights();
	//chunk.create_mesh();
}

void ChunkManager::clear_unload_list() {
	//unload_list.clear();
}

void ChunkManager::generate_chunks() {
	for (int i = 0; i < ChunkManager::chunks.size(); ++i) {
		//ChunkManager::chunks[i].remove_heights();
		//ChunkManager::chunks[i].create_mesh();
	}
}

void ChunkManager::render_chunks() {
	
	glEnable(GL_DEPTH_TEST);  // Ensure depth testing is on
	//glEnable(GL_CULL_FACE);   // Cull back faces for performance
	//glCullFace(GL_BACK);
	std::lock_guard<std::mutex> lock(chunk_mutex);
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

		if (chunks[i].normals.empty()) {
			std::cerr << "Warning: Chunk at (" << chunks[i].chunk_world_xposition
				<< ", " << chunks[i].chunk_world_zposition
				<< ") has no normals!" << std::endl;
			continue;
		}

		if (chunks[i].vertices.empty()) {
			std::cerr << "Warning: Chunk at (" << chunks[i].chunk_world_xposition
				<< ", " << chunks[i].chunk_world_zposition
				<< ") has no vertices!" << std::endl;
			continue;
		}

		if (chunks[i].tangents.empty()) {
			std::cerr << "Warning: Chunk at (" << chunks[i].chunk_world_xposition
				<< ", " << chunks[i].chunk_world_zposition
				<< ") has no tangents!" << std::endl;
			continue;
		}
		
		if (chunks[i].buffers_generated && chunks[i].buffers_initialized && !chunks[i].indices.empty()) {
			
			glBindVertexArray(chunks[i].VertexArrayID);
			glDrawElements(GL_TRIANGLES, chunks[i].indices.size(), GL_UNSIGNED_INT, 0);
		}
		 // Starting from vertex 0; 3 vertices total -> 1 triangle
		
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