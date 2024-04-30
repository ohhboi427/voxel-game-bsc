#include "IO.h"

#include <fstream>

auto LoadTextFile(const std::filesystem::path& path) -> std::string
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
