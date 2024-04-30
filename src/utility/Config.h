#pragma once

#include <string_view>

namespace Config
{
	/**
	 * @brief Loads the configs from the file overwriting the current state.
	 */
	auto Load() -> void;

	/**
	 * @brief Saves the current state of the configs to the file.
	 */
	auto Save() -> void;

	/**
	 * @brief Retrieves a value from the config file.
	 * 
	 * @tparam T Only support 'int64_t', 'bool' and 'std::string'.
	 * 
	 * @param table The table of the value.
	 * @param key The key of the value.
	 * 
	 * @return A reference to the value.
	 */
	template<typename T>
	auto Get(std::string_view table, std::string_view key) -> T&;
}
