#include "ShaderProgram.h"


namespace STEditor
{
    void ShaderProgram::addVertexShader(const Shader& vertexShader)
    {
        m_vertexShaders.emplace_back(vertexShader);
    }

    void ShaderProgram::addFragmentShader(const Shader& fragmentShader)
    {
        m_fragmentShaders.emplace_back(fragmentShader);
    }

	void ShaderProgram::addGeometryShader(const Shader& geometryShader)
	{
		m_geometryShaders.emplace_back(geometryShader);
	}

    void ShaderProgram::compileShader()
    {
		APP_INFO("Compiling shaders");
        for (auto&& vertexShader : m_vertexShaders)
        {
            vertexShader.id = glCreateShader(GL_VERTEX_SHADER);
            const char* vertexShaderSource = vertexShader.source.c_str();
            glShaderSource(vertexShader.id, 1, &vertexShaderSource, NULL);
            glCompileShader(vertexShader.id);

            checkShaderCompilation(vertexShader.id);
        }

        for (auto&& fragmentShader : m_fragmentShaders)
        {
            fragmentShader.id = glCreateShader(GL_FRAGMENT_SHADER);
            const char* fragmentShaderSource = fragmentShader.source.c_str();
            glShaderSource(fragmentShader.id, 1, &fragmentShaderSource, NULL);
            glCompileShader(fragmentShader.id);

            checkShaderCompilation(fragmentShader.id);
        }

		//for (auto&& geometryShader : m_geometryShaders)
		//{
		//	geometryShader.id = glCreateShader(GL_GEOMETRY_SHADER);
		//	const char* geometryShaderSource = geometryShader.source.c_str();
		//	glShaderSource(geometryShader.id, 1, &geometryShaderSource, NULL);
		//	glCompileShader(geometryShader.id);

		//	checkShaderCompilation(geometryShader.id);
		//}

		APP_INFO("Shaders compiled");
    }


    void ShaderProgram::link()
    {
		APP_INFO("Linking program");

        m_id = glCreateProgram();
        for (auto&& vertexShader : m_vertexShaders)
            glAttachShader(m_id, vertexShader.id);

        for (auto&& fragmentShader : m_fragmentShaders)
            glAttachShader(m_id, fragmentShader.id);

		//for (auto&& geometryShader : m_geometryShaders)
		//	glAttachShader(m_id, geometryShader.id);

        glLinkProgram(m_id);

        checkProgramCompilation(m_id);

		APP_INFO("Program linked");
    }


    void ShaderProgram::use()
    {
        glUseProgram(m_id);
    }

    void ShaderProgram::destroy()
    {
        for (auto&& vertexShader : m_vertexShaders)
            glDeleteShader(vertexShader.id);

        for (auto&& fragmentShader : m_fragmentShaders)
            glDeleteShader(fragmentShader.id);

		//for (auto&& geometryShader : m_geometryShaders)
		//	glDeleteShader(geometryShader.id);

        glDeleteProgram(m_id);

		APP_INFO("Shader program destroyed");

    }

    unsigned int ShaderProgram::id() const
    {
        return m_id;
    }

    void ShaderProgram::setUniform1f(const std::string& name, float value)
    {
        int location = glGetUniformLocation(m_id, name.c_str());
        glUniform1f(location, value);
    }

    void ShaderProgram::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        int location = glGetUniformLocation(m_id, name.c_str());
        glUniform4f(location, v0, v1, v2, v3);

    }

    void ShaderProgram::setUniform1i(const std::string& name, int value)
    {
        int location = glGetUniformLocation(m_id, name.c_str());
        glUniform1i(location, value);
    }


    void ShaderProgram::checkShaderCompilation(unsigned int shaderId)
    {
        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			APP_ERROR("Shader compilation failed: {0}", infoLog);
        }
        else
        {
			APP_INFO("Shader ({}) compilation succeeded", shaderId);
        }
    }


    void ShaderProgram::checkProgramCompilation(unsigned int programId)
    {
        int success;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
			APP_ERROR("Program compilation failed: {0}", infoLog);
        }
        else
			APP_INFO("Program ({}) compilation succeeded", programId);

    }
    void ShaderProgram::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
    {
        int location = glGetUniformLocation(m_id, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void ShaderProgram::setUniform3f(const std::string& name, float v0, float v1, float v2)
    {
        int location = glGetUniformLocation(m_id, name.c_str());
        glUniform3f(location, v0, v1, v2);
    }
}