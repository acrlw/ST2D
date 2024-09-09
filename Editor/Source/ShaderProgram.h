#pragma once

#include "ST2DCore.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <string>

namespace STEditor
{
	struct Shader
	{
		std::string source;
		unsigned int id;

		void loadFromFile(const std::string& path)
		{
			std::ifstream file(path);
			if (!file)
			{
				APP_ERROR("Unable to read shader from \"{}\"", path);
				return;
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			source = buffer.str();
			if (source.empty())
				APP_ERROR("'{}' is empty", path);
		}
	};

	class ShaderProgram
	{
	public:
		ShaderProgram() = default;

		void addVertexShader(const std::string& path);
		void addFragmentShader(const std::string& path);
		void addGeometryShader(const std::string& path);

		void addVertexShader(const Shader& vertexShader);
		void addFragmentShader(const Shader& fragmentShader);
		void addGeometryShader(const Shader& geometryShader);

		void compileShader();
		void link();
		void use();
		void destroy();
		unsigned int id() const;

		void setUniform1f(const std::string& name, float value);
		void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void setUniform1i(const std::string& name, int value);
		void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

		void setUniform3f(const std::string& name, float v0, float v1, float v2);

		static void checkShaderCompilation(unsigned int shaderId);
		static void checkProgramCompilation(unsigned int programId);


	private:
		std::vector<Shader> m_vertexShaders;
		std::vector<Shader> m_fragmentShaders;
		std::vector<Shader> m_geometryShaders;

		unsigned int m_id;

	};

}