

#include "generators.h"

void Generators::generate_poolroom(Chunk &chunk) {
	int maxWidth = (chunk.CHUNK_SIZE - 1) / 2;
	int maxDepth = (chunk.CHUNK_SIZE - 1) / 2;
	int maxHeight = (chunk.CHUNK_SIZE - 1) / 2;

	int width = 10 + rand() % (maxWidth);
	int depth = 10 + rand() % (maxDepth);
	int height = 10 + rand() % (maxHeight);

	int x = rand() % (chunk.CHUNK_SIZE - 1 - width);
	int z = rand() % (chunk.CHUNK_SIZE - 1 - depth);
	int y = rand() % (chunk.CHUNK_SIZE - 1 - height);
	BlockType type = STONE;
	chunk.room = { x, y, z, width, height, depth, type };
}

void Generators::carve_room(Chunk &chunk) {
	for (int x = 0; x < chunk.CHUNK_SIZE; ++x) {
		for (int y = 0; y < chunk.CHUNK_SIZE; ++y) {
			for (int z = 0; z < chunk.CHUNK_SIZE; ++z) {
				// Calculate the 1D index in the chunk array
				int index = x * chunk.CHUNK_SIZE * chunk.CHUNK_SIZE + y * chunk.CHUNK_SIZE + z;

				// Check if the block is inside the room's boundaries
				bool insideRoom = (x >= chunk.room.x && x < chunk.room.x + chunk.room.width &&
					y >= chunk.room.y && y < chunk.room.y + chunk.room.height &&
					z >= chunk.room.z && z < chunk.room.z + chunk.room.depth);

				// Check if the block is on the immediate border of the room
				bool onBorder = false;
				if ((x == chunk.room.x - 1 || x == chunk.room.x + chunk.room.width) &&
					y >= chunk.room.y && y < chunk.room.y + chunk.room.height &&
					z >= chunk.room.z && z < chunk.room.z + chunk.room.depth) {
					onBorder = true;
				}
				else if ((y == chunk.room.y - 1 || y == chunk.room.y + chunk.room.height) &&
					x >= chunk.room.x && x < chunk.room.x + chunk.room.width &&
					z >= chunk.room.z && z < chunk.room.z + chunk.room.depth) {
					onBorder = true;
				}
				else if ((z == chunk.room.z - 1 || z == chunk.room.z + chunk.room.depth) &&
					x >= chunk.room.x && x < chunk.room.x + chunk.room.width &&
					y >= chunk.room.y && y < chunk.room.y + chunk.room.height) {
					onBorder = true;
				}

				if (chunk.blocks[index] == GRASS) {
					continue;
				}

				if (insideRoom) {
					// Inside the room, set the block as inactive
					chunk.blocks[index] = INACTIVE;
				}
				else if (onBorder) {
					// Border blocks, set the block as stone
					if ((z == 12 || z == 13 || z == 14) && y > chunk.room.height) {
						chunk.blocks[index] = INACTIVE;
					}


				}
				else {
					// Outside the room and not bordering, set the block as inactive
					chunk.blocks[index] = INACTIVE;
				}
			}
		}
	}

}