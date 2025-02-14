#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <vector>
#include <queue>
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
	std::vector<Chunk> chunks;
	std::vector<Chunk> unload_list;
	std::queue<std::pair<int, int>> pending_chunks;
	std::vector<int> load_list_index;
	ChunkManager(glm::vec3 position);
	ChunkManager() = default;
	void generate_visible_chunks(glm::vec3 position);
	void update_visible_chunks(glm::vec3 position);
	void generate_new_visible_chunks(glm::vec3 position);
	void generate_new_chunk(Chunk &chunk);
	void clear_unload_list();
	void remove_objects();
	void generate_chunks();
	void render_chunks();
};

#endif // !CHUNK_MANAGER_H
