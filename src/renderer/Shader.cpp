#include "Shader.h"

#include <glad/gl.h>

#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace
{
	auto loadTextFile(const std::filesystem::path& path) -> std::string
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

	auto loadShaderSourceFile(const std::filesystem::path& path) -> std::string
	{
		static const std::regex rule("#include \"([a-zA-Z]+.[a-zA-Z]+)\"");

		std::string source = loadTextFile(path);
		std::filesystem::path parent = path.parent_path();

		std::string result;
		std::smatch match;
		std::string::const_iterator searchStart = source.cbegin();

		bool hasInclude = false;
		while(std::regex_search(searchStart, source.cend(), match, rule))
		{
			hasInclude = true;

			result += match.prefix().str();
			result += loadShaderSourceFile(parent / match[1u].str());

			searchStart = match.suffix().first;
		}

		if(!hasInclude)
		{
			return source;
		}

		result += match.suffix().str();
		return result;
	}
}

Shader::Shader(const Sources& sources)
{
	m_handle = glCreateProgram();

	std::vector<GLuint> shaders;
	for(const auto& [type, path] : sources)
	{
		GLuint shader = shaders.emplace_back(glCreateShader(type));

		std::string source = loadShaderSourceFile(path);
		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, nullptr);

		glCompileShader(shader);

		GLint status;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if(status != GL_TRUE)
		{
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string str(static_cast<size_t>(length) - 1u, '\0');
			glGetShaderInfoLog(shader, length, nullptr, str.data());

			printf("%s\n", str.c_str());
		}

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
