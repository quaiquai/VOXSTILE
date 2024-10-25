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
	void setBool(std::string &name, bool value) const;

	// ------------------------------------------------------------------------
	void setInt(std::string &name, int value) const;

	// ------------------------------------------------------------------------
	void setFloat(std::string &name, float value) const;

	// ------------------------------------------------------------------------
	void setVec2(std::string &name, const glm::vec2 &value) const;

	void setVec2(std::string &name, float x, float y) const;

	// ------------------------------------------------------------------------
	void setVec3(std::string &name, const glm::vec3 &value) const;

	void setVec3(std::string &name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	void setVec4(std::string &name, const glm::vec4 &value) const;

	void setVec4(std::string &name, float x, float y, float z, float w) const;

	// ------------------------------------------------------------------------
	void setMat2(std::string &name, const glm::mat2 &mat) const;

	// ------------------------------------------------------------------------
	void setMat3(std::string &name, const glm::mat3 &mat) const;

	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const;


private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void checkCompileErrors(GLuint shader, std::string type);
};
#endif