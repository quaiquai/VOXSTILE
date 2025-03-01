
#include "Chunk.h"
#include "noise.h"
#include <iostream>


const int Chunk::CHUNK_SIZE = 32;
const int Chunk::NUMBER_OF_CUBE_VERTS = 24;

int Chunk::CHUNK_COUNT = 0;

Chunk::Chunk(int worldx, int worldz) {
	
	Chunk::chunk_world_xposition = worldx;
	Chunk::chunk_world_zposition = worldz;
	absolute_positionX = CHUNK_SIZE * chunk_world_xposition;
	absolute_positionZ = CHUNK_SIZE * chunk_world_zposition;
	buffers_initialized = false;
	buffers_generated = false;
	blocks_generated = false;
	block_number = 0;
	chunk_id = CHUNK_COUNT;
	blocks.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	// Fill the vector with the desired value
	blocks.assign(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, STONE);
	room = generate_room(0, 0);
	carve_room(room);
	prev_room = nullptr;
	/*
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			int height = generate_height(x + absolute_positionX, z + absolute_positionZ);
			//int height = 10;
			for (int y = 0; y < CHUNK_SIZE; y++) {
				if (y == height - 1) {
					blocks.push_back(GRASS);
				}
				if (y < height - 1) {
					blocks.push_back(STONE);
				}
				if (y > height-1) {
					blocks.push_back(INACTIVE);
				}
			}
		}
	}
	*/
	//vertices.reserve(Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * 256 * 24);
	//colors.reserve(Chunk::CHUNK_SIZE *Chunk::CHUNK_SIZE *256*24);
	
	++CHUNK_COUNT;
}

Chunk::Chunk(const Chunk &c) {
	VertexArrayID = c.VertexArrayID;
	vertex_buffer = c.vertex_buffer;
	normalBuffer = c.normalBuffer;
	colorBuffer = c.colorBuffer;
	tangent_buffer = c.tangent_buffer;
	bitangent_buffer = c.bitangent_buffer;
	IndexBuffer = c.IndexBuffer;
	texture_buffer = c.texture_buffer;
	chunk_world_xposition = c.chunk_world_xposition;
	chunk_world_zposition = c.chunk_world_zposition;
	absolute_positionX = CHUNK_SIZE * chunk_world_xposition;
	absolute_positionZ = CHUNK_SIZE * chunk_world_zposition;
	buffers_initialized = c.buffers_initialized;
	buffers_generated = c.buffers_generated;
	blocks_generated = c.blocks_generated;
	blocks = c.blocks;
	vertices = c.vertices;
	colors = c.colors;
	normals = c.normals;
	indices = c.indices;
	tangents = c.tangents;
	bitangents = c.bitangents;
	tex_coords = c.tex_coords;
	block_number = c.block_number;
	chunk_id = c.chunk_id;

}

Chunk::Chunk(Chunk&& other) noexcept
	: VertexArrayID(other.VertexArrayID),
	vertex_buffer(other.vertex_buffer),
	normalBuffer(other.normalBuffer),
	colorBuffer(other.colorBuffer),
	IndexBuffer(other.IndexBuffer),
	tangent_buffer(other.tangent_buffer),
	bitangent_buffer(other.bitangent_buffer),
	texture_buffer(other.texture_buffer),
	chunk_world_xposition(other.chunk_world_xposition),
	chunk_world_zposition(other.chunk_world_zposition),
	absolute_positionX(other.absolute_positionX),
	absolute_positionZ(other.absolute_positionZ),
	buffers_initialized(other.buffers_initialized),
	buffers_generated(other.buffers_generated),
	blocks_generated(other.blocks_generated),
	block_number(other.block_number),
	chunk_id(other.chunk_id),
	vertices(std::move(other.vertices)),
	tex_coords(std::move(other.tex_coords)),
	colors(std::move(other.colors)),
	normals(std::move(other.normals)),
	indices(std::move(other.indices)),
	tangents(std::move(other.tangents)),
	bitangents(std::move(other.bitangents)),
	blocks(std::move(other.blocks)){

	
	other.VertexArrayID = 0;
	other.vertex_buffer = 0;
	other.normalBuffer = 0;
	other.IndexBuffer = 0;
	other.colorBuffer = 0;
	other.texture_buffer = 0;
	other.tangent_buffer = 0;
	other.bitangent_buffer = 0;
	
}

Chunk& Chunk::operator=(Chunk&& other) noexcept {
	if (this != &other) {  // Prevent self-assignment
						   // Free existing dynamically allocated resources

		// Move data from `other`
		VertexArrayID = other.VertexArrayID;
		vertex_buffer = other.vertex_buffer;
		normalBuffer = other.normalBuffer;
		colorBuffer = other.colorBuffer;
		IndexBuffer = other.IndexBuffer;
		tangent_buffer = other.tangent_buffer;
		bitangent_buffer = other.bitangent_buffer;
		texture_buffer = other.texture_buffer;
		chunk_world_xposition = other.chunk_world_xposition;
		chunk_world_zposition = other.chunk_world_zposition;
		absolute_positionX = other.absolute_positionX;
		absolute_positionZ = other.absolute_positionZ;
		block_number = other.block_number;
		chunk_id = other.chunk_id;
		buffers_initialized = other.buffers_initialized;
		buffers_generated = other.buffers_generated;
		blocks_generated = other.blocks_generated;
	

								   // Move STL containers
		vertices = std::move(other.vertices);
		colors = std::move(other.colors);
		normals = std::move(other.normals);
		indices = std::move(other.indices);
		blocks = std::move(other.blocks);
		tangents = std::move(other.tangents);
		bitangents = std::move(other.bitangents);
		tex_coords = std::move(other.tex_coords);
		
		other.VertexArrayID = 0;
		other.vertex_buffer = 0;
		other.normalBuffer = 0;
		other.IndexBuffer = 0;
		other.colorBuffer = 0;
		other.texture_buffer = 0;
		other.tangent_buffer = 0;
		other.bitangent_buffer = 0;
	}
	return *this;
}

void Chunk::generate_buffers() {
	glGenVertexArrays(1, &VertexArrayID);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &colorBuffer);
	glGenBuffers(1, &IndexBuffer);
	glGenBuffers(1, &texture_buffer);
	glGenBuffers(1, &tangent_buffer);
	glGenBuffers(1, &bitangent_buffer);
	//glGenTextures(1, &textureID);
	buffers_generated = true;
}

void Chunk::generate_hallways(Room prevroom) {
	int x1 = prevroom.x + prevroom.width / 2, z1 = prevroom.z + prevroom.depth / 2;
	int x2 = room.x + room.width / 2, z2 = room.z + room.depth / 2;
	int y = room.y;
	while (CHUNK_SIZE * x1 != CHUNK_SIZE *  x2) { blocks[x1*CHUNK_SIZE*CHUNK_SIZE + y * CHUNK_SIZE + z1] = GRASS; x1 += (CHUNK_SIZE *x1 < CHUNK_SIZE *x2) ? 1 : -1; }
	while (CHUNK_SIZE *z1 != CHUNK_SIZE * z2) { blocks[x1*CHUNK_SIZE*CHUNK_SIZE + y * CHUNK_SIZE + z1] = GRASS; z1 += (CHUNK_SIZE *z1 < CHUNK_SIZE *z2) ? 1 : -1; }
}

Room Chunk::generate_room(int chunkX, int chunkZ) {
	int maxWidth = (CHUNK_SIZE-1) / 2;
	int maxDepth = (CHUNK_SIZE-1) / 2;
	int maxHeight = (CHUNK_SIZE-1) / 2;

	int width = 10 + rand() % (maxWidth);
	int depth = 10 + rand() % (maxDepth);
	int height = 10 + rand() % (maxHeight);

	int x = rand() % (CHUNK_SIZE-1 - width);
	int z = rand() % (CHUNK_SIZE-1 - depth);
	int y = rand() % (CHUNK_SIZE-1 - height);
	BlockType type = STONE;
	return { x, y, z, width, height, depth, type };
}

void Chunk::carve_room(Room room) {
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				// Calculate the 1D index in the chunk array
				int index = x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z;

				// Check if the block is inside the room's boundaries
				bool insideRoom = (x >= room.x && x < room.x + room.width &&
					y >= room.y && y < room.y + room.height &&
					z >= room.z && z < room.z + room.depth);

				// Check if the block is on the immediate border of the room
				bool onBorder = false;
				if ((x == room.x - 1 || x == room.x + room.width) &&
					y >= room.y && y < room.y + room.height &&
					z >= room.z && z < room.z + room.depth) {
					onBorder = true;
				}
				else if ((y == room.y - 1 || y == room.y + room.height) &&
					x >= room.x && x < room.x + room.width &&
					z >= room.z && z < room.z + room.depth) {
					onBorder = true;
				}
				else if ((z == room.z - 1 || z == room.z + room.depth) &&
					x >= room.x && x < room.x + room.width &&
					y >= room.y && y < room.y + room.height) {
					onBorder = true;
				}

				if (blocks[index] == GRASS) {
					continue;
				}

				if (insideRoom) {
					// Inside the room, set the block as inactive
					blocks[index] = INACTIVE;
				}
				else if (onBorder) {
					// Border blocks, set the block as stone
					if ((z == 12 || z== 13 ||z==14) && y > room.height) {
						blocks[index] = INACTIVE;
					}
				
					
				}
				else {
					// Outside the room and not bordering, set the block as inactive
					blocks[index] = INACTIVE;
				}
			}
		}
	}
		
}


float Chunk::generate_height(int x, int z) {
	float n = Noise2D(x * 0.05, z * 0.05);
	n += 1.0f;
	n *= 10.0f;
	
	return n;
}

void Chunk::generate_mesh() {
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int y = 0; y < CHUNK_SIZE; y++) {
			for (int z = 0; z < CHUNK_SIZE; z++) {
				create_cube(x, y, z);
			}
		}
	}
	blocks_generated = true;
}

void Chunk::create_mesh() {
	const int CHUNK_VOL = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	const int directions[6][3] = { { 1, 0, 0 },{ -1, 0, 0 },{ 0, 1, 0 },
	{ 0, -1, 0 },{ 0, 0, 1 },{ 0, 0, -1 } };

	for (int d = 0; d < 3; ++d) { // d = 0 (x), d = 1 (y), d = 2 (z)
		int u = (d + 1) % 3;
		int v = (d + 2) % 3;

		std::vector<int> mask(CHUNK_SIZE * CHUNK_SIZE);

		for (int slice = 0; slice <= CHUNK_SIZE; ++slice) {
			std::fill(mask.begin(), mask.end(), -1);

			for (int x = 0; x < CHUNK_SIZE; ++x) {
				for (int z = 0; z < CHUNK_SIZE; ++z) {
					int index = x * CHUNK_SIZE + z;
					int id = -1;

					if (slice < CHUNK_SIZE) {
						int a[3] = { x, slice, z };
						int b[3] = { x, slice - 1, z };

						int currBlock = blocks[a[0] * CHUNK_SIZE * CHUNK_SIZE + a[2] * CHUNK_SIZE + a[1]];
						int prevBlock = (slice > 0) ? blocks[b[0] * CHUNK_SIZE * CHUNK_SIZE + b[2] * CHUNK_SIZE + b[1]] : -1;

						if (currBlock != prevBlock && currBlock != 2) {
							id = currBlock;
						}
					}
					mask[index] = id;
				}
			}

			for (int x = 0; x < CHUNK_SIZE; ++x) {
				for (int z = 0; z < CHUNK_SIZE; ++z) {
					int id = mask[x * CHUNK_SIZE + z];
					if (id == -1) continue;

					int width = 1, height = 1;
					while (x + width < CHUNK_SIZE && mask[(x + width) * CHUNK_SIZE + z] == id) {
						width++;
					}
					while (z + height < CHUNK_SIZE) {
						bool valid = true;
						for (int k = 0; k < width; ++k) {
							if (mask[(x + k) * CHUNK_SIZE + z + height] != id) {
								valid = false;
								break;
							}
						}
						if (!valid) break;
						height++;
					}

					for (int i = x; i < x + width; ++i) {
						for (int j = z; j < z + height; ++j) {
							mask[i * CHUNK_SIZE + j] = -1;
						}
					}

					float normal[3] = { 0.0f, 0.0f, 0.0f };
					normal[d] = (slice == CHUNK_SIZE) ? -1.0f : 1.0f;

					int a[3] = { x, slice, z };
					int b[3] = { x + width, slice, z };
					int c[3] = { x, slice, z + height };
					int d[3] = { x + width, slice, z + height };


					int baseIndex = vertices.size();
					vertices.insert(vertices.end(), { float(a[0]), float(a[1]), float(a[2]), normal[0], normal[1], normal[2], 1.0f, 1.0f, 1.0f });
					vertices.insert(vertices.end(), { float(b[0]), float(b[1]), float(b[2]), normal[0], normal[1], normal[2], 1.0f, 1.0f, 1.0f });
					vertices.insert(vertices.end(), { float(c[0]), float(c[1]), float(c[2]), normal[0], normal[1], normal[2], 1.0f, 1.0f, 1.0f });
					vertices.insert(vertices.end(), { float(d[0]), float(d[1]), float(d[2]), normal[0], normal[1], normal[2], 1.0f, 1.0f, 1.0f });

					indices.push_back(baseIndex);
					indices.push_back(baseIndex + 1);
					indices.push_back(baseIndex + 2);
					indices.push_back(baseIndex + 2);
					indices.push_back(baseIndex + 1);
					indices.push_back(baseIndex + 3);
				}
			}
		}
	}
}

// Function to calculate tangent and bitangent for a triangle
void Chunk::calculate_tangent_bitangent(
	const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
	const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2,
	glm::vec3& tangent, glm::vec3& bitangent) {

	glm::vec3 edge1 = v1 - v0;
	glm::vec3 edge2 = v2 - v0;
	glm::vec2 deltaUV1 = uv1 - uv0;
	glm::vec2 deltaUV2 = uv2 - uv0;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);
}



void Chunk::create_cube(int x, int y, int z) {

	
	int block_index = x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z;
	if (blocks[block_index] == INACTIVE) {
		return;
	}
	/*
	int left = (x > 0) ? (blocks[(x - 1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y]) : 2;
	int right = (x < CHUNK_SIZE - 1) ? (blocks[(x + 1) * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + y]) : 2;

	int below = (y > 0) ? (blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (y-1)]) : -1;
	int above = (y < CHUNK_SIZE - 1) ? (blocks[x * CHUNK_SIZE * CHUNK_SIZE + z * CHUNK_SIZE + (y+1)]) : 2;

	int back = (z > 0) ? (blocks[x * CHUNK_SIZE * CHUNK_SIZE + (z-1) * CHUNK_SIZE + y]) : -1;
	int front = (z < CHUNK_SIZE - 1) ? (blocks[x * CHUNK_SIZE * CHUNK_SIZE + (z+1) * CHUNK_SIZE + y]) : 2;
	if (blocks[block_index] == INACTIVE) {
		return;
	}
	if (left != 2 && right != 2 && below != 2 && above != 2 && back != 2 &&
		front != 2) {
		return;
	}
	*/

	
	// Create cube vertices based on block_render_size and x, y, z offsets
	glm::vec3 p0 = {(x - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y - Block::BLOCK_RENDER_SIZE / 2.0f, (z + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p1 = {(x + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y - Block::BLOCK_RENDER_SIZE / 2.0f, (z + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p2 = {(x + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y + Block::BLOCK_RENDER_SIZE / 2.0f, (z + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p3 = {(x - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y + Block::BLOCK_RENDER_SIZE / 2.0f, (z + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p4 = {(x + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y - Block::BLOCK_RENDER_SIZE / 2.0f, (z - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p5 = {(x - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y - Block::BLOCK_RENDER_SIZE / 2.0f, (z - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p6 = {(x - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y + Block::BLOCK_RENDER_SIZE / 2.0f, (z - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };
	glm::vec3 p7 = {(x + Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_xposition * Chunk::CHUNK_SIZE), y + Block::BLOCK_RENDER_SIZE / 2.0f, (z - Block::BLOCK_RENDER_SIZE / 2.0f) + (chunk_world_zposition * Chunk::CHUNK_SIZE) };

	//need to know which block we are on because the vertices array is 1D and the values change based on the position in the chunk
	//using this number we can properly index into it for indices and other stuff
	int baseVertexIndex = block_number * NUMBER_OF_CUBE_VERTS;

	//Start of the index buffer creation. append to the end of the vector the correct order of the indices
	// Flatten and insert vertices for each face (24 vertices total per cube)
	vertices.insert(vertices.end(), { p0.x, p0.y, p0.z, 
									  p1.x, p1.y, p1.z, 
									  p2.x, p2.y, p2.z, 
									  p3.x, p3.y, p3.z }); // Front face
	vertices.insert(vertices.end(), { p4.x, p4.y, p4.z, 
									  p5.x, p5.y, p5.z, 
								      p6.x, p6.y, p6.z, 
								      p7.x, p7.y, p7.z }); // Back face
	vertices.insert(vertices.end(), { p5.x, p5.y, p5.z, 
							          p0.x, p0.y, p0.z, 
									  p3.x, p3.y, p3.z, 
							          p6.x, p6.y, p6.z }); // Left face
	vertices.insert(vertices.end(), { p1.x, p1.y, p1.z, 
									  p4.x, p4.y, p4.z, 
									  p7.x, p7.y, p7.z, 
									  p2.x, p2.y, p2.z }); // Right face
	vertices.insert(vertices.end(), { p3.x, p3.y, p3.z, 
									  p2.x, p2.y, p2.z, 
									  p7.x, p7.y, p7.z, 
									  p6.x, p6.y, p6.z }); // Top face
	vertices.insert(vertices.end(), { p5.x, p5.y, p5.z, 
									  p4.x, p4.y, p4.z, 
									  p1.x, p1.y, p1.z, 
									  p0.x, p0.y, p0.z }); // Bottom face

	for (int i = 0; i < 24; ++i) {
		colors.insert(colors.end(), { block_colors[blocks[block_index]].x, block_colors[blocks[block_index]].y, block_colors[blocks[block_index]].z });
	}

	float texLayer = 0;  // Normalize layer index (0 to 1)
	//float texLayer = blockType / float(numLayers - 1);  // Normalize layer index (0 to 1)

														// Front face
														// Front face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

	// Back face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

	// Left face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

	// Right face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

	// Top face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

	// Bottom face
	tex_coords.insert(tex_coords.end(), {
		0.0f, 0.0f, texLayer,  // bottom-left
		1.0f, 0.0f, texLayer,  // bottom-right
		1.0f, 1.0f, texLayer,  // top-right
		0.0f, 1.0f, texLayer   // top-left
		});

    // Normals for flat shading (6 faces, each with the same normal for 4 vertices)
	// Front face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { 0.0f, 0.0f, 1.0f });
	}

	// Back face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { 0.0f, 0.0f, -1.0f });
	}

	// Left face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { -1.0f, 0.0f, 0.0f });
	}

	// Right face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { 1.0f, 0.0f, 0.0f });
	}

	// Top face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { 0.0f, 1.0f, 0.0f });
	}

	// Bottom face normal
	for (int i = 0; i < 4; ++i) {
		normals.insert(normals.end(), { 0.0f, -1.0f, 0.0f });
	}

															  // Add indices for the cube (use block_number to offset each cube's indices)
	indices.insert(indices.end(), {
		// Front face
		baseVertexIndex + 0, baseVertexIndex + 1, baseVertexIndex + 2,
		baseVertexIndex + 0, baseVertexIndex + 2, baseVertexIndex + 3,

		// Back face
		baseVertexIndex + 4, baseVertexIndex + 5, baseVertexIndex + 6,
		baseVertexIndex + 4, baseVertexIndex + 6, baseVertexIndex + 7,

		// Left face
		baseVertexIndex + 8, baseVertexIndex + 9, baseVertexIndex + 10,
		baseVertexIndex + 8, baseVertexIndex + 10, baseVertexIndex + 11,

		// Right face
		baseVertexIndex + 12, baseVertexIndex + 13, baseVertexIndex + 14,
		baseVertexIndex + 12, baseVertexIndex + 14, baseVertexIndex + 15,

		// Top face
		baseVertexIndex + 16, baseVertexIndex + 17, baseVertexIndex + 18,
		baseVertexIndex + 16, baseVertexIndex + 18, baseVertexIndex + 19,

		// Bottom face
		baseVertexIndex + 20, baseVertexIndex + 21, baseVertexIndex + 22,
		baseVertexIndex + 20, baseVertexIndex + 22, baseVertexIndex + 23
	});


	auto calculateTangents = [](const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2) {
		glm::vec3 edge1 = v1 - v0;
		glm::vec3 edge2 = v2 - v0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent;
		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		//tangent = glm::normalize(tangent);

		glm::vec3 bitangent;
		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		//bitangent = glm::normalize(bitangent);

		return std::make_pair(tangent, bitangent);
	};

	// Function to insert tangents and bitangents for a triangle
	auto insertTangentsAndBitangents = [&](const glm::vec3& tangent, const glm::vec3& bitangent, int count) {
		for (int i = 0; i < count; ++i) {
			tangents.insert(tangents.end(), { tangent.x, tangent.y, tangent.z });
			bitangents.insert(bitangents.end(), { bitangent.x, bitangent.y, bitangent.z });
		}
	};

	// Calculate tangents and bitangents for each triangle of each face
		// Front face (two triangles: 0-1-2 and 0-2-3)
	
	{
		auto tb = calculateTangents(p0, p1, p2,{ 0.0f, 0.0f },{ 1.0f, 0.0f },{ 1.0f, 1.0f });

		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 0-1-2
	}

		// Back face (two triangles: 4-5-6 and 4-6-7)
	{
		auto tb = calculateTangents(p4, p5, p6, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f });
		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 4-5-6
	}

	// Left face (two triangles: 5-0-3 and 5-3-6)
	{
		auto tb = calculateTangents(p5, p0, p3, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f });
		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 5-0-3
	}

	// Right face (two triangles: 1-4-7 and 1-7-2)
	{
		auto tb = calculateTangents(p1, p4, p7, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f });
		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 1-4-7
	}

	// Top face (two triangles: 3-2-7 and 3-7-6)
	{
		auto tb = calculateTangents(p3, p2, p7, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f });
		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 3-2-7
	}

	// Bottom face (two triangles: 5-4-1 and 5-1-0)
	{
		auto tb = calculateTangents(p5, p4, p1, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f });
		insertTangentsAndBitangents(tb.first, tb.second, 4); // For triangle 5-4-1
	}


	++block_number;
}


Chunk::~Chunk() { 
	// Delete the buffers
	
	if (colorBuffer && normalBuffer && IndexBuffer) {
		glDeleteBuffers(1, &vertex_buffer);
		glDeleteBuffers(1, &normalBuffer);
		glDeleteBuffers(1, &colorBuffer);
		glDeleteBuffers(1, &IndexBuffer);
		glDeleteBuffers(1, &texture_buffer);
		glDeleteBuffers(1, &bitangent_buffer);
		glDeleteVertexArrays(1, &VertexArrayID);
	}
	
	
	
}