#include "Config.h"

#include <toml++/toml.hpp>

#include <fstream>
#include <string>

namespace
{
	constexpr auto ConfigFilePath = "config.toml";

	toml::table s_table;
}

auto Config::Load() -> void
{
	s_table = toml::parse_file(ConfigFilePath);
}

auto Config::Save() -> void
{
	std::ofstream file(ConfigFilePath);

	file << s_table << std::endl;
}

template<>
auto Config::Get<int64_t>(std::string_view table, std::string_view key) -> int64_t&
{
	return s_table[table][key].as_integer()->get();
}

template<>
auto Config::Get<bool>(std::string_view table, std::string_view key) -> bool&
{
	return s_table[table][key].as_boolean()->get();
}

template<>
auto Config::Get<std::string>(std::string_view table, std::string_view key) -> std::string&
{
	return s_table[table][key].as_string()->get();
}
