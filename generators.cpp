

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

	//chunk.absolute_positionX, chunk.absolute_positionZ are used for calculating portal position in room
	chunk.room = { x, y, z, width, height, depth, chunk.absolute_positionX, chunk.absolute_positionZ, type};
}

void Generators::generate_stairs(Chunk &chunk, int startX, int startY, int startZ, int direction) {
	int stairHeight = 4; // Height of the staircase
	int stairWidth = 2;  // Width of the staircase

	for (int y = 0; y < stairHeight; ++y) {
		for (int x = 0; x < stairWidth; ++x) {
			int blockX = startX + x;
			int blockY = startY + y;
			int blockZ = startZ + y * direction;

			int index = blockX * chunk.CHUNK_SIZE * chunk.CHUNK_SIZE + blockY * chunk.CHUNK_SIZE + blockZ;
			chunk.blocks[index] = STONE; // Set the block to stone
		}
	}
}

void Generators::generate_bridge(Chunk &chunk, int startX, int startY, int startZ, int length) {
	int bridgeWidth = 3; // Width of the bridge

	for (int z = 0; z < length; ++z) {
		for (int x = 0; x < bridgeWidth; ++x) {
			int blockX = startX + x;
			int blockY = startY;
			int blockZ = startZ + z;

			int index = blockX * chunk.CHUNK_SIZE * chunk.CHUNK_SIZE + blockY * chunk.CHUNK_SIZE + blockZ;
			chunk.blocks[index] = STONE; // Set the block to stone
		}
	}
}

void Generators::generate_pool(Chunk &chunk, int startX, int startY, int startZ, int width, int depth) {
	int poolHeight = 1; // Height of the pool

	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < depth; ++z) {
			int blockX = startX + x;
			int blockY = startY;
			int blockZ = startZ + z;

			int index = blockX * chunk.CHUNK_SIZE * chunk.CHUNK_SIZE + blockY * chunk.CHUNK_SIZE + blockZ;
			chunk.blocks[index] = STONE; // Set the block to water
		}
	}
}

void Generators::generate_overhang(Chunk &chunk, int startX, int startY, int startZ, int width, int depth) {
	int overhangHeight = 2; // Height of the overhang

	for (int x = 0; x < width; ++x) {
		for (int z = 0; z < depth; ++z) {
			for (int y = 0; y < overhangHeight; ++y) {
				int blockX = startX + x;
				int blockY = startY + y;
				int blockZ = startZ + z;

				int index = blockX * chunk.CHUNK_SIZE * chunk.CHUNK_SIZE + blockY * chunk.CHUNK_SIZE + blockZ;
				chunk.blocks[index] = STONE; // Set the block to stone
			}
		}
	}
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

	// Randomly generate structures within the room
	int roomX = chunk.room.x;
	int roomY = chunk.room.y;
	int roomZ = chunk.room.z;
	int roomWidth = chunk.room.width;
	int roomHeight = chunk.room.height;
	int roomDepth = chunk.room.depth;

	// Generate a bridge that spans the entire width of the room
	int bridgeY = roomY + 2; // Bridge height
	int bridgeZ = roomZ + roomDepth / 2; // Bridge positioned in the middle of the room's depth
	int bridgeLength = roomWidth; // Bridge spans the entire width of the room
	generate_bridge(chunk, roomX, bridgeY, bridgeZ, bridgeLength);

	// Generate stairs at both ends of the bridge
	int stairWidth = 2; // Width of the stairs
	int stairHeight = 2; // Height of the stairs (to match the bridge height)

						 // Stairs at the start of the bridge (left side)
	generate_stairs(chunk, roomX, roomY, bridgeZ, 1); // Direction: 1 for forward

													  // Stairs at the end of the bridge (right side)
	generate_stairs(chunk, roomX + roomWidth - stairWidth, roomY, bridgeZ, -1); // Direction: -1 for backward


	// Example: Generate a pool
	int poolX = roomX + rand() % (roomWidth - 5); // Random X position within the room
	int poolZ = roomZ + rand() % (roomDepth - 5); // Random Z position within the room
	int poolWidth = 3 + rand() % 5; // Random pool width
	int poolDepth = 3 + rand() % 5; // Random pool depth
	generate_pool(chunk, poolX, roomY, poolZ, poolWidth, poolDepth);

	// Example: Generate an overhang
	int overhangX = roomX + rand() % (roomWidth - 5); // Random X position within the room
	int overhangZ = roomZ + rand() % (roomDepth - 5); // Random Z position within the room
	int overhangWidth = 3 + rand() % 5; // Random overhang width
	int overhangDepth = 3 + rand() % 5; // Random overhang depth
	generate_overhang(chunk, overhangX, roomY + roomHeight - 2, overhangZ, overhangWidth, overhangDepth);


}