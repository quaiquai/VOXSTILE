#pragma once
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "shader.h"

class Portal {
public:

	GLuint fbo;
	GLuint texture;
	GLuint vao, vbo, ebo;
	glm::vec3 position;
	glm::vec3 exit_position;
	glm::mat4 view_matrix;
	glm::mat4 projection_matrix;
	glm::vec3 camera_position;
	glm::mat4 model_matrix;

	float vertices[20] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom-right
		0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top-right
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top-left
	};

	// Define the indices for the quad
	unsigned int indices[6] = {
		0, 1, 2, // First triangle
		2, 3, 0  // Second triangle
	};


	Portal();
	void draw_portal();
	void setup_camera(Shader &portal_shader, glm::vec3 camera_position, glm::vec3 camera_front);
	void setup_framebuffer();
	void renderSceneFromPortalPerspective();
	void drawQuad();
	~Portal() = default;

};