#ifndef _FPS_STATISTIC_
#define _FPS_STATISTIC_
#include <chrono>

namespace Guarneri
{
	class Time
	{
	public:
		static float fps;
		static int frame_count;
		static float frame_cost;
		static std::chrono::steady_clock::time_point frame_start_time;
		static std::chrono::steady_clock::time_point start_up_time;
		static std::chrono::steady_clock::time_point stop_watch_start_time;

	public:
		static void start_watch();
		static float stop_watch();
		static void start();
		static void frame_start();
		static void frame_end();
	};


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

#endif