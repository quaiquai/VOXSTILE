#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "glad/glad.h"
#include "chunk.h"

class ChunkManager {

public:

	static int CHUNK_SIZE;
	static int RENDER_DISTANCE;

	int last_x_chunk;
	int last_z_chunk;
	int frame_counter;
	int update_interval;

	// Threading
	std::thread worker_thread;
	std::mutex chunk_mutex;
	std::condition_variable chunk_cv;
	bool stop_thread = false;

	std::vector<Chunk> chunks;
	std::vector<Chunk> unload_list;
	std::queue<std::pair<int, int>> pending_chunks;
	std::vector<int> load_list_index;

	ChunkManager(glm::vec3 position);
	ChunkManager() = default;
	~ChunkManager();

	void generate_visible_chunks(glm::vec3 position);
	void update_visible_chunks(glm::vec3 position);
	void generate_new_visible_chunks(glm::vec3 position);
	void generate_new_chunk(Chunk &chunk);
	void clear_unload_list();
	void remove_objects();
	void generate_chunks();
	void render_chunks();
	void worker_loop(); // Background thread function
	
};

#endif // !CHUNK_MANAGER_H
