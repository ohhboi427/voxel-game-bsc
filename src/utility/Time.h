#pragma once

#include <chrono>

namespace Time
{
	namespace Detail
	{
		using Clock = std::chrono::high_resolution_clock;
		using TimePoint = Clock::time_point;

		inline TimePoint m_start;
		inline TimePoint m_current;
		inline TimePoint m_last;
	}

	/**
	 * @brief Retrieves the duration between the current frame and the last reset.
	 *
	 * @tparam T The type of the duration value. by default it is 'float'.
	 * @tparam R The period of the duration. By default it is a second.
	 *
	 * @return The time between the current frame and the last reset.
	 */
	template<typename T = float, typename R = std::ratio<1, 1>>
	[[nodiscard]] inline auto GetElapsedTime() noexcept -> T
	{
		return std::chrono::duration_cast<std::chrono::duration<T, R>>(
			Detail::m_current - Detail::m_start
		).count();
	}

	/**
	 * @brief Retrieves the duration between the current and previous frame.
	 * 
	 * @tparam T The type of the duration value. by default it is 'float'.
	 * @tparam R The period of the duration. By default it is a second.
	 * 
	 * @return The time between the current and previous frame.
	 */
	template<typename T = float, typename R = std::ratio<1, 1>>
	[[nodiscard]] inline auto GetDeltaTime() noexcept -> T
	{
		return std::chrono::duration_cast<std::chrono::duration<T, R>>(
			Detail::m_current - Detail::m_last
		).count();
	}

	/**
	 * @brief Resets the internal clock.
	 *
	 * Sets the start time to the current time.
	 */
	auto Reset() noexcept -> void;

	/**
	 * @brief Updates the internal clock.
	 *
	 * Updates the current time.
	 */
	auto Tick() noexcept -> void;
};
