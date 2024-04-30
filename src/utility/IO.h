#pragma once

#include <filesystem>
#include <string>

/**
 * @brief Loads a text file.
 * 
 * @param path The path of the file.
 * 
 * @return A string containing the text.
 */
[[nodiscard]] auto LoadTextFile(const std::filesystem::path& path) -> std::string;
