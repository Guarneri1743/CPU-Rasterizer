#ifndef _FPS_STATISTIC_
#define _FPS_STATISTIC_
#include <chrono>

namespace Guarneri {
	class Time {
	public:
		static float fps;
		static int frame_count;

		static std::chrono::steady_clock::time_point frame_start_time;
		static std::chrono::steady_clock::time_point start_up_time;
		static std::chrono::steady_clock::time_point stop_watch_start_time;

		static void start_watch() {
			stop_watch_start_time = std::chrono::steady_clock::now();
		}

		static float stop_watch() {
			return (float)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - stop_watch_start_time).count() / (float)1000.0f;
		}

		static void start() {
			start_up_time = std::chrono::steady_clock::now();
		}

		static void frame_start() {
			frame_start_time = std::chrono::steady_clock::now();
		}

		static void frame_end() {
			frame_count++;
			float cost = (float)(std::chrono::steady_clock::now() - frame_start_time).count() / 1e+9f;
			fps = 1.0f / cost;
		}
	};

	float Time::fps;
	int Time::frame_count;
	std::chrono::steady_clock::time_point Time::frame_start_time;
	std::chrono::steady_clock::time_point Time::start_up_time;
	std::chrono::steady_clock::time_point Time::stop_watch_start_time;
}

#endif