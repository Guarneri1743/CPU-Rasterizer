#include "Time.hpp"

namespace Guarneri
{
	float Time::fps;
	float Time::frame_cost;
	int Time::frame_count;
	std::chrono::steady_clock::time_point Time::frame_start_time;
	std::chrono::steady_clock::time_point Time::start_up_time;
	std::chrono::steady_clock::time_point Time::stop_watch_start_time;

	void Time::start_watch()
	{
		stop_watch_start_time = std::chrono::steady_clock::now();
	}

	float Time::stop_watch()
	{
		return (float)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - stop_watch_start_time).count() / 1e+3f;
	}

	void Time::start()
	{
		start_up_time = std::chrono::steady_clock::now();
	}

	void Time::frame_start()
	{
		frame_start_time = std::chrono::steady_clock::now();
	}

	void Time::frame_end()
	{
		frame_count++;
		float cost = (float)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - frame_start_time).count() / 1e+6f;
		fps = 1.0f / cost;
		frame_cost = cost * 1e+3f;
	}
}