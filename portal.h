#pragma once
#include "glad/glad.h"
#include <iostream>
#include "renderer.h"
#include "shader.h"

class Portal {
public:

	GLuint fbo;
	GLuint texture;
	GLuint vao, vbo, ebo;
	glm::vec3 position;
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
	glm::vec3 camera_position;
	glm::mat4 model_matrix;

	Portal();
	void draw_portal();
	void setup_camera(Shader &portal_shader, glm::vec3 camera_position, glm::vec3 camera_front);
	void renderSceneFromPortalPerspective();
	void drawQuad();
	~Portal() = default;

};