#pragma once
#include "glad/glad.h"
#include <iostream>
#include "renderer.h"

class Portal {
public:

	GLuint portal_fb;
	GLuint portal_texture;

	Portal() = default;
	~Portal() = default;

};