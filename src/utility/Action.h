#pragma once

#include <functional>
#include <utility>
#include <vector>

/**
 * @brief Holds multiple callbacks of the same argument list.
 * 
 * @tparam ...TArgs The types of the arguments'.
 */
template<typename... TArgs>
class Action
{
public:
	/**
	 * @brief Copies in a new callback.
	 * 
	 * @param callback The callback.
	 */
	auto operator+=(const std::function<void(TArgs...)>& callback) -> void
	{
		m_callbacks.push_back(callback);
	}

	/**
	 * @brief Moves in a new callback.
	 * 
	 * @param callback The callback.
	 */
	auto operator+=(std::function<void(TArgs...)>&& callback) -> void
	{
		m_callbacks.push_back(move(callback));
	}

	/**
	 * @brief Calls all of the callbacks.
	 * 
	 * @param ...args The arguments passed to the callbacks.
	 */
	auto operator()(const TArgs&... args) -> void
	{
		for(auto& callback : m_callbacks)
		{
			callback(args...);
		}
	}

private:
	std::vector<std::function<void(TArgs...)>> m_callbacks;
};
