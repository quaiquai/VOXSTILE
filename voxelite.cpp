// voxelite.cpp : Defines the entry point for the console application.
//
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>

#include "block.h"
#include "shader.h"
#include "chunk.h"
#include "terrain_chunk.h"
#include "chunk_manager.h"
#include "renderer.h"
#include "portal.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

bool cursorCallbackMode = false;

// camera
glm::vec3 cameraPos = glm::vec3(14.0f, 20.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//portal camera
glm::vec3 portalcameraPos = glm::vec3(20.0f, 5.0f, 20.0f);
glm::vec3 portalcameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 portalcameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 90.0f;

int current_visited_chunk_index;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float highBias = 0.004;
float lowBias = 0.003;
float sample_texel = 1.0;

int total_verts = 0;

int main()
{

	GLFWwindow* window;
	Renderer renderer;

	

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "Voxelite", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	gladLoadGL();

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;       // if using Docking Branch
	

															  // Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	//renderer.renderWireframes();

	// Create and compile our GLSL program from the shaders
	Shader basic = Shader("shaders//basic_vs.glsl", "shaders//basic_fs.glsl");
	Shader depth = Shader("shaders//light_depth_vs.glsl", "shaders//light_depth_fs.glsl");
	Shader portal_shader = Shader("shaders//portal_vs.glsl", "shaders//portal_fs.glsl");

	glm::vec3 lightposition(-0.0f, 36.0f, -1.0f);
	glm::vec3 lightat(0.0, 0.0, 0.0);
	glm::vec3 lightDirection(lightat - lightposition);
	//glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

	ChunkManager chunk_manager(cameraPos);

	// Create a 2D array texture
	GLuint arrayTexture;
	glGenTextures(1, &arrayTexture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);

	GLsizei mipLevelCount = 1;
	const int numLayers = 4;  // Number of images to load into 3D texture
	
	/*
	const char* textureFiles[numLayers] = {
		"textures\\poolroom_tile\\WhiteTiles02_2K_BaseColor.png",
		"textures\\poolroom_tile\\WhiteTiles02_2K_Normal.png",
		"textures\\poolroom_tile\\WhiteTiles02_2K_Roughness.png",
		"textures\\poolroom_tile\\WhiteTiles02_2K_AO.png",
	};
	*/
	const char* textureFiles[numLayers] = {
		"textures\\white_tile\\t_Color.png",
		"textures\\white_tile\\t_NormalGL.png",
		"textures\\white_tile\\t_Roughness.png",
		"textures\\white_tile\\t_AmbientOcclusion.png",
	};
	
	
	int width, height, channels;
	unsigned char *first_image = stbi_load(textureFiles[0], &width, &height, &channels, 4); // Force 4 channels
	GLenum format = GL_RGBA; // Always use RGBA to ensure consistency

							 // Allocate memory for all texture data
	size_t layerSize = width * height * 4; // Always assume 4 channels per pixel
	std::vector<unsigned char> textureData(layerSize * numLayers); // Allocate memory for all layers

																   // Copy the first image into the texture data
	std::memcpy(textureData.data(), first_image, layerSize);
	stbi_image_free(first_image);

	// Load the remaining images and copy their data
	for (int i = 1; i < numLayers; i++) {
		unsigned char *image = stbi_load(textureFiles[i], &width, &height, &channels, 4); // Force 4 channels
		if (!image) {
			std::cerr << "Failed to load texture: " << textureFiles[i] << std::endl;
			return -1;
		}

		// Copy the image data into the appropriate location in the texture data
		std::memcpy(textureData.data() + i * layerSize, image, layerSize);
		stbi_image_free(image);
	}

	// Upload all texture data in one pass
	glTexImage3D(GL_TEXTURE_2D_ARRAY,
		0,                 // mipmap level
		GL_RGBA8,          // Internal GPU format (ensure RGBA8 for 4 channels)
		width,             // width
		height,            // height
		numLayers,         // depth (number of textures)
		0,                 // border
		GL_RGBA,           // Pixel format
		GL_UNSIGNED_BYTE,  // Pixel data type
		textureData.data());

	// Check for OpenGL errors
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: " << err << std::endl;
	}

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 6);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	/////////////////////////////
	// Portal Texture and framebuffer
	/////////////////////////////
	//Portal portal;
	//renderer.init_framebuffer(portal);

	///////////////////////////////
	///////////////////////////////
	////////////////////////////////


	/////////////////////
	//Depth map fbo
	/////////////////////
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	///////////////////////////
	///////////////////////////
	///////////////////////////


	
	float near_plane = 1.0f, far_plane = 200.5f;
	
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		/* Poll for and process events */
		glfwPollEvents();
		io.ConfigFlags = cursorCallbackMode ? io.ConfigFlags & ~ImGuiConfigFlags_NoMouse : io.ConfigFlags | ImGuiConfigFlags_NoMouse;
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow(); // Show demo window! :)


		ImGui::Begin("Editor");
		if (ImGui::SliderFloat3("lp", &lightposition[0], -100.0f, 500.0f)) {
			
		};
		ImGui::InputFloat("lowBias", &lowBias, 0.0, 0.0, "%.6f");
		ImGui::InputFloat("highBias", &highBias, 0.0, 0.0, "%.6f");
		ImGui::InputFloat("texel sample", &sample_texel);
		ImGui::Text("Number of Vertices in Scene = %i", chunk_manager.total_verts);
		ImGui::End();
		

		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//positioning
		int currX = cameraPos.x / chunk_manager.CHUNK_SIZE;
		int currz = cameraPos.z / chunk_manager.CHUNK_SIZE;
		if (chunk_manager.last_x_chunk != currX || chunk_manager.last_z_chunk != currz) {
			chunk_manager.update_visible_chunks(cameraPos);
		}
		
		chunk_manager.add_pending_chunks();
		chunk_manager.generate_chunk_buffers();
		
		renderer.initChunkBuffers(chunk_manager);


		depth.use();
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightposition, lightat, glm::vec3(0.0f, 1.0f, 0.0f));
		lightDirection = lightat - lightposition;
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		depth.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		/* Render here */
		glClearColor(135.0 / 255.0, 206.0 / 255.0, 235.0 / 255.0, 1.0f);
		
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// floor
		glm::mat4 model = glm::mat4(1.0f);
		depth.setMat4("model", model);
		glCullFace(GL_FRONT);
		chunk_manager.render_chunks();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		
		

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 10000.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


		basic.use();

		//iterator pointing to the chunk that matches where the current player is
		std::vector<Chunk>::iterator current_visited_chunk;
		{
			std::lock_guard<std::mutex> lock(chunk_manager.chunk_mutex);
			current_visited_chunk = std::find_if(chunk_manager.chunks.begin(), chunk_manager.chunks.end(), [currX, currz](const Chunk& obj) {
				return (obj.chunk_world_xposition == currX && obj.chunk_world_zposition == currz);
			});
		}
		//gets the index into chunk_manager chunks list for the chunk the player is in
		if (current_visited_chunk != chunk_manager.chunks.end()) {
			current_visited_chunk_index = std::distance(chunk_manager.chunks.begin(), current_visited_chunk);
			chunk_manager.chunks[current_visited_chunk_index].configure_portal(basic, cameraPos, cameraFront);
			glBindFramebuffer(GL_FRAMEBUFFER, chunk_manager.chunks[current_visited_chunk_index].portal.fbo);

			glCullFace(GL_BACK);

			//currently cameraPos - potal.position gets the relative position to the portal for the other camera to match it
			//portal.setup_camera(basic, cameraPos - portal.position, cameraFront);


			basic.setVec3("viewPos", chunk_manager.chunks[current_visited_chunk_index].portal.camera_position);
			basic.setVec3("lightPosition", lightposition);
			basic.setVec3("lightColor", lightColor);
			basic.setVec3("lightDirection", lightDirection);
			basic.setFloat("lowBias", lowBias);
			basic.setFloat("highBias", highBias);
			basic.setFloat("sampleTexel", sample_texel);

			/* Render here */
			glViewport(0, 0, 1920, 1080);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// pass projection matrix to shader (note that in this case it could change every frame)
			//glm::mat4 projection = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, 0.1f, 10000.0f);
			//basic.setMat4("projection", projection);

			// camera/view transformation
			//glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
			//basic.setMat4("view", view);

			model = glm::mat4(1.0f);
			//model = glm::rotate(model, glm::radians(currentFrame), glm::vec3(0.0f, 1.0f, 0.0f));
			basic.setMat4("model", model);
			basic.setMat4("lightSpaceMatrix", lightSpaceMatrix);

			glActiveTexture(GL_TEXTURE0); // Or the correct texture unit
			glBindTexture(GL_TEXTURE_2D_ARRAY, arrayTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			basic.setInt("texture3D", 0);
			basic.setInt("shadowMap", 1);

			if (current_visited_chunk_index) {
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, chunk_manager.chunks[current_visited_chunk_index].portal.texture);
				basic.setInt("portal", 2);
				basic.setFloat("averagePortalBrightness", 0.2);
				basic.setVec3("portalLightPosition", glm::vec3(chunk_manager.chunks[current_visited_chunk_index].portal.position));
			}

			chunk_manager.render_chunks(current_visited_chunk_index);
		}

		
		

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, 1920, 1080);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		basic.setVec3("viewPos", cameraPos);
		
		basic.setMat4("projection", projection);

		// camera/view transformation
		
		basic.setMat4("view", view);

		chunk_manager.render_chunks();

		{
			std::lock_guard<std::mutex> lock(chunk_manager.chunk_mutex);
			if (current_visited_chunk != chunk_manager.chunks.end()) {

				portal_shader.use();
				glViewport(0, 0, 1920, 1080);
				
				model = chunk_manager.chunks[current_visited_chunk_index].portal.model_matrix;
				//model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
				portal_shader.setMat4("model", model);
				portal_shader.setMat4("projection", projection);
				portal_shader.setMat4("view", view);
				chunk_manager.chunks[current_visited_chunk_index].portal.drawQuad();
			}
		}


		

		// input
		// -----
		processInput(window);


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		/* Swap front and back buffers */
		glfwSwapBuffers(window);
	
		
		//chunk_manager.add_pending_chunks();
		//chunk_manager.remove_unload_chunks();
		//chunk_manager.generate_chunk_buffers();

		//renderer.initChunkBuffers(chunk_manager);

		//chunk_manager.load_list_index.clear();
		chunk_manager.frame_counter++;
		//chunk_manager.clear_unload_list();
	}

	glfwTerminate();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

    return 0;
}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		cameraSpeed = static_cast<float>(50.5 * deltaTime);
	}
	else {
		cameraSpeed = static_cast<float>(2.5 * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		if (!cursorCallbackMode) {
			//glfwSetWindowShouldClose(window, true); //closes window on escape
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			//glfwSetCursorPosCallback(window, unlocked_mouse_callback);
			cursorCallbackMode = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (cursorCallbackMode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			//glfwSetCursorPosCallback(window, mouse_callback);
			cursorCallbackMode = false;
		}
	}
	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (!cursorCallbackMode) {
		//cout << xpos << ypos << endl;
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f; // change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}
