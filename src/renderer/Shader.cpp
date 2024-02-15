#include "Shader.h"

#include <glad/gl.h>

#include <fstream>

#include <vector>
#include <string>

static auto loadTextFile(const std::filesystem::path& path) -> std::string
{
	std::string text;

	if(std::ifstream file(path); file.good())
	{
		file.seekg(0, std::ios::end);

		if(std::streampos size = file.tellg(); size != -1)
		{
			file.seekg(0, std::ios::beg);

			text.resize(static_cast<size_t>(size));
			file.read(text.data(), static_cast<std::streamsize>(size));
		}
	}

	return text;
}

Shader::Shader(const Sources& sources)
{
	m_handle = glCreateProgram();

	std::vector<GLuint> shaders;
	for(const auto& [type, path] : sources)
	{
		GLuint shader = shaders.emplace_back(glCreateShader(type));

		std::string source = loadTextFile(path);
		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, nullptr);

		glCompileShader(shader);

		glAttachShader(m_handle, shader);
	}

	glLinkProgram(m_handle);

	for(const auto& shader : shaders)
	{
		glDetachShader(m_handle, shader);
		glDeleteShader(shader);
	}
}

Shader::~Shader()
{
	glDeleteProgram(m_handle);
}

void Shader::Use() const noexcept
{
	glUseProgram(m_handle);
}
