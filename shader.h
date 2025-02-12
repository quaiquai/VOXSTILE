#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>


class Shader
{
public:
	//the id of the shader for updating and sending information
	unsigned int ID;

	// constructor generates a basic shader on the fly
	// ------------------------------------------------------------------------
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	//constructor for a compute shader
	//------------------------------------------------------------------------
	Shader(const char* computePath);


	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value);

	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value);

	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value);

	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value);

	void setVec2(const std::string &name, float x, float y);

	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value);

	void setVec3(const std::string &name, float x, float y, float z);

	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value);

	void setVec4(const std::string &name, float x, float y, float z, float w);

	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat);

	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat);

	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat);


private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type);
};
#endif