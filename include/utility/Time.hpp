#pragma once
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
}