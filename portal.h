#pragma once
#include "glad/glad.h"
#include <iostream>
#include "renderer.h"

class Portal {
public:

	GLuint fbo;
	GLuint texture;

	Portal() = default;
	~Portal() = default;

};