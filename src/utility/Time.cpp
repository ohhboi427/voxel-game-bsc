#include "Time.h"

auto Time::Reset() noexcept -> void
{
	Detail::m_start = Detail::Clock::now();
	Detail::m_current = Detail::m_start;
	Detail::m_last = Detail::m_start;
}

auto Time::Tick() noexcept -> void
{
	Detail::m_last = Detail::m_current;
	Detail::m_current = Detail::Clock::now();
}
