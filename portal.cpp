#include "portal.h"


Portal::Portal() {

	float vertices[] = {
		// Positions          // Texture Coords
		-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // Bottom-left
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // Bottom-right
		0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // Top-right
		-0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // Top-left
	};

	position = { 0.0, 0.0, 0.0 };
	model_matrix = glm::mat4(1.0);
	camera_position = { 14.0, 10.0, 10.0 };

	// Define the indices for the quad
	unsigned int indices[] = {
		0, 1, 2, // First triangle
		2, 3, 0  // Second triangle
	};

	// Create the VAO, VBO, and EBO
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	// Bind the VAO
	glBindVertexArray(vao);

	// Bind and upload the VBO
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind and upload the EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Set up the vertex attributes
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind the VAO
	glBindVertexArray(0);

}

void Portal::setup_camera(Shader &portal_shader, glm::vec3 cp, glm::vec3 camera_front) {

	//cp = to the relative positon of the main camera to the main portal (entrance portal)

	// Set up the view matrix
	//using the relative position of the main portal to the entrance of it we adjust the exit portals camera to the same position by adding the
	// relative position to the camera positon. this kinda works for now because we dont have a set exit portal positon.
	view_matrix = glm::lookAt(camera_position + cp, (camera_position + cp) + camera_front, glm::vec3(0.0f, 1.0f, 0.0f));

	// Set up the projection matrix (perspective projection)
	projection_matrix = glm::perspective(glm::radians(90.0f), (float)1920 / (float)1080, 0.1f, 10000.0f);

	// Upload matrices to the shader (assuming you have a shader program)
	portal_shader.setMat4("view", view_matrix);
	portal_shader.setMat4("projection", projection_matrix);

}

void Portal::drawQuad() {
	// Bind the VAO
	glBindVertexArray(vao);

	// Bind the texture (if provided)
	if (texture != 0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	// Draw the quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Unbind the VAO
	glBindVertexArray(0);
}

void Portal::renderSceneFromPortalPerspective() {
	// Bind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 1920, 1080);

	// Clear the texture
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the camera for the portal's perspective
	//Portal::setup_camera();

	// Render a quad as seen through the portal
	//drawQuad(quadVAO, 0); // No texture for the scene quad

						  // Unbind the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}