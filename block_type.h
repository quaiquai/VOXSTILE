#ifndef BLOCK_TYPE_H
#define BLOCK_TYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum BlockType {
	STONE,
	GRASS,
	INACTIVE
};

static glm::vec3 block_colors[] = {
	glm::vec3(0.5, 0.5, 0.5), //stone
	glm::vec3(0.0, 0.7, 0.0), //grass
};

#endif // !BLOCK_TYPE_H
