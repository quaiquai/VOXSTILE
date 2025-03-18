#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "glad/glad.h"
#include "chunk.h"
#include "generators.h"
#include <algorithm>
#include <unordered_set>

// Hash function for unordered_set<pair<int, int>>
struct PairHash {
	std::size_t operator()(const std::pair<int, int>& p) const {
		return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
	}
};

class ChunkManager {

public:

	static int CHUNK_SIZE;
	static int RENDER_DISTANCE;

	int last_x_chunk;
	int last_z_chunk;
	int frame_counter;
	int update_interval;
	int total_verts;

	static constexpr size_t MAX_QUEUE_SIZE = 100;

	// Threading
	std::thread worker_thread;
	std::mutex chunk_mutex;
	std::condition_variable chunk_cv;
	bool stop_thread = false;

	std::vector<Chunk> chunks;
	std::vector<Room> rooms;
	std::queue<int> unload_list;
	std::queue<Chunk> pending_ready_chunks;
	std::queue<std::pair<int, int>> chunks_to_load;
	std::unordered_set<std::pair<int, int>, PairHash> chunks_to_load_list;
	std::queue<std::pair<int, int>> pending_chunks;
	std::vector<int> load_list_index;

	ChunkManager(glm::vec3 position);
	ChunkManager() = default;
	~ChunkManager();

	void spawn_initial_chunks(glm::vec3 position);
	void update_visible_chunks(glm::vec3 position);
	void generate_new_visible_chunks(glm::vec3 position);
	void generate_new_chunk(Chunk &chunk);
	void generate_chunk_buffers();
	void clear_unload_list();

	void add_pending_chunks();
	void remove_unload_chunks();
	void fill_chunks();
	void generate_chunks();
	void render_chunks();

	//void configure_chunk_portals();

	void worker_loop(); // Background thread function

	
};

#endif // !CHUNK_MANAGER_H
