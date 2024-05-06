#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>

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
	 * @tparam T The type of the value. Only support 'int64_t', 'double' 'bool' and 'std::string'.
	 * 
	 * @param table The table of the value.
	 * @param key The key of the value.
	 * 
	 * @return A reference to the value.
	 */
	template<typename T>
		requires (std::disjunction_v<
			std::is_same<T, int64_t>,
			std::is_same<T, double>,
			std::is_same<T, bool>,
			std::is_same<T, std::string>>)
	auto Get(std::string_view table, std::string_view key) -> T&;
}
