#pragma once

#include "ST2DCore.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace STEditor
{
	struct Shader
	{
		std::string source;
		unsigned int id;
	};

	class ShaderProgram
	{
	public:
		ShaderProgram() = default;

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