
#include "Chunk.h"
#include "noise.h"
#include <iostream>


const int Chunk::CHUNK_SIZE = 16;
const int Chunk::NUMBER_OF_CUBE_VERTS = 24;

int Chunk::CHUNK_COUNT = 0;

Chunk::Chunk(int worldx, int worldz) {
	glGenVertexArrays(1, &VertexArrayID);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &normalBuffer);
	glGenBuffers(1, &colorBuffer);
	glGenBuffers(1, &IndexBuffer);
	Chunk::chunk_world_xposition = worldx;
	Chunk::chunk_world_zposition = worldz;
	absolute_positionX = CHUNK_SIZE * chunk_world_xposition;
	absolute_positionZ = CHUNK_SIZE * chunk_world_zposition;
	m_pBlocks = new Block **[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; i++) {
		m_pBlocks[i] = new Block *[CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			m_pBlocks[i][j] = new Block[CHUNK_SIZE];
		}
	}
	block_number = 0;
	//vertices.reserve(Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE * 256 * 24);
	//colors.reserve(Chunk::CHUNK_SIZE *Chunk::CHUNK_SIZE *256*24);
	chunk_id = CHUNK_COUNT;
	++CHUNK_COUNT;
}

Chunk::Chunk(const Chunk &c) {
	VertexArrayID = c.VertexArrayID;
	vertex_buffer = c.vertex_buffer;
	normalBuffer = c.normalBuffer;
	colorBuffer = c.colorBuffer;
	IndexBuffer = c.IndexBuffer;
	chunk_world_xposition = c.chunk_world_xposition;
	chunk_world_zposition = c.chunk_world_zposition;
	absolute_positionX = CHUNK_SIZE * chunk_world_xposition;
	absolute_positionZ = CHUNK_SIZE * chunk_world_zposition;
	m_pBlocks = new Block **[CHUNK_SIZE];
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		m_pBlocks[i] = new Block *[CHUNK_SIZE];
		for (int j = 0; j < CHUNK_SIZE; j++) {
			m_pBlocks[i][j] = new Block[CHUNK_SIZE];
			for (int k = 0; k < CHUNK_SIZE; k++) {
				m_pBlocks[i][j][k] = c.m_pBlocks[i][j][k]; // Copy each Block
			}
		}
	}
	vertices = c.vertices;
	colors = c.colors;
	normals = c.normals;
	indices = c.indices;
	block_number = c.block_number;
	chunk_id = c.chunk_id;
}

Chunk::Chunk(Chunk&& other) noexcept
	: VertexArrayID(other.VertexArrayID),
	vertex_buffer(other.vertex_buffer),
	normalBuffer(other.normalBuffer),
	colorBuffer(other.colorBuffer),
	IndexBuffer(other.IndexBuffer),
	chunk_world_xposition(other.chunk_world_xposition),
	chunk_world_zposition(other.chunk_world_zposition),
	absolute_positionX(other.absolute_positionX),
	absolute_positionZ(other.absolute_positionZ),
	m_pBlocks(other.m_pBlocks), // Move ownership of dynamic array
	block_number(other.block_number),
	chunk_id(other.chunk_id),
	vertices(std::move(other.vertices)),
	colors(std::move(other.colors)),
	normals(std::move(other.normals)),
	indices(std::move(other.indices)) {

	// Invalidate other's resources to prevent double deletion
	other.m_pBlocks = nullptr;
}

Chunk& Chunk::operator=(Chunk&& other) noexcept {
	if (this != &other) {  // Prevent self-assignment
						   // Free existing dynamically allocated resources
		delete[] m_pBlocks;

		// Move data from `other`
		VertexArrayID = other.VertexArrayID;
		vertex_buffer = other.vertex_buffer;
		normalBuffer = other.normalBuffer;
		colorBuffer = other.colorBuffer;
		IndexBuffer = other.IndexBuffer;
		chunk_world_xposition = other.chunk_world_xposition;
		chunk_world_zposition = other.chunk_world_zposition;
		absolute_positionX = other.absolute_positionX;
		absolute_positionZ = other.absolute_positionZ;
		block_number = other.block_number;
		chunk_id = other.chunk_id;

		// Move ownership of dynamically allocated array
		m_pBlocks = other.m_pBlocks;
		other.m_pBlocks = nullptr; // Ensure the moved-from object is safe

								   // Move STL containers
		vertices = std::move(other.vertices);
		colors = std::move(other.colors);
		normals = std::move(other.normals);
		indices = std::move(other.indices);
	}
	return *this;
}


float Chunk::generate_height(int x, int z) {
	float n = Noise2D(x * 0.05, z * 0.05);
	n += 1.0f;
	n *= 10.0f;
	
	return n;
}

void Chunk::remove_heights() {
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			int height = generate_height(x + absolute_positionX, z + absolute_positionZ);
			for (int y = 0; y < CHUNK_SIZE; y++) {
				
				
				if (y == height - 1) {
					m_pBlocks[x][y][z].m_blockType = GRASS;
				}
				if (y >= height) {
					m_pBlocks[x][y][z].is_active = false;
				}
				else {
					create_cube(x, y, z, height);
				}
				
			}
		}
	}
}


void Chunk::create_mesh() {
	for (int x = 0; x < CHUNK_SIZE; x++) {
		for (int z = 0; z < CHUNK_SIZE; z++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				if (!(m_pBlocks[x][y][z].is_active)) {
					continue;
				}
				//create_cube(x, y, z);
			}
		}
	}
}


void Chunk::create_cube(int x, int y, int z, int height) {

	//if this block is covered by other blocks, we should not render
	//if (x > 0 && x < Chunk::CHUNK_SIZE-1) {
		//if (z > 0 && z < Chunk::CHUNK_SIZE-1) {
			if (y >= 0 && y < Chunk::CHUNK_SIZE - 1 && y < height-1) {
				//if (m_pBlocks[x + 1][y][z].is_active && m_pBlocks[x][y][z + 1].is_active && m_pBlocks[x - 1][y][z].is_active && m_pBlocks[x][y][z - 1].is_active && m_pBlocks[x][y + 1][z].is_active && m_pBlocks[x][y - 1][z].is_active) {
					return;
				//}
			}
		//}
	//}
	
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
		colors.insert(colors.end(), { block_colors[m_pBlocks[x][y][z].m_blockType].x, block_colors[m_pBlocks[x][y][z].m_blockType].y, block_colors[m_pBlocks[x][y][z].m_blockType].z });
	}

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
	++block_number;
}


Chunk::~Chunk() { // Delete the blocks
	if (!m_pBlocks) return;
	for (int i = 0; i < CHUNK_SIZE; ++i) {
		for (int j = 0; j < CHUNK_SIZE; ++j) {
			delete[] m_pBlocks[i][j];
		}
		delete[] m_pBlocks[i];
	}
	delete[] m_pBlocks;
	m_pBlocks = nullptr;

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &IndexBuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}